/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QPointer>
#include <QtScript/QScriptValue>

#include "sms-gateway.h"

#include "sms-sender.h"

class SmsGatewayManager;
class SmsScriptsManager;
class SmsTokenReadJob;

class SmsInternalSender : public SmsSender
{
	Q_OBJECT

	QPointer<SmsGatewayManager> m_smsGatewayManager;
	QPointer<SmsScriptsManager> m_smsScriptsManager;

	SmsGateway Gateway;
	SmsTokenReadJob *TokenJob;

	void queryForGateway();
	void gatewaySelected();

	void sendSms();

private slots:
	void jobFinished(bool ok, const QString &entryIcon, const QString &entryMessage);

public:
	explicit SmsInternalSender(SmsGatewayManager *smsGatewayManager, SmsScriptsManager *smsScriptsManager, const QString &number, const SmsGateway &gateway, QObject *parent = 0);
	virtual ~SmsInternalSender();

	virtual void sendMessage(const QString &message);

	void findGatewayForNumber(const QString &number);

public slots:
	void gatewayQueryDone(const QString &gatewayId);
	void readToken(const QString &tokenImageUrl, QScriptValue callbackObject, QScriptValue callbackMethod);

	QScriptValue readFromConfiguration(const QString &group, const QString &name, const QString &defaultValue);

	virtual void cancel();

	void result();
	void failure(const QString &errorMessage);

};
