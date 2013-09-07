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

#include "otr-op-data.h"

OtrOpData::OtrOpData() :
		AppOpsWrapper(0), Notifier(0), PrivateKeyService(0)
{
}

OtrOpData::~OtrOpData()
{
}

void OtrOpData::setAppOpsWrapper(OtrAppOpsWrapper *appOpsWrapper)
{
	AppOpsWrapper = appOpsWrapper;
}

OtrAppOpsWrapper * OtrOpData::appOpsWrapper() const
{
	return AppOpsWrapper;
}

void OtrOpData::setNotifier(OtrNotifier *notifier)
{
	Notifier = notifier;
}

OtrNotifier * OtrOpData::notifier() const
{
	return Notifier;
}

void OtrOpData::setPrivateKeyService(OtrPrivateKeyService *privateKeyService)
{
	PrivateKeyService = privateKeyService;
}

OtrPrivateKeyService * OtrOpData::privateKeyService() const
{
	return PrivateKeyService;
}

void OtrOpData::setMessage(const Message &message)
{
	MyMessage = message;
}

Message OtrOpData::message() const
{
	return MyMessage;
}