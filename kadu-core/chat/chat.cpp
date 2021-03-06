/*
 * %kadu copyright begin%
 * Copyright 2010, 2011, 2012 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2011, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "buddies/group.h"
#include "chat/chat-details.h"
#include "chat/chat-manager.h"
#include "contacts/contact-set.h"
#include "core/injected-factory.h"

#include "chat.h"

KaduSharedBaseClassImpl(Chat)

    /**
     * @short Null Chat object.
     *
     * Null Chat object (without @link ChatShared @endlink attached).
     */
    Chat Chat::null;

Chat::Chat()
{
}

/**
 * @short Returns new access object for given ChatShared instance.
 * @return new access object for given ChatShared instance
 *
 * Returns new access object for given ChatShared instance.
 */
Chat::Chat(ChatShared *data) : SharedBase<ChatShared>(data)
{
}

/**
 * @short Casts QObject * to Chat class.
 * @param data QObject * instance that could be casted to Chat object
 * @return Chat object from given data object or Chat::null if data cannot be cast to Chat.
 *
 * If data parameter contains object of type ChatShared this method will create access
 * object to this data and return it. Else, Chat::null will be returned.
 */
Chat::Chat(QObject *data)
{
    ChatShared *shared = qobject_cast<ChatShared *>(data);
    if (shared)
        setData(shared);
}

/**
 * @short Copy contructor for other Chat object.
 * @param copy Chat object to copy
 * @return Chat object that will be access object for the same data as copy object.
 *
 * Creates new Chat object that will be access object fot he same data as copy object.
 */
Chat::Chat(const Chat &copy) : SharedBase<ChatShared>(copy)
{
}

Chat::~Chat()
{
}

bool Chat::isInGroup(Group group) const
{
    return isNull() ? false : data()->isInGroup(group);
}

bool Chat::showInAllGroup() const
{
    return isNull() ? false : data()->showInAllGroup();
}

void Chat::addToGroup(Group group) const
{
    if (!isNull() && !data()->isInGroup(group))
        data()->addToGroup(group);
}
void Chat::removeFromGroup(Group group) const
{
    if (!isNull() && data()->isInGroup(group))
        data()->removeFromGroup(group);
}

KaduSharedBase_PropertyReadDef(Chat, ContactSet, contacts, Contacts, ContactSet())
    KaduSharedBase_PropertyReadDef(Chat, QString, name, Name, QString())
        KaduSharedBase_PropertyReadDef(Chat, ChatDetails *, details, Details, 0)
            KaduSharedBase_PropertyDefCRW(Chat, Account, chatAccount, ChatAccount, Account::null)
                KaduSharedBase_PropertyDefCRW(Chat, QString, type, Type, QString())
                    KaduSharedBase_PropertyDefCRW(Chat, QString, display, Display, QString())
                        KaduSharedBase_PropertyBoolDef(Chat, IgnoreAllMessages, false)
                            KaduSharedBase_PropertyDefCRW(Chat, QSet<Group>, groups, Groups, QSet<Group>())
                                KaduSharedBase_PropertyDef(Chat, quint16, unreadMessagesCount, UnreadMessagesCount, 0)

                                    bool Chat::isConnected() const
{
    if (!isNull())
        return data()->isConnected();
    else
        return false;
}

bool Chat::isOpen() const
{
    if (!isNull())
        return data()->isOpen();
    else
        return false;
}

void Chat::setOpen(bool open)
{
    if (!isNull())
        data()->setOpen(open);
}

QString title(const Chat &chat)
{
    if (!chat.display().isEmpty())
        return chat.display();
    else
        return chat.name();
}
