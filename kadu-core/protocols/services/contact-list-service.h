/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONTAT_LIST_SERVICE_H
#define CONTAT_LIST_SERVICE_H

#include <QtCore/QObject>

#include "buddies/buddy-list.h"

class ContactListService : public QObject
{
	Q_OBJECT

public:
	ContactListService(QObject *parent = 0)
		: QObject(parent) {}

	virtual void importContactList() = 0;
	virtual void exportContactList() = 0;
	virtual void exportContactList(BuddyList buddies) = 0;

signals:
	void contactListImported(bool ok, BuddyList buddies);
	void contactListExported(bool ok);

};

#endif // CONTAT_LIST_SERVICE_H
