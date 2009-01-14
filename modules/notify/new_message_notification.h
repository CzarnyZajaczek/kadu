/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef NEW_MESSAGE_NOTIFICATION_H
#define NEW_MESSAGE_NOTIFICATION_H

#include "protocol_notification.h"
#include "notify.h"

class MessageNotification : public AccountNotification
{
	Q_OBJECT

public:

	enum MessageType {
		NewChat,
		NewMessage
	};

	static void registerEvents(Notify * manager);
	static void unregisterEvents(Notify * manager);

	MessageNotification(MessageType messageType, const ContactList &contacts, const QString &message, Account *account);
	virtual ~MessageNotification() {};

};

#endif // NEW_MESSAGE_NOTIFICATION_H
