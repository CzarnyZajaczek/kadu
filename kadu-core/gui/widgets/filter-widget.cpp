/*
 * %kadu copyright begin%
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * Mac OS X implementation based on searchbox class by Matteo Bertozzi:
 * http://th30z.netsons.org/2008/08/qt4-mac-searchbox-wrapper/
 */

#include <QtGui/QApplication>
#include <QtGui/QHBoxLayout>
#include <QtGui/QKeyEvent>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include "gui/widgets/buddies-list-view.h"
#include "gui/widgets/line-edit-with-clear-button.h"

#include "filter-widget.h"

#ifdef Q_OS_MAC
#include <QtCore/QVarLengthArray>

static QString toQString(CFStringRef str)
{
	if(!str)
		return QString();

	CFIndex length = CFStringGetLength(str);
	const UniChar *chars = CFStringGetCharactersPtr(str);
	if (chars)
		return QString(reinterpret_cast<const QChar *>(chars), length);

	QVarLengthArray<UniChar> buffer(length);
	CFStringGetCharacters(str, CFRangeMake(0, length), buffer.data());
	return QString(reinterpret_cast<const QChar *>(buffer.constData()), length);
}

static OSStatus FilterFieldEventHandler(EventHandlerCallRef handlerCallRef,
					EventRef event, void *userData)
{
	Q_UNUSED(handlerCallRef);
	FilterWidget *filter = (FilterWidget *) userData;
	OSType eventClass = GetEventClass(event);
	UInt32 eventKind = GetEventKind(event);

	if (eventClass == kEventClassSearchField && eventKind == kEventSearchFieldCancelClicked)
		filter->clear();
	else if (eventClass == kEventClassTextField && eventKind == kEventTextDidChange)
		filter->emitTextChanged();
	return (eventNotHandledErr);
}

void FilterWidget::emitTextChanged(void)
{
	emit textChanged(text());
}

void FilterWidget::clear(void)
{
	setText(QString());
}

QString FilterWidget::text(void) const
{
	CFStringRef cfString = HIViewCopyText(searchField);
	QString text = toQString(cfString);
	CFRelease(cfString);
	return(text);
}

void FilterWidget::setText(const QString &text)
{
	CFRelease(searchFieldText);
	searchFieldText = CFStringCreateWithCString(0,
			  (const char *)text.toAscii(), 0);
	HIViewSetText(searchField, searchFieldText);
	emit textChanged(text);
}

void FilterWidget::activate(void)
{
	HIViewAdvanceFocus(searchField, 0);
	/* Dorr: I don't know the Carbon API. The HIViewAdvanceFocus was the only one
	 * method to pass the focus to searchbox. However I have to reset the text here
	 * otherwise some letters will be duplicated */
	setText(text());
}

QSize FilterWidget::sizeHint (void) const
{
	HIRect optimalBounds;
	EventRef event;
	CreateEvent(0, kEventClassControl, kEventControlGetOptimalBounds,
		GetCurrentEventTime(), kEventAttributeUserEvent, &event);
	SendEventToEventTargetWithOptions(event,
		HIObjectGetEventTarget(HIObjectRef(winId())),
		kEventTargetDontPropagate);
	GetEventParameter(event, kEventParamControlOptimalBounds, typeHIRect,
		0, sizeof(HIRect), 0, &optimalBounds);
	ReleaseEvent(event);
	return QSize(optimalBounds.size.width + 200, optimalBounds.size.height);
}
#endif

void FilterWidget::filterTextChanged(const QString &s)
{
#ifdef Q_OS_MAC
	Q_UNUSED(s);
#else
	emit textChanged(s);

	if (NameFilterEdit->text().isEmpty())
	{
		QModelIndexList selection = View->selectionModel()->selectedIndexes();
		if (!selection.isEmpty())
		{
			qSort(selection);
			View->scrollTo(selection.first());
		}
		View->setFocus(Qt::OtherFocusReason);
		hide();
	}
	else
	{
		if (!isVisible() || View->selectionModel()->selectedIndexes().count() == 0)
		{
			View->setCurrentIndex(View->model()->index(0, 0));
			View->selectionModel()->select(View->model()->index(0, 0), QItemSelectionModel::SelectCurrent);
		}
		show();
	}
#endif
}

FilterWidget::FilterWidget(QWidget *parent) : QWidget(parent)
{
#ifdef Q_OS_MAC

	searchFieldText = CFStringCreateWithCString(0,
		(const char *) tr("Search").toAscii(), 0);
	HISearchFieldCreate(NULL, kHISearchFieldAttributesSearchIcon |
		kHISearchFieldAttributesCancel,
		NULL, searchFieldText, &searchField);
	create(reinterpret_cast<WId>(searchField));
	EventTypeSpec mySFieldEvents[] = {
		{ kEventClassSearchField, kEventSearchFieldCancelClicked },
		{ kEventClassTextField, kEventTextDidChange },
		{ kEventClassTextField, kEventTextAccepted }
	};
	HIViewInstallEventHandler(searchField, FilterFieldEventHandler,
		GetEventTypeCount(mySFieldEvents), mySFieldEvents,
		(void *) this, NULL);

#elif !defined(Q_WS_MAEMO_5)

	View = 0;

	QHBoxLayout *layout = new QHBoxLayout(this);
	layout->setMargin(3);

	NameFilterEdit = new LineEditWithClearButton(this);

	setFocusProxy(NameFilterEdit);

	layout->addWidget(new QLabel(tr("Filter") + ':', this));
	layout->addWidget(NameFilterEdit);

	connect(NameFilterEdit, SIGNAL(textChanged(const QString &)),
			this, SLOT(filterTextChanged(const QString &)));
#endif
}

FilterWidget::~FilterWidget()
{
#ifdef Q_OS_MAC
	CFRelease(searchField);
	CFRelease(searchFieldText);
#endif
}

void FilterWidget::setFilter(const QString &filter)
{
#ifndef Q_OS_MAC
	NameFilterEdit->setText(filter);
#endif
}

void FilterWidget::setView(BuddiesListView *view)
{
#ifdef Q_OS_MAC
	Q_UNUSED(view);
#else
	View = view;
#endif
}

#ifndef Q_OS_MAC
bool FilterWidget::sendKeyEventToView(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Enter:
		case Qt::Key_Return:
		case Qt::Key_Down:
		case Qt::Key_Up:
		case Qt::Key_PageDown:
		case Qt::Key_PageUp:
			qApp->sendEvent(View, event);
			return true;
	}

	return false;
}

void FilterWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
		case Qt::Key_Escape:
			setFilter("");
			event->accept();
			return;
	}

	if (View && sendKeyEventToView(event))
		return;

	QWidget::keyPressEvent(event);
}

#endif
