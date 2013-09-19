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

#include "contacts/contact.h"

#include "otr-context-converter.h"
#include "otr-user-state.h"

#include "otr-trust-level-service.h"

OtrTrustLevelService::OtrTrustLevelService(QObject *parent) :
		QObject(parent), UserState(0)
{
}

OtrTrustLevelService::~OtrTrustLevelService()
{
}

void OtrTrustLevelService::setUserState(OtrUserState *userState)
{
	UserState = userState;
}

void OtrTrustLevelService::setContextConverter(OtrContextConverter *contextConverter)
{
	ContextConverter = contextConverter;
}

void OtrTrustLevelService::storeTrustLevelToContact(const Contact &contact, TrustLevel level) const
{
	contact.addProperty("otr:trustLevel", (int)level, CustomProperties::NonStorable);
}

OtrTrustLevelService::TrustLevel OtrTrustLevelService::loadTrustLevelFromContact(const Contact &contact) const
{
	return (TrustLevel)contact.property("otr:trustLevel", QVariant()).toInt();
}

OtrTrustLevelService::TrustLevel OtrTrustLevelService::trustLevelFromContext(ConnContext *context) const
{
	if (!context)
		return TrustLevelNotPrivate;

	if (context->msgstate == OTRL_MSGSTATE_FINISHED)
		return TrustLevelNotPrivate;

	if (context->msgstate != OTRL_MSGSTATE_ENCRYPTED)
		return TrustLevelNotPrivate;

	if (!context->active_fingerprint)
		return TrustLevelUnverified;

	if (!context->active_fingerprint->trust)
		return TrustLevelUnverified;

	if (context->active_fingerprint->trust[0] == '\0')
		return TrustLevelUnverified;

	return TrustLevelPrivate;
}

void OtrTrustLevelService::updateTrustLevels()
{
	if (!ContextConverter)
		return;

	ConnContext *context = UserState->userState()->context_root;
	while (context)
	{
		Contact contact = ContextConverter.data()->connectionContextToContact(context);
		storeTrustLevelToContact(contact, trustLevelFromContext(context));

		context = context->next;
	}

	emit trustLevelsUpdated();
}
