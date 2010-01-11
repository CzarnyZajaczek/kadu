/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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
 */

#include <QtGui/QDrag>

#include "gui/widgets/buddies-list-widget.h"
#include "gui/windows/open-chat-with/open-chat-with.h"

#include "activate.h"

#include "configuration/configuration-file.h"
#include "gui/hot-key.h"
#include "icons-manager.h"
#include "misc/misc.h"

#include "tabwidget.h"

TabWidget::TabWidget()
{
	tabbar = new TabBar(this);
	tabbar->setMovable(true);

	setTabBar(tabbar);
	//akceptujemy dnd
	setAcceptDrops(true);
	connect(tabbar, SIGNAL(contextMenu(int, const QPoint&)),
			SLOT(onContextMenu(int, const QPoint&)));
	connect(tabbar, SIGNAL(tabCloseRequested(int)),
			SLOT(onDeleteTab(int)));
	connect(tabbar,SIGNAL(mouseDoubleClickEventSignal(QMouseEvent*)),
			SLOT(mouseDoubleClickEvent(QMouseEvent*)));
	//przycisk otwarcia nowej karty pokazywany w lewym g��rnym rogu

	openChatButton = new QToolButton(this);
   	openChatButton->setIcon(IconsManager::instance()->loadIcon("OpenChat"));
	setCornerWidget(openChatButton, Qt::TopLeftCorner);
	connect(openChatButton, SIGNAL(clicked()), SLOT(newChat()));
	openChatButton->setAutoRaise(true);

	//przycisk zamkni�0�1cia aktywnej karty znajduj�0�2cy si�0�1 w prawym g��rnym rogu
	closeChatButton = new QToolButton(this);
   	closeChatButton->setIcon(IconsManager::instance()->loadIcon("TabsRemove"));
	setCornerWidget(closeChatButton, Qt::TopRightCorner);
	connect(closeChatButton, SIGNAL(clicked()), SLOT(deleteTab()));
	closeChatButton->setAutoRaise(true);
	openChatWithWindow = NULL;
}

void TabWidget::closeChatWidget(ChatWidget *chat)
{
	if (chat)
		delete chat;
}

void TabWidget::closeEvent(QCloseEvent *e)
{
	//w zale�0�4no�0�2ci od opcji w konfiguracji zamykamy wszystkie karty, lub tylko aktywn�0�2
	if (config_oldStyleClosing)
	{
		QWidget *current = currentWidget();
		delete current;

	}
	else
	{
		//dop��ki s�0�2 jeszcze karty zamykamy aktywn�0�2
		while(count())
		{
			QWidget* current = currentWidget();
			delete current;
		}
	}

	if (count() > 0)
		e->ignore();
	else
		e->accept();
}

void TabWidget::chatKeyPressed(QKeyEvent *e, CustomInput *k, bool &handled)
{
	handled = true;
	// obs�0�0uga skr��t��w klawiszowych
	if (HotKey::shortCut(e, "ShortCuts", "MoveTabLeft"))
		moveTabLeft();
	else if(HotKey::shortCut(e, "ShortCuts", "MoveTabRight"))
		moveTabRight();
	else if(HotKey::shortCut(e, "ShortCuts", "SwitchTabLeft"))
		switchTabLeft();
	else if(HotKey::shortCut(e, "ShortCuts", "SwitchTabRight"))
		switchTabRight();
	else
		// skr��t nie zosta�0�0 znaleziony i wykonany. Przekazujemy zdarzenie dalej
		handled = false;
}

void TabWidget::onContextMenu(int id, const QPoint &pos)
{
	emit contextMenu(widget(id), pos);
}

void TabWidget::moveTab(int from, int to)
{
	kdebugf();
	QString tablabel=tabText(from);
	QWidget *w = widget(from);
	QIcon tabiconset = tabIcon(from);
	QString tabtooltip = tabToolTip(from);
	bool current = (w == currentWidget());
	blockSignals(true);
	removeTab(from);

	insertTab(to, w, tabiconset, tablabel);
	setTabToolTip(to, tabtooltip);

	if (current)
		setCurrentIndex(to);

	blockSignals(false);
}

void TabWidget::onDeleteTab(int id)
{
	QWidget *chat = widget(id);
	delete chat;
}

void TabWidget::switchTabLeft()
{
	if (currentIndex() == 0)
		setCurrentIndex(count() - 1);
	else
		setCurrentIndex(currentIndex()-1);
}

void TabWidget::switchTabRight()
{
	if (currentIndex() == (count() - 1))
		setCurrentIndex(0);
	else
		setCurrentIndex(currentIndex()+1);
}

void TabWidget::moveTabLeft()
{
	if (currentIndex() == 0)
		moveTab(0, count() - 1);
	else
		moveTab(currentIndex(), currentIndex() - 1);
}

void TabWidget::moveTabRight()
{
	if (currentIndex() == (count() - 1))
		moveTab(count() - 1, 0);
	else
		moveTab(currentIndex(), currentIndex() + 1);
}

void TabWidget::dragEnterEvent(QDragEnterEvent* e)
{
	kdebugf();
	// Akceptujemu dnd je�0�2li pochodzi on z UserBox'a lub paska kart
// 	if ((UlesDrag::canDecode(e) && (dynamic_cast<ContactsListWidget *>(e->source()))))
// 		e->acceptProposedAction();
// 	else
		e->ignore();
//
	kdebugf2();
}

void TabWidget::dropEvent(QDropEvent* e)
{
	kdebugf();
	QStringList ules;

	// Je�0�2li dnd pochodzi�0�0 z userboxa pr��bujemy doda�0�4 now�0�2 kart�0�1
	if (dynamic_cast<BuddiesListWidget *>(e->source()) && false)/*UlesDrag::decode(e, ules))*/
	{
		if (tabbar->tabAt(e->pos()) != -1)
		// Je�0�2li w miejscu upuszczenia jest karta, dodajemy na jej pozycji
			emit openTab(ules, tabbar->tabAt(e->pos()));
		else
		// Je�0�2li nie na ko��cu tabbara
			emit openTab(ules, -1);
	}

	kdebugf2();
}

void TabWidget::windowActivationChange(bool oldActive)
{
	kdebugf();
	ChatWidget *chat = dynamic_cast<ChatWidget*>(currentWidget());
	if (_isActiveWindow(this) && !oldActive && chat)
	{
		chat->markAllMessagesRead();
		emit chatWidgetActivated(chat);
	}
	kdebugf2();
}

void TabWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
	kdebugf();
	// je�0�2li dwuklik nast�0�2pil lewym przyciskiem myszy pokazujemy okno openchatwith
	if (e->button() == Qt::LeftButton)
		newChat();
	kdebugf2();
}

void TabWidget::newChat()
{
	// je�0�2li okno openchatwith nie istnieje tworzymy nowe
	if (!openChatWithWindow)
	{
		openChatWithWindow = new OpenChatWith;
   		connect(openChatWithWindow, SIGNAL(destroyed()), this, SLOT(openChatWithWindowClose()));
		// zapisujemy geometri�0�1 okna, aby j�0�2 p���0�2niej przywr��ci�0�4 (tabsy nie b�0�1d�0�2 psu�0�0y pozycji okna z akcji kadu)
		openTabWithGeometry = openChatWithWindow->frameGeometry();
		// pokazujemy je w miejscu w kt��rym nast�0�2pi�0�0o wywo�0�0anie metody (obecnie znajduje si�0�1 kursor)
		openChatWithWindow->setGeometry(QCursor::pos().x(), QCursor::pos().y(), openChatWithWindow->width(), openChatWithWindow->height());// jak wykonuje si�0�1 to po pokazaniu okienkoa wyst�0�1puje denerwuj�0�2ce miganie
		openChatWithWindow->show();
	}
	// je�0�2li istnieje przywracamy je na pierwszy plan w miejscu cursora
	else
	{
		openChatWithWindow->setGeometry(QCursor::pos().x(), QCursor::pos().y(), openChatWithWindow->width(), openChatWithWindow->height());// jak wykonuje si�0�1 to po pokazaniu okienka wyst�0�1puje denerwuj�0�2ce miganie
		openChatWithWindow->setWindowState(openChatWithWindow->windowState() & Qt::WindowMinimized);
		openChatWithWindow->raise();
	}
}

void TabWidget::deleteTab()
{
	// zamykamy bie�0�4�0�2c�0�2 kart�0�1
	QWidget *current = currentWidget();
	delete current;
}

void TabWidget::tabInserted(int index)
{
	show();
}

void TabWidget::tabRemoved(int index)
{
	if (count() == 0)
		hide();
}

void TabWidget::openChatWithWindowClose()
{
	// ponownie zapisujemy poprawn�0�2 pozycj�0�1 okna openTabWith
	config_file.writeEntry("General", "OpenChatWith", openTabWithGeometry);
	// po zamkni�0�1ciu okna openchatwith przypisujemy NULL do wska�0�2nika do niego,
	openChatWithWindow = 0;
}

void TabWidget::configurationUpdated()
{
	// od�0�2wie�0�4enie ikon
   	openChatButton->setIcon(IconsManager::instance()->loadIcon("OpenChat"));
   	closeChatButton->setIcon(IconsManager::instance()->loadIcon("TabsRemove"));

	tabbar->setTabsClosable(config_file.readBoolEntry("Tabs", "CloseButtonOnTab"));

	// uaktualniamy zmienne konfiguracyjne
	closeChatButton->setShown(config_file.readBoolEntry("Tabs", "CloseButton"));
	openChatButton->setShown(config_file.readBoolEntry("Tabs", "OpenChatButton"));
	config_oldStyleClosing = config_file.readBoolEntry("Tabs", "OldStyleClosing");
}

TabBar::TabBar(QWidget *parent)
	: QTabBar(parent)
{
}

void TabBar::mousePressEvent(QMouseEvent* e)
{
	if (tabAt(e->pos()) != -1 && e->button() == Qt::RightButton)
		emit contextMenu(tabAt(e->pos()), mapToGlobal(e->pos()));

	QTabBar::mousePressEvent(e);
}

void TabBar::mouseReleaseEvent(QMouseEvent* e)
{
	if (tabAt(e->pos()) != -1 && e->button() == Qt::MidButton)
		emit tabCloseRequested(tabAt(e->pos()));
	QTabBar::mouseReleaseEvent(e);
}

void TabBar::mouseDoubleClickEvent(QMouseEvent* e)
{
	kdebugf();
	// w celu u�0�0atwienia sobie zadania przekazujemy zdarzenie dalej- tu klasie tabdialog
	emit mouseDoubleClickEventSignal(e);
	kdebugf2();
}
