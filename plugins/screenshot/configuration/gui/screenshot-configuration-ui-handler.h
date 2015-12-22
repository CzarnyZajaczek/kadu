/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SCREENSHOT_CONFIGURATION_UI_HANDLER_H
#define SCREENSHOT_CONFIGURATION_UI_HANDLER_H

#include "configuration/gui/configuration-ui-handler.h"

class ScreenShotConfigurationUiHandler : public ConfigurationUiHandler
{
	static ScreenShotConfigurationUiHandler *Instance;

	explicit ScreenShotConfigurationUiHandler();
	virtual ~ScreenShotConfigurationUiHandler();

public:
	static void registerConfigurationUi();
	static void unregisterConfigurationUi();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
	virtual void mainConfigurationWindowDestroyed() override;
	virtual void mainConfigurationWindowApplied() override;

};

#endif // SCREENSHOT_CONFIGURATION_UI_HANDLER_H
