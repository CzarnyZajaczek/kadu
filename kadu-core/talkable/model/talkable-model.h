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

#ifndef TALKABLE_MODEL_H
#define TALKABLE_MODEL_H

#include "model/kadu-abstract-model.h"
#include "model/merged-proxy-model.h"

class BuddiesModel;
class ChatsModel;

class TalkableModel : public MergedProxyModel, public KaduAbstractModel
{
	Q_OBJECT

	ChatsModel *Chats;
	BuddiesModel *Buddies;

public:
	explicit TalkableModel(QObject *parent = 0);
	virtual ~TalkableModel();

	virtual QModelIndexList indexListForValue(const QVariant &value) const;

	void setIncludeMyself(bool includeMyself);
	bool includeMyself() const;

};

#endif // TALKABLE_MODEL_H
