/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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
#include "gui/actions/action.h"
#include "gui/actions/action-description.h"

#include "encryption-provider-manager.h"

#include "encryption-actions.h"

void checkCanEncrypt(Action *action)
{
	Chat chat = action->chat();
	if (!chat)
	{
		action->setEnabled(false);
		return;
	}

	action->setEnabled(EncryptionProviderManager::instance()->canEncrypt(chat));
}

EncryptionActions * EncryptionActions::Instance = 0;

void EncryptionActions::registerActions()
{
	if (!Instance)
		Instance = new EncryptionActions();
}

void EncryptionActions::unregisterActions()
{
	delete Instance;
	Instance = 0;
}

EncryptionActions::EncryptionActions()
{
	EnableEncryptionActionDescription = new ActionDescription(this,
			ActionDescription::TypeChat, "encryptionAction",
			this, SLOT(enableEncryptionActionActivated(QAction *, bool)),
			"security-high", "security-low", tr("Enable encryption for this conversation"),
			true, tr("Disable encryption for this conversation"),
			checkCanEncrypt
	);
}

EncryptionActions::~EncryptionActions()
{
}

void EncryptionActions::enableEncryptionActionActivated(QAction *sender, bool toggled)
{
	Q_UNUSED(sender)
	Q_UNUSED(toggled)
}
