/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "message-render-info-factory.h"

#include "chat-style/chat-style-manager.h"
#include "core/core.h"
#include "gui/configuration/chat-configuration-holder.h"
#include "message/message-render-header-behavior.h"
#include "message/message-render-info.h"
#include "message/message.h"

MessageRenderInfoFactory::MessageRenderInfoFactory(QObject *parent) : QObject{parent}
{
}

MessageRenderInfoFactory::~MessageRenderInfoFactory()
{
}

void MessageRenderInfoFactory::setChatConfigurationHolder(ChatConfigurationHolder *chatConfigurationHolder)
{
    m_chatConfigurationHolder = chatConfigurationHolder;
}

void MessageRenderInfoFactory::setChatStyleManager(ChatStyleManager *chatStyleManager)
{
    m_chatStyleManager = chatStyleManager;
}

MessageRenderInfo MessageRenderInfoFactory::messageRenderInfo(
    const Message &previous, const Message &message, MessageRenderHeaderBehavior renderHeaderBehavior)
{
    auto header = includeHeader(previous, message, renderHeaderBehavior);
    return MessageRenderInfo{
        message, backgroundColor(message), fontColor(message), nickColor(message), header,
        header ? m_chatStyleManager->cfgHeaderSeparatorHeight() : m_chatStyleManager->paragraphSeparator(),
        showServerTime(message)
    };
}

QString MessageRenderInfoFactory::backgroundColor(const Message &message) const
{
    return message.type() == MessageTypeSent ? m_chatConfigurationHolder->myBackgroundColor()
                                             : m_chatConfigurationHolder->usrBackgroundColor();
}

QString MessageRenderInfoFactory::nickColor(const Message &message) const
{
    return message.type() == MessageTypeSent ? m_chatConfigurationHolder->myNickColor()
                                             : m_chatConfigurationHolder->usrNickColor();
}

QString MessageRenderInfoFactory::fontColor(const Message &message) const
{
    return message.type() == MessageTypeSent ? m_chatConfigurationHolder->myFontColor()
                                             : m_chatConfigurationHolder->usrFontColor();
}

bool MessageRenderInfoFactory::includeHeader(
    const Message &previous, const Message &message, MessageRenderHeaderBehavior renderHeaderBehavior) const
{
    if (renderHeaderBehavior == MessageRenderHeaderBehavior::Always)
        return true;
    if (!previous || previous.type() == MessageTypeSystem || message.type() == MessageTypeSystem)
        return true;
    if (message.messageSender() != previous.messageSender())
        return true;

    if (message.receiveDate().toTime_t() < previous.receiveDate().toTime_t())
        qWarning("New message has earlier date than last message");

    auto minimumInterval = m_chatStyleManager->cfgNoHeaderInterval() * 60;
    auto actualInterval = static_cast<int>(message.receiveDate().toTime_t() - previous.receiveDate().toTime_t());
    return actualInterval > minimumInterval;
}

int MessageRenderInfoFactory::separatorSize(bool includeHeader) const
{
    return includeHeader ? m_chatStyleManager->cfgHeaderSeparatorHeight() : m_chatStyleManager->paragraphSeparator();
}

bool MessageRenderInfoFactory::showServerTime(const Message &message) const
{
    if (message.type() == MessageTypeSystem || !message.sendDate().isValid())
        return false;
    if (!m_chatStyleManager->noServerTime())
        return true;

    auto minimumInterval = m_chatStyleManager->noServerTimeDiff();
    auto actuvalInterval =
        static_cast<int>(message.receiveDate().toTime_t()) - static_cast<int>(message.sendDate().toTime_t());
    return abs(actuvalInterval) > minimumInterval;
}
