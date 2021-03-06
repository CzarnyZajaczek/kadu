/*
 * %kadu copyright begin%
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "storage/storable-object.h"

#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyManager;
class Buddy;
class Chat;
class ConfigurationManager;
class Configuration;
class Contact;
class MessageStorage;
class Message;
class SortedMessages;

/**
 * @addtogroup Message
 * @{
 */

/**
 * @class UnreadMessageRepository
 * @short Repository of unread messages used in Kadu.
 *
 * This class stores all pending messages in permanent storage.
 * Pending message is an incoming message that have not ever been displayed to user.
 */
class KADUAPI UnreadMessageRepository : public StorableObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit UnreadMessageRepository(QObject *parent = nullptr);
    virtual ~UnreadMessageRepository();

    virtual StorableObject *storageParent()
    {
        return {};
    }
    virtual QString storageNodeName()
    {
        return QStringLiteral("Messages");
    }

    /**
     * @short Returns list of all unread messages.
     * @return list of all unread messages
     *
     * Returns list of all unread messages.
     */
    const QList<Message> &allUnreadMessages() const;

    /**
     * @short Returns list of all unread messages for given chat.
     * @param chat chat to get unread messages for
     * @return list of all unread messages for given chat
     *
     * Returns list of all unread messages for given chat.
     */
    SortedMessages unreadMessagesForChat(const Chat &chat) const;

    /**
     * @short Returns true if there is any unread message in manager.
     * @return true if there is any unread message in manager
     *
     * Returns true if there is any unread message in manager.
     */
    bool hasUnreadMessages() const;

    /**
     * @short Returns count of unread messages in manager.
     * @return count of unread messages in manager
     *
     * Returns count of unread messages in manager.
     */
    int unreadMessagesCount() const;

    /**
     * @short Marks messages from list as read.
     * @param messages list of messages to be marked as read
     *
     * Marks given messages as read. Messages are removed from unread list, its statuses
     * are updated to MessageStatusRead and its pending statues are set to false.
     * Signal unreadMessageRemoved is emited for each changed message.
     */
    void markMessagesAsRead(const SortedMessages &messages);

    /**
     * @short Returns one of unread messages.
     * @return one of unread messages
     *
     * Returns one of unread messages. If no unread messages are available, returns null message.
     */
    Message unreadMessage() const;

    /**
     * @short Returns one of unread messages sent by given buddy.
     * @param buddy sender of message
     * @return one of unread messages sent by given buddy
     *
     * Returns one of unread messages sent by given buddy. If no applicable messages are available, returns null
     * message.
     */
    Message unreadMessageForBuddy(const Buddy &buddy) const;

    /**
     * @short Returns one of unread messages sent by given contact.
     * @param contact sender of message
     * @return one of unread messages sent by given contact
     *
     * Returns one of unread messages sent by given contact. If no applicable messages are available, returns null
     * message.
     */
    Message unreadMessageForContact(const Contact &contact) const;

    /**
     * @short Adds new unread message to the list.
     * @param message new unread message
     *
     * Adds new unread message to the list. Message's pending status is set to true if no applicable
     * chat widget is found. Signal unreadMessageAdded is emited.
     */
    void addUnreadMessage(const Message &message);

    /**
     * @short Remove unread message from the list.
     * @param message unread message to remove
     *
     * Removes unread message from the list. Message's pending status is set to false.
     * Signal unreadMessageRemoved is emited.
     */
    void removeUnreadMessage(const Message &message);

signals:
    /**
     * @short Signal emited every time an unread message is added to manager.
     * @param message added unread message
     *
     * This signal is emited every time an unread message is added to manager.
     */
    void unreadMessageAdded(const Message &message);

    /**
     * @short Signal emited every time an unread message is removed from manager.
     * @param message removed unread message
     *
     * This signal is emited every time an unread message is removed from manager.
     */
    void unreadMessageRemoved(const Message &message);

protected:
    virtual void load();
    virtual void store();

private:
    QPointer<BuddyManager> m_buddyManager;
    QPointer<ConfigurationManager> m_configurationManager;
    QPointer<Configuration> m_configuration;
    QPointer<MessageStorage> m_messageStorage;

    QList<Message> m_unreadMessages;

    /**
     * @short Imports list of pending messages from < 0.10.0 configurations.
     * @return true if there was something to import
     *
     * This methods import list of pending messages from < 0.10.0 configurations. If there was no such
     * list false is returned.
     */
    bool importFromPendingMessages();

private slots:
    INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
    INJEQT_SET void setConfigurationManager(ConfigurationManager *configurationManager);
    INJEQT_SET void setConfiguration(Configuration *configuration);
    INJEQT_SET void setMessageStorage(MessageStorage *messageStorage);
    INJEQT_INIT void init();
    INJEQT_DONE void done();
};

/**
 * @}
 */
