/*
 * %kadu copyright begin%
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/gui/configuration-ui-handler-repository.h"
#include "core/application.h"
#include "core/core.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

#include "hint_manager.h"
#include "hints-configuration-ui-handler.h"

#include "hints-plugin.h"

HintsPlugin * HintsPlugin::Instance = 0;

HintsPlugin::HintsPlugin(QObject *parent) :
		QObject(parent), HintManagerInstance(0)
{
	Instance = this;
}

HintsPlugin::~HintsPlugin()
{
	Instance = 0;
}

bool HintsPlugin::init(PluginRepository *pluginRepository)
{
	Q_UNUSED(pluginRepository)

	HintManagerInstance = new HintManager(this);
	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/hints.ui"));
	Core::instance()->configurationUiHandlerRepository()->addConfigurationUiHandler(HintManagerInstance->uiHandler());

	return true;
}

void HintsPlugin::done()
{
	Core::instance()->configurationUiHandlerRepository()->removeConfigurationUiHandler(HintManagerInstance->uiHandler());
	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/hints.ui"));
}

Q_EXPORT_PLUGIN2(hints, HintsPlugin)

#include "moc_hints-plugin.cpp"
