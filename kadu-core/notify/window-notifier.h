/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "notify/notifier.h"

#include <QtCore/QPointer>

/**
 * @defgroup window_notify Window notify
 * @{
 */

class NotificationCallbackRepository;

class WindowNotifier : public Notifier
{
	Q_OBJECT

public:
	explicit WindowNotifier(QObject *parent = nullptr);
	virtual ~WindowNotifier();

	void setNotificationCallbackRepository(NotificationCallbackRepository *notificationCallbackRepository);

	virtual void notify(Notification *notification);

	virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }
	virtual NotifierConfigurationWidget *createConfigurationWidget(QWidget *parent = nullptr) { Q_UNUSED(parent) return nullptr; }

private:
	QPointer<NotificationCallbackRepository> m_notificationCallbackRepository;

	void createDefaultConfiguration();

private slots:
	void notificationClosed(Notification *notification);

};

/** @} */
