/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2008, 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#include "accounts/account.h"
#include "accounts/account-manager.h"
#include "configuration/xml-configuration-file.h"
#include "contacts/contact-details.h"
#include "contacts/contact-manager.h"
#include "contacts/contact-shared.h"
#include "buddies/buddy-manager.h"
#include "storage/storage-point.h"
#include "dnshandler.h"

#include "contact.h"

KaduSharedBaseClassImpl(Contact)

Contact Contact::null;

Contact Contact::create()
{
	return new ContactShared();
}

Contact Contact::loadFromStorage(StoragePoint *contactStoragePoint)
{
	return ContactShared::loadFromStorage(contactStoragePoint);
}

Contact::Contact()
{
}

Contact::Contact(ContactShared *data) :
		SharedBase<ContactShared>(data)
{
	data->ref.ref();
}

Contact::Contact(QObject *data)
{
	ContactShared *shared = dynamic_cast<ContactShared *>(data);
	if (shared)
		setData(shared);
}

Contact::Contact(const Contact &copy) :
		SharedBase<ContactShared>(copy)
{
}

Contact::~Contact()
{
}

/*
void Contact::refreshDNSName()
{
	if (!(Address.isNull()))
		connect(new DNSHandler(Id, Address), SIGNAL(result(const QString &, const QString &)),
				this, SLOT(setDNSName(const QString &, const QString &)));
}*/

bool Contact::validateId()
{
	return true;
}

bool Contact::isValid()
{
	return validateId();
}

KaduSharedBase_PropertyDef(Contact, ContactDetails *, details, Details, 0)
KaduSharedBase_PropertyReadDef(Contact, QUuid, uuid, Uuid, QUuid())
KaduSharedBase_PropertyReadDef(Contact, StoragePoint *, storage, Storage, 0)
KaduSharedBase_PropertyDef(Contact, Account, contactAccount, ContactAccount, Account::null)
KaduSharedBase_PropertyDef(Contact, Avatar, contactAvatar, ContactAvatar, Avatar::null)
KaduSharedBase_PropertyDef(Contact, Buddy, ownerBuddy, OwnerBuddy, Buddy::null)
KaduSharedBase_PropertyDef(Contact, QString, id, Id, QString::null)
KaduSharedBase_PropertyDef(Contact, int, priority, Priority, -1)
KaduSharedBase_PropertyDef(Contact, Status, currentStatus, CurrentStatus, Status::null)
KaduSharedBase_PropertyDef(Contact, QString, protocolVersion, ProtocolVersion, QString::null)
KaduSharedBase_PropertyDef(Contact, QHostAddress, address, Address, QHostAddress())
KaduSharedBase_PropertyDef(Contact, unsigned int, port, Port, 0)
KaduSharedBase_PropertyDef(Contact, QString, dnsName, DnsName, QString::null)
