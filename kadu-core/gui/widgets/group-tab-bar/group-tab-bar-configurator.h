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

#ifndef GROUP_TAB_BAR_CONFIGURATOR_H
#define GROUP_TAB_BAR_CONFIGURATOR_H

#include <QtCore/QWeakPointer>

#include "configuration/configuration-aware-object.h"

class QDomElement;

class GroupTabBar;
class GroupTabBarConfiguration;

class GroupTabBarConfigurator : private ConfigurationAwareObject
{
	QWeakPointer<GroupTabBar> ConfigurableGroupTabBar;

	void createDefaultConfiguration();

	void storeGroupFilters(const GroupTabBarConfiguration &configuration);
	void storeGroupFilter(QDomElement parentElement, const GroupFilter &groupFilter);

protected:
	virtual void configurationUpdated();

public:
	void setGroupTabBar(GroupTabBar *groupTabBar);
	GroupTabBarConfiguration configuration() const;

	void storeConfiguration();

};

#endif // GROUP_TAB_BAR_CONFIGURATOR_H
