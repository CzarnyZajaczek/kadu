/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "accounts/account-manager.h"
#include "accounts/account.h"
#include "buddies/buddy-manager.h"
#include "configuration/configuration-api.h"
#include "configuration/configuration.h"
#include "contacts/contact-manager.h"
#include "core/injected-factory.h"
#include "storage/storage-point.h"

#include "contact.h"

KaduSharedBaseClassImpl(Contact)

    Contact Contact::null;

Contact::Contact()
{
}

Contact::Contact(ContactShared *data) : SharedBase<ContactShared>(data)
{
}

Contact::Contact(QObject *data)
{
    ContactShared *shared = qobject_cast<ContactShared *>(data);
    if (shared)
        setData(shared);
}

Contact::Contact(const Contact &copy) : SharedBase<ContactShared>(copy)
{
}

Contact::~Contact()
{
}

QString Contact::display(bool useBuddyData) const
{
    if (data())
        return data()->display(useBuddyData);

    return QString();
}

Contact Contact::contactWithHigherStatus(const Contact &c1, const Contact &c2)
{
    if (!c1)
        return c2;
    if (!c2)
        return c1;
    return c2.currentStatus() < c1.currentStatus() ? c2 : c1;
}

KaduSharedBase_PropertyReadDef(Contact, QUuid, uuid, Uuid, QUuid()) KaduSharedBase_PropertyReadDef(
    Contact, std::shared_ptr<StoragePoint>, storage, Storage, std::shared_ptr<StoragePoint>())
    KaduSharedBase_PropertyDefCRW(Contact, Account, contactAccount, ContactAccount, Account::null)
        KaduSharedBase_PropertyDefCRW(Contact, Buddy, ownerBuddy, OwnerBuddy, Buddy::null)
            KaduSharedBase_PropertyDefCRW(Contact, QString, id, Id, QString())
                KaduSharedBase_PropertyDef(Contact, int, priority, Priority, -1)
                    KaduSharedBase_PropertyDefCRW(Contact, Status, currentStatus, CurrentStatus, Status())
                        KaduSharedBase_PropertyBoolDef(Contact, Blocking, false) KaduSharedBase_PropertyDef(
                            Contact, bool, ignoreNextStatusChange, IgnoreNextStatusChange, false)

                            RosterEntry *Contact::rosterEntry() const
{
    if (isNull())
        return 0;
    else
        return data()->rosterEntry();
}

KaduSharedBase_PropertyDef(Contact, short int, maximumImageSize, MaximumImageSize, 0)
    KaduSharedBase_PropertyDef(Contact, quint16, unreadMessagesCount, UnreadMessagesCount, 0)
        KaduSharedBase_PropertyBoolReadDef(Contact, Anonymous, true)
