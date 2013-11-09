/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "single-window-chat-widget-container-handler.h"

#include "single-window.h"

SingleWindowChatWidgetContainerHandler::SingleWindowChatWidgetContainerHandler(QObject *parent) :
		ChatWidgetContainerHandler(parent)
{
}

SingleWindowChatWidgetContainerHandler::~SingleWindowChatWidgetContainerHandler()
{
}

void SingleWindowChatWidgetContainerHandler::setSingleWindow(SingleWindow *singleWindow)
{
	m_singleWindow = singleWindow;
}

bool SingleWindowChatWidgetContainerHandler::containChatWidget(ChatWidget *chatWidget)
{
	if (!m_singleWindow)
		return false;

	return m_singleWindow.data()->containChatWidget(chatWidget);
}

bool SingleWindowChatWidgetContainerHandler::isChatWidgetActive(ChatWidget *chatWidget)
{
	return m_singleWindow ? m_singleWindow.data()->isChatWidgetActive(chatWidget) : false;
}