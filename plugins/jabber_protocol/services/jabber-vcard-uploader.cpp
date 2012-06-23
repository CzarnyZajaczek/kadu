/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <xmpp_tasks.h>

#include "jabber-vcard-uploader.h"

JabberVCardUploader::JabberVCardUploader(XMPP::Client *client, QObject *parent) :
		QObject(parent), XmppClient(client)
{
}

JabberVCardUploader::~JabberVCardUploader()
{
}

void JabberVCardUploader::done()
{
	emit vCardUploaded(true);
	deleteLater();
}

void JabberVCardUploader::failed()
{
	emit vCardUploaded(false);
	deleteLater();
}

void JabberVCardUploader::taskFinished()
{
	if (!Task || !Task.data()->success())
		failed();
	else
		done();
}

void JabberVCardUploader::uploadVCard(const QString &id, XMPP::VCard vCard)
{
	if (!XmppClient || !XmppClient.data()->rootTask())
	{
		failed();
		return;
	}

	Task = new XMPP::JT_VCard(XmppClient.data()->rootTask());

	connect(Task.data(), SIGNAL(finished()), this, SLOT(taskFinished()));
	Task.data()->set(id, vCard);
	Task.data()->go(true);
}
