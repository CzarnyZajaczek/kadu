/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Longer (longer89@gmail.com)
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

#ifndef BUDDIES_LIST_VIEW_MENU_MANAGER_H
#define BUDDIES_LIST_VIEW_MENU_MANAGER_H

#include <QtCore/QObject>

#include "exports.h"

class QMenu;
class QWidget;

class ActionDataSource;
class ActionDescription;

class KADUAPI BuddiesListViewMenuManager : public QObject
{
	Q_OBJECT
	Q_DISABLE_COPY(BuddiesListViewMenuManager)

	static BuddiesListViewMenuManager *Instance;

	QList<ActionDescription *> BuddiesContexMenu;
	QList<ActionDescription *> BuddyListActions;

	BuddiesListViewMenuManager();

public:
	static BuddiesListViewMenuManager * instance();

	QList<ActionDescription *> buddiesContexMenu() { return BuddiesContexMenu; }
	QList<ActionDescription *> buddyListActions() { return BuddyListActions; }

	void addActionDescription(ActionDescription *actionDescription);
	void insertActionDescription(int pos, ActionDescription *actionDescription);
	void removeActionDescription(ActionDescription *actionDescription);
	void addSeparator();
	void insertSeparator(int pos);
	void addListActionDescription(ActionDescription *actionDescription);
	void insertListActionDescription(int pos, ActionDescription *actionDescription);
	void removeListActionDescription(ActionDescription *actionDescription);
	void addListSeparator();

	QMenu * menu(QWidget *parent, ActionDataSource *actionDataSource);

};

#endif // BUDDIES_LIST_VIEW_MENU_MANAGER_H
