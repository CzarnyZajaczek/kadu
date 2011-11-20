/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "chat/chat.h"
#include "gui/actions/action-context.h"
#include "gui/widgets/chat-widget.h"
#include "gui/widgets/chat-widget-manager.h"
#include "gui/windows/message-dialog.h"

#include "leave-chat-action.h"

LeaveChatAction::LeaveChatAction(QObject *parent) :
		ActionDescription(parent)
{
	setType(ActionDescription::TypeChat);
	setName("leaveChatAction");
	setIcon(KaduIcon("application-exit"));
	setText(tr("Leave"));

	registerAction();
}

LeaveChatAction::~LeaveChatAction()
{
}

void LeaveChatAction::triggered(QWidget *widget, ActionContext *context, bool toggled)
{
	Q_UNUSED(toggled)

	const Chat &chat = context->chat();
	if (!chat)
		return;

	ChatWidget *chatWidget = ChatWidgetManager::instance()->byChat(chat, false);
	if (!chatWidget)
		return;

	if (!MessageDialog::ask(KaduIcon("dialog-warning"), tr("Kadu"), tr("All messages received in this conference will be ignored\nfrom now on. Are you sure you want to leave this conference?"), widget))
		return;

	chat.setIgnoreAllMessages(true);
	chatWidget->close();
}