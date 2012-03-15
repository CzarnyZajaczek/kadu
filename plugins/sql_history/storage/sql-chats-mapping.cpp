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

#include <QtSql/QSqlQuery>

#include "accounts/account.h"
#include "chat/chat.h"
#include "chat/chat-manager.h"
#include "chat/type/chat-type-contact.h"
#include "chat/type/chat-type-contact-set.h"
#include "contacts/contact-set.h"

#include "storage/sql-accounts-mapping.h"
#include "storage/sql-contacts-mapping.h"

#include "sql-chats-mapping.h"

SqlChatsMapping::SqlChatsMapping(const QSqlDatabase &database, SqlAccountsMapping *accountsMapping, SqlContactsMapping *contactsMapping, QObject *parent) :
		QObject(parent), Database(database), AccountsMapping(accountsMapping), ContactsMapping(contactsMapping)
{
	Q_ASSERT(AccountsMapping);
	Q_ASSERT(ContactsMapping);

	loadMappingsFromDatabase();
}

SqlChatsMapping::~SqlChatsMapping()
{
}

void SqlChatsMapping::addMapping(int id, const Chat &chat)
{
	chat.addProperty("sql_history:id", id, CustomProperties::NonStorable);
	ChatMapping.insert(id, chat);
}

void SqlChatsMapping::loadMappingsFromDatabase()
{
	QSqlQuery query(Database);
	query.prepare("SELECT id, account_id, chat FROM kadu_chats");

	query.setForwardOnly(true);
	query.exec();

	while (query.next())
	{
		int id = query.value(0).toInt();
		Account account = AccountsMapping->accountById(query.value(1).toInt());
		QString chatString = query.value(2).toString();

		if (id <= 0 || !account || chatString.isEmpty())
			continue;

		Chat chat = stringToChat(account, chatString);
		if (chat)
			addMapping(id, chat);
	}
}

Chat SqlChatsMapping::chatById(int sqlId) const
{
	if (ChatMapping.contains(sqlId))
		return ChatMapping.value(sqlId);
	else
		return Chat::null;
}

int SqlChatsMapping::idByChat(const Chat &chat, bool create)
{
	int id = chat.property("sql_history:id", 0).toInt();
	if (!create || id > 0)
		return id;

	QString chatString = chatToString(chat);
	if (chatString.isEmpty())
		return 0;

	QSqlQuery query(Database);
	query.prepare("INSERT INTO kadu_chats (account_id, chat) VALUES (:account_id, :chat)");
	query.bindValue(":account_id", SqlAccountsMapping::idByAccount(chat.chatAccount()));
	query.bindValue(":chat", chatToString(chat));
	query.exec();

	id = query.lastInsertId().toInt();
	addMapping(id, chat);

	return id;
}

const QMap<int, Chat> & SqlChatsMapping::mapping() const
{
	return ChatMapping;
}

QString SqlChatsMapping::chatToString(const Chat &chat)
{
	if ("Contact" == chat.type())
		return QString("Contact;") + QString::number(ContactsMapping->idByContact(chat.contacts().toContact(), true));

	if ("ContactSet" == chat.type())
	{
		QStringList result;
		result << "ContactSet";

		foreach (const Contact &contact, chat.contacts())
			result << QString::number(ContactsMapping->idByContact(contact, true));

		return result.join(";");
	}

	return QString();
}

Chat SqlChatsMapping::stringToChat(const Account &account, const QString &string)
{
	Q_UNUSED(account);

	QStringList items = string.split(";", QString::SkipEmptyParts);
	int len = items.length();

	if (len < 2)
		return Chat::null;

	QString chatType = items.at(0);
	if ("Contact" == chatType)
	{
		Contact contact = ContactsMapping->contactById(items.at(1).toInt());
		if (!contact)
			return Chat::null;

		return ChatTypeContact::findChat(contact, ActionCreate);
	}

	if ("ContactSet" == chatType)
	{
		ContactSet contacts;
		for (int i = 1; i < len; i++)
		{
			Contact contact = ContactsMapping->contactById(items.at(i).toInt());
			if (!contact)
				return Chat::null;

			contacts.insert(contact);
		}

		return ChatTypeContactSet::findChat(contacts, ActionCreate);
	}

	return Chat::null;
}