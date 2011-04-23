/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CHAT_NOTIFIER_H
#define CHAT_NOTIFIER_H

#include "notify/notifier.h"

class ChatWidget;

class ChatNotifier : public Notifier
{
	Q_OBJECT

	void sendNotificationToChatWidget(Notification *notification, ChatWidget *chatWidget);

public:
	explicit ChatNotifier(QObject *parent = 0);
	virtual ~ChatNotifier();

	virtual NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent = 0);

	virtual void notify(Notification *notification);

};

#endif // CHAT_NOTIFIER_H
