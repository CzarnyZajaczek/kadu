/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2004 Adrian Smarzewski (adrian@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011, 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2004, 2006 Marcin Ślusarz (joi@kadu.net)
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

#include "configuration/configuration-file.h"
#include "core/core.h"
#include "notify/notification-manager.h"
#include "kadu-application.h"

#include "chat-notifier.h"

#include "chat-notify-plugin.h"

ChatNotifyPlugin::~ChatNotifyPlugin()
{
}

bool ChatNotifyPlugin::init(bool firstLoad)
{
	if (firstLoad)
		createDefaultConfiguration();

	NotifierInstance = new ChatNotifier(this);
	NotifierInstance->setChatWidgetRepository(Core::instance()->chatWidgetRepository());
	NotifierInstance->setFormattedStringFactory(Core::instance()->formattedStringFactory());

	NotificationManager::instance()->registerNotifier(NotifierInstance);

	return true;
}

void ChatNotifyPlugin::done()
{
	NotificationManager::instance()->unregisterNotifier(NotifierInstance);
}

void ChatNotifyPlugin::createDefaultConfiguration()
{
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Notify", "FileTransfer_ChatNotifier", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Notify", "FileTransfer/IncomingFile_ChatNotifier", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Notify", "StatusChanged_ChatNotifier", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Notify", "StatusChanged/ToAway_ChatNotifier", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Notify", "StatusChanged/ToDoNotDisturb_ChatNotifier", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Notify", "StatusChanged/ToFreeForChat_ChatNotifier", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Notify", "StatusChanged/ToNotAvailable_ChatNotifier", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Notify", "StatusChanged/ToOffline_ChatNotifier", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Notify", "StatusChanged/ToOnline_ChatNotifier", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("Notify", "OTR_ChatNotifier", true);
}

Q_EXPORT_PLUGIN2(chat_notify, ChatNotifyPlugin)

#include "moc_chat-notify-plugin.cpp"
