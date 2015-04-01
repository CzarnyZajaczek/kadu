/*
 * %kadu copyright begin%
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "notify/notification-manager.h"
#include "notify/notify-event.h"

#include "antistring-notification.h"

void AntistringNotification::registerNotifications()
{
	NotificationManager::instance()->registerNotifyEvent(NotifyEvent("Antistring", QT_TRANSLATE_NOOP("@default", "Antistring notifications"), NotifyEvent::CallbackNotRequired));
}

void AntistringNotification::unregisterNotifications()
{
	NotificationManager::instance()->unregisterNotifyEvent("Antistring");
}

void AntistringNotification::notifyStringReceived(const Chat &chat)
{
	AntistringNotification *notification = new AntistringNotification(chat);
	notification->setTitle(tr("Antistring"));
	notification->setText(tr("Your interlocutor send you love letter"));
	NotificationManager::instance()->notify(notification);
}

AntistringNotification::AntistringNotification(const Chat &chat) :
		Notification(Account::null, chat, "Antistring", KaduIcon())
{
	addChatCallbacks();
}

AntistringNotification::~AntistringNotification()
{
}

#include "moc_antistring-notification.cpp"
