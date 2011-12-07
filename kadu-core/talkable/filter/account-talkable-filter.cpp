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
#include "contacts/contact.h"
#include "contacts/contact-set.h"

#include "account-talkable-filter.h"

AccountTalkableFilter::AccountTalkableFilter(const Account &filterAccount, QObject *parent) :
		TalkableFilter(parent), FilterAccount(filterAccount)
{
	Q_ASSERT(FilterAccount);
}

AccountTalkableFilter::~AccountTalkableFilter()
{
}

TalkableFilter::FilterResult AccountTalkableFilter::filterChat(const Chat &chat)
{
	if (chat.chatAccount() == FilterAccount)
		return Undecided;
	else
		return Rejected;
}

TalkableFilter::FilterResult AccountTalkableFilter::filterBuddy(const Buddy &buddy)
{
	if (buddy.hasContact(FilterAccount))
		return Undecided;
	else
		return Rejected;
}

TalkableFilter::FilterResult AccountTalkableFilter::filterContact(const Contact &contact)
{
	if (contact.contactAccount() == FilterAccount)
		return Undecided;
	else
		return Rejected;
}
