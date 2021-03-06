/*
 * %kadu copyright begin%
 * Copyright 2013, 2014, 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "exports.h"

#include <QtCore/QObject>

class ChatWidget;
enum class OpenChatActivation;

/**
 * @addtogroup Gui
 * @{
 */

/**
 * @class ChatWidgetContainerHandler
 * @short Class for handling containers of chat widgets.
 *
 * Chat widgets are put into different containers (like separate windows,
 * tabs, single window). This class is used to handle chat widgets interactions
 * with one container type, like:
 * <ul>
 *   <li>checking if container accepts given chat widget</li>
 *   <li>adding chat widget to a container</li>
 *   <li>removing chat widget from a container</li>
 *   <li>checking if chat widget in a container is active</li>
 *   <li>activating chat widget in a container</li>
 * </ul>
 *
 * Signal chatAcceptanceChanged(Chat) is emitted when acceptance state
 * for given chat widget changes.
 */
class KADUAPI ChatWidgetContainerHandler : public QObject
{
    Q_OBJECT

public:
    explicit ChatWidgetContainerHandler(QObject *parent = nullptr);
    virtual ~ChatWidgetContainerHandler();

    /**
     * @short Check if container accepts given chat.
     */
    virtual bool acceptChat(Chat chat) const = 0;

    /**
     * @short Add chat to container.
     *
     * This method will fail silently if acceptChat(chatWidget) returns false.
     * This method will not fail if acceptChat(chatWidget) returns true.
     */
    virtual ChatWidget *addChat(Chat chat, OpenChatActivation activation) = 0;

    /**
     * @short Remove chat from container.
     *
     * If container does not contain given chat widget this method does nothing.
     */
    virtual void removeChat(Chat chat) = 0;

    /**
     * @short Return true if given chat widget is active and its container is active.
     */
    virtual bool isChatWidgetActive(ChatWidget *chatWidget) = 0;

    /**
     * @short Try to activate given chat widget and its container.
     */
    virtual void tryActivateChatWidget(ChatWidget *chatWidget) = 0;

    /**
     * @short Try to minimize given chat widget and its container.
     */
    virtual void tryMinimizeChatWidget(ChatWidget *chatWidget) = 0;

signals:
    /**
     * @short Signal emitted when given chat widget was activated.
     */
    void chatWidgetActivated(ChatWidget *chatWidget);

    /**
     * @short Signal emitted when acceptance state for given chat widget changes.
     */
    void chatAcceptanceChanged(Chat chat);
};

/**
 * @}
 */
