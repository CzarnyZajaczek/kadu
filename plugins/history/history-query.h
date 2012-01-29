/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_QUERY_H
#define HISTORY_QUERY_H

#include "talkable/talkable.h"

class HistoryQuery
{
	Talkable QueryTalkable;
	QString QueryString;

public:
	HistoryQuery();
	HistoryQuery(const HistoryQuery &copyMe);

	HistoryQuery & operator = (const HistoryQuery &copyMe);

	void setTalkable(const Talkable &talkable);
	Talkable talkable() const;

	void setString(const QString &string);
	QString string() const;

};

#endif //  HISTORY_QUERY_H
