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

#ifndef OTR_BUDDY_CONFIGURATION_WIDGET_FACTORY_H
#define OTR_BUDDY_CONFIGURATION_WIDGET_FACTORY_H

#include <QtCore/QWeakPointer>

#include "gui/widgets/buddy-configuration-widget-factory.h"

class OtrPolicyService;

class OtrBuddyConfigurationWidgetFactory : public BuddyConfigurationWidgetFactory
{
	QWeakPointer<OtrPolicyService> PolicyService;

public:
	virtual ~OtrBuddyConfigurationWidgetFactory();

	void setPolicyService(OtrPolicyService *policyService);

	virtual BuddyConfigurationWidget * createWidget(const Buddy &buddy, QWidget *parent);

};

#endif // OTR_BUDDY_CONFIGURATION_WIDGET_FACTORY_H
