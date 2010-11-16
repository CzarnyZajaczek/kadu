/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "avatars/avatar.h"
#include "avatars/avatar-manager.h"
#include "contacts/contact.h"
#include "protocols/protocol.h"
#include "protocols/services/avatar-service.h"

#include "avatar-job-runner.h"

AvatarJobRunner::AvatarJobRunner(QObject *parent) :
		QObject(parent)
{
}

AvatarJobRunner::~AvatarJobRunner()
{
}

AvatarService * AvatarJobRunner::avatarService(Account account)
{
	Protocol *protocol = account.protocolHandler();
	if (!protocol)
		return 0;

	return protocol->avatarService();
}

AvatarService * AvatarJobRunner::avatarService(Contact contact)
{
	Account account = contact.contactAccount();
	if (account.isNull())
		return 0;

	return avatarService(account);
}

void AvatarJobRunner::runJob(Contact contact)
{
	AvatarService *service = avatarService(contact);
	if (!service)
		return;

	connect(service, SIGNAL(avatarFetched(Contact,bool,QByteArray)),
			this, SLOT(avatarFetched(Contact,bool,QByteArray)));
	service->fetchAvatar(contact);
}

void AvatarJobRunner::avatarFetched(Contact contact, bool ok, const QByteArray &data)
{
	if (!ok)
	{
		emit jobFinished(false);
		deleteLater();
		return;
	}

	Avatar avatar = AvatarManager::instance()->byContact(contact, ActionCreateAndAdd);
	avatar.setLastUpdated(QDateTime::currentDateTime());

	QPixmap pixmap;
	if (!data.isEmpty())
		pixmap.loadFromData(data);

	avatar.setPixmap(pixmap);

	emit jobFinished(true);

	deleteLater();
}
