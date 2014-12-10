/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2010, 2011, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/core.h"
#include "gui/actions/action-description.h"
#include "gui/actions/action.h"
#include "gui/windows/main-window.h"

#include "actions.h"

Actions * Actions::Instance = 0;

Actions * Actions::instance()
{
	if (!Instance)
		Instance = new Actions();

	return Instance;
}

Actions::Actions() :
	BlockSignals(false)
{
}

void Actions::insert(ActionDescription *action)
{
	QMap<QString, ActionDescription *>::insert(action->name(), action);

	if (!BlockSignals)
		emit actionLoaded(action);
}

void Actions::remove(ActionDescription *action)
{
	QMap<QString, ActionDescription *>::remove(action->name());

	if (!Core::instance()->isClosing())
		emit actionUnloaded(action);
}

QAction * Actions::createAction(const QString &name, ActionContext *context, QObject *parent)
{
	if (!contains(name))
		return nullptr;

	auto v = value(name);
	if (!v)
		return nullptr;

	auto result = v->createAction(context, parent);

	emit actionCreated(result);

	return result;
}

#include "moc_actions.cpp"
