/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONNECTION_ERROR_NOTIFICATION_H
#define CONNECTION_ERROR_NOTIFICATION_H
// TODO account_notification
#include "protocol_notification.h"

class Notify;

class ConnectionErrorNotification : public AccountNotification
{
	Q_OBJECT

	static QStringList ActiveErrors;
	QString ErrorServer;
	QString ErrorMessage;

public:

	static void registerEvent(Notify * manager);
	static void unregisterEvent(Notify * manager);

	static bool activeError(const QString &errorMessage);

	ConnectionErrorNotification(const QString &errorServer, const QString &errorMessage, Account *account);
	virtual ~ConnectionErrorNotification();

	QString errorMessage() const;
	QString errorServer() const;

};

#endif // CONNECTION_ERROR_NOTIFICATION_H
