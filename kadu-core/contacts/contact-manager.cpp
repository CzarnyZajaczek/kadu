/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "configuration/configuration-manager.h"
#include "contacts/contact.h"
#include "contacts/contact-shared.h"
#include "core/core.h"
#include "protocols/protocol.h"
#include "protocols/protocol-factory.h"
#include "debug.h"

#include "contact-manager.h"

ContactManager * ContactManager::Instance = 0;

ContactManager * ContactManager::instance()
{
	if (0 == Instance)
		Instance = new ContactManager();

	return Instance;
}

ContactManager::ContactManager()
{
}

ContactManager::~ContactManager()
{
}

void ContactManager::idChanged(const QString &oldId)
{
	Contact contact(sender());
	if (!contact.isNull())
		emit contactIdChanged(contact, oldId);
}

void ContactManager::itemAboutToBeRegistered(Contact item)
{
	emit contactAboutToBeAdded(item);
}

void ContactManager::itemRegisterd(Contact item)
{
	emit contactAdded(item);
	connect(item.data(), SIGNAL(idChanged(const QString &)), this, SLOT(idChanged(const QString &)));
}

void ContactManager::itemAboutToBeUnregisterd(Contact item)
{
	emit contactAboutToBeRemoved(item);
	disconnect(item.data(), SIGNAL(idChanged(const QString &)), this, SLOT(idChanged(const QString &)));
}

void ContactManager::itemUnregistered(Contact item)
{
	emit contactRemoved(item);
}

void ContactManager::detailsLoaded(Contact item)
{
	if (!item.isNull())
		registerItem(item);
}

void ContactManager::detailsUnloaded(Contact item)
{
	if (!item.isNull())
		unregisterItem(item);
}

Contact ContactManager::byId(Account account, const QString &id, bool create)
{
	ensureLoaded();

	if (id.isEmpty() || account.isNull())
		return Contact::null;

	foreach (const Contact &contact, allItems())
		if (account == contact.contactAccount() && id == contact.id())
			return contact;

	if (!create)
		return Contact::null;

	Contact contact = contact.create();
	contact.setContactAccount(account);
	contact.setId(id);

	addItem(contact);

	Protocol *protocolHandler = account.protocolHandler();
	if (!protocolHandler)
		return contact;

	ProtocolFactory *factory = protocolHandler->protocolFactory();
	if (!factory)
		return contact;

	ContactDetails *details = factory->createContactDetails(contact);
	details->setState(StateNotLoaded);
	contact.setDetails(details);

	return contact;
}

QList<Contact> ContactManager::contacts(Account account)
{
	ensureLoaded();

	QList<Contact> contacts;

	if (account.isNull())
		return contacts;

	foreach (const Contact &contact, allItems())
		if (account == contact.contactAccount())
			contacts.append(contact);

	return contacts;
}
