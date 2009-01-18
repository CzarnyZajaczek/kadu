/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpinBox>
#include <QtGui/QComboBox>
#include <QtGui/QGridLayout>
#include <QtGui/QApplication>

#include "config_file.h"
#include "debug.h"
#include "kadu.h"

#include "qt4_docking_notify_configuration_widget.h"

Qt4NotifyConfigurationWidget::Qt4NotifyConfigurationWidget(QWidget *parent, char *name)
	: NotifierConfigurationWidget(parent, name), currentNotifyEvent("")
{
	QString tooltip = qApp->translate("@default", Kadu::SyntaxTextNotify) +
		tr("\n%&t - title (eg. New message) %&m - notification text (eg. Message from Jim), %&d - details (eg. message quotation),\n%&i - notification icon");

	timeout = new QSpinBox(this);
	timeout->setSuffix(" s");
	timeout->setMinimum(1);
	timeout->setMaximum(100);

	title = new QLineEdit(this);
	title->setToolTip(tooltip);

	syntax = new QLineEdit(this);
	syntax->setToolTip(tooltip);

	QStringList iconNames;
	iconNames << tr("NoIcon") << tr("Information") << tr("Warning") << tr("Critical");
	icon = new QComboBox(this);
	icon->addItems(iconNames);

	connect(timeout, SIGNAL(valueChanged(int)), this, SLOT(timeoutChanged(int)));
	connect(syntax, SIGNAL(textChanged(const QString &)), this, SLOT(syntaxChanged(const QString &)));
	connect(title, SIGNAL(textChanged(const QString &)), this, SLOT(titleChanged(const QString &)));
	connect(icon, SIGNAL(currentIndexChanged(int)), this, SLOT(iconChanged(int)));

	QGridLayout *gridLayout = new QGridLayout(this, 0, 0, 0, 5);
	gridLayout->addWidget(new QLabel(tr("Timeout") + ":", this), 1, 0, Qt::AlignRight);
	gridLayout->addWidget(timeout, 1, 1);
	gridLayout->addWidget(new QLabel(tr("Notification Icon") + ":", this), 2, 0, Qt::AlignRight);
	gridLayout->addWidget(icon, 2, 1);
	gridLayout->addWidget(new QLabel(tr("Title") + ":", this), 3, 0, Qt::AlignRight);
	gridLayout->addWidget(title, 3, 1);
	gridLayout->addWidget(new QLabel(tr("Syntax") + ":", this), 4, 0, Qt::AlignRight);
	gridLayout->addWidget(syntax, 4, 1);

	parent->layout()->addWidget(this);
}

void Qt4NotifyConfigurationWidget::saveNotifyConfigurations()
{
	kdebugf();

	if (currentNotifyEvent != "")
		properties[currentNotifyEvent] = currentProperties;

	foreach(const Qt4NotifyProperties &property, properties)
	{
		const QString &eventName = property.eventName;

		config_file.writeEntry("Qt4DockingNotify", QString("Event_") + eventName + "_timeout", (int)property.timeout);
		config_file.writeEntry("Qt4DockingNotify", QString("Event_") + eventName + "_syntax", property.syntax);
		config_file.writeEntry("Qt4DockingNotify", QString("Event_") + eventName + "_title", property.title);
		config_file.writeEntry("Qt4DockingNotify", QString("Event_") + eventName + "_icon", (int)property.icon);
	}
}

void Qt4NotifyConfigurationWidget::switchToEvent(const QString &event)
{
	kdebugf();

	if (currentNotifyEvent != "")
		properties[currentNotifyEvent] = currentProperties;

	if (properties.contains(event))
	{
		currentProperties = properties[event];
		currentNotifyEvent = event;
	}
	else
	{
		currentNotifyEvent = event;
		currentProperties.eventName = event;

		currentProperties.timeout = config_file.readUnsignedNumEntry("Qt4DockingNotify", QString("Event_") + event + "_timeout", 10);
		currentProperties.syntax = config_file.readEntry("Qt4DockingNotify", QString("Event_") + event + "_syntax");
		currentProperties.title = config_file.readEntry("Qt4DockingNotify", QString("Event_") + event + "_title");
		currentProperties.icon = config_file.readUnsignedNumEntry("Qt4DockingNotify", QString("Event_") + event + "_icon", 0);
	}

	timeout->setValue(currentProperties.timeout);
	syntax->setText(currentProperties.syntax);
	title->setText(currentProperties.title);
	icon->setCurrentIndex(currentProperties.icon);
}

void Qt4NotifyConfigurationWidget::timeoutChanged(int timeout)
{
	currentProperties.timeout = timeout;
}

void Qt4NotifyConfigurationWidget::syntaxChanged(const QString &syntax)
{
	currentProperties.syntax = syntax;
}

void Qt4NotifyConfigurationWidget::titleChanged(const QString &title)
{
	currentProperties.title = title;
}

void Qt4NotifyConfigurationWidget::iconChanged(int index)
{
	currentProperties.icon = index;
}
