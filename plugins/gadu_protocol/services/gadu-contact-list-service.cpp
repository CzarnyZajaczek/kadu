/*
 * %kadu copyright begin%
 * Copyright 2010 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include <libgadu.h>

#include <QtCore/QByteArray>

#include "buddies/buddy-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-shared.h"
#include "misc/misc.h"
#include "debug.h"

#include "helpers/gadu-list-helper.h"
#include "gadu-account-details.h"
#include "gadu-protocol.h"
#include "services/gadu-contact-list-state-machine.h"
#include "socket-notifiers/gadu-protocol-socket-notifiers.h"

#include "gadu-contact-list-service.h"

GaduContactListService::GaduContactListService(GaduProtocol *protocol) :
		ContactListService(protocol), Protocol(protocol), StateMachine(new GaduContactListStateMachine(this))
{
	connect(StateMachine, SIGNAL(awaitingServerGetResponseStateEntered()), SLOT(importContactList()));
	connect(StateMachine, SIGNAL(awaitingServerPutResponseStateEntered()), SLOT(exportContactList()));

	StateMachine->start();
}

GaduContactListService::~GaduContactListService()
{
}

void GaduContactListService::handleEventUserlist100GetReply(struct gg_event *e)
{
	if (!StateMachine->awaitingServerGetResponse())
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "got unexpected userlist 100 get reply, ignoring\n");
		return;
	}

	GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(Protocol->account().details());
	if (!accountDetails)
	{
		Q_ASSERT(0);
		emit stateMachineInternalError();
		emit contactListImported(false, BuddyList());
		return;
	}

	if (e->event.userlist100_reply.format_type != GG_USERLIST100_FORMAT_TYPE_GG70)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "got userlist 100 reply with unwanted format type (%d)\n", (int)e->event.userlist100_reply.format_type);
		emit stateMachineInternalError();
		emit contactListImported(false, BuddyList());
		return;
	}

	const char *content = e->event.userlist100_reply.reply;
	if (!content)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "got userlist 100 reply without any content\n");
		emit stateMachineInternalError();
		emit contactListImported(false, BuddyList());
		return;
	}

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "userlist 100 reply:\n%s\n", content);

	QByteArray content2(content);
	BuddyList buddies = GaduListHelper::byteArrayToBuddyList(Protocol->account(), content2);
	emit stateMachineSucceededImporting();
	emit contactListImported(true, buddies);
	accountDetails->setUserlistVersion(e->event.userlist100_reply.version);

	// TODO: dirty contacts

	// cleanup references, so buddy and contact instances can be removed
	// this is really a hack, we need to call aboutToBeRemoved someway for non-manager contacts and buddies too
	// or just only store managed only, i dont know yet
	foreach (Buddy buddy, buddies)
	{
		foreach (Contact contact, buddy.contacts())
			contact.data()->aboutToBeRemoved();
		buddy.data()->aboutToBeRemoved();
	}
}

void GaduContactListService::handleEventUserlist100PutReply(struct gg_event *e)
{
	if (!StateMachine->awaitingServerPutResponse())
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "got unexpected userlist 100 put reply, ignoring\n");
		return;
	}

	if (e->event.userlist100_reply.type == GG_USERLIST100_REPLY_ACK)
	{
		GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(Protocol->account().details());
		if (accountDetails)
		{
			accountDetails->setUserlistVersion(e->event.userlist100_reply.version);

			foreach (const Contact &contact, ContactManager::instance()->contacts(Protocol->account()))
				contact.setDirty(false);

			emit stateMachineSucceededExporting();
			emit contactListExported(true);

			return;
		}
	}

	emit stateMachineFailedExporting();
	emit contactListExported(false);
}

void GaduContactListService::handleEventUserlist100Reply(struct gg_event *e)
{
	switch (e->event.userlist100_reply.type)
	{
		case GG_USERLIST100_REPLY_LIST:
			handleEventUserlist100GetReply(e);
			break;
		case GG_USERLIST100_REPLY_ACK:
		case GG_USERLIST100_REPLY_REJECT:
			handleEventUserlist100PutReply(e);
			break;
		default:
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "got unknown userlist100 reply type (%d)\n", e->event.userlist100_reply.type);
	}
}

void GaduContactListService::handleEventUserlist100Version(gg_event *e)
{
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "new version of userlist available: %d\n", e->event.userlist100_version.version);

	GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(Protocol->account().details());
	Q_ASSERT(accountDetails);
	if (accountDetails && accountDetails->userlistVersion() != (int)e->event.userlist100_version.version)
		emit stateMachineNewVersionAvailable();
}

void GaduContactListService::importContactList()
{
	ContactListService::importContactList();

	if (-1 == gg_userlist100_request(Protocol->gaduSession(), GG_USERLIST100_GET, 0, GG_USERLIST100_FORMAT_TYPE_GG70, 0))
	{
		emit stateMachineInternalError();
		emit contactListImported(false, BuddyList());
	}
}

void GaduContactListService::exportContactList()
{
	exportContactList(BuddyManager::instance()->buddies(Protocol->account()));
}

void GaduContactListService::exportContactList(const BuddyList &buddies)
{
	QByteArray contacts = GaduListHelper::buddyListToByteArray(Protocol->account(), buddies);

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "\n%s\n", contacts.constData());

	GaduAccountDetails *accountDetails = dynamic_cast<GaduAccountDetails *>(Protocol->account().details());
	Q_ASSERT(accountDetails);
	if (!accountDetails || -1 == gg_userlist100_request(
			Protocol->gaduSession(), GG_USERLIST100_PUT, accountDetails->userlistVersion(),
			GG_USERLIST100_FORMAT_TYPE_GG70, contacts.constData()))
	{
		emit stateMachineInternalError();
		emit contactListExported(false);
	}
}

void GaduContactListService::copySupportedBuddyInformation(Buddy &destination, const Buddy &source)
{
	GaduListHelper::setSupportedBuddyInformation(destination, source);
}

QList<Buddy> GaduContactListService::loadBuddyList(QTextStream &dataStream)
{
	return GaduListHelper::streamToBuddyList(Protocol->account(), dataStream);
}

QByteArray GaduContactListService::storeBuddyList(const BuddyList &buddies)
{
	return GaduListHelper::buddyListToByteArray(Protocol->account(), buddies);
}
