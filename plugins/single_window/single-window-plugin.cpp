/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "core/application.h"
#include "core/core.h"
#include "gui/widgets/chat-widget/chat-widget-container-handler-repository.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/paths-provider.h"

#include "single-window-chat-widget-container-handler.h"
#include "single-window.h"

#include "single-window-plugin.h"

SingleWindowPlugin::~SingleWindowPlugin()
{
}

bool SingleWindowPlugin::init()
{
	m_singleWindowChatWidgetContainerHandler.reset(new SingleWindowChatWidgetContainerHandler());
	m_singleWindowManager.reset(new SingleWindowManager());
	MainConfigurationWindow::registerUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/single_window.ui"));

	m_singleWindowChatWidgetContainerHandler.data()->setSingleWindow(m_singleWindowManager.data()->window());
	Core::instance()->chatWidgetContainerHandlerRepository()->registerChatWidgetContainerHandler(m_singleWindowChatWidgetContainerHandler.data());

	return true;
}

void SingleWindowPlugin::done()
{
	Core::instance()->chatWidgetContainerHandlerRepository()->unregisterChatWidgetContainerHandler(m_singleWindowChatWidgetContainerHandler.data());
	m_singleWindowChatWidgetContainerHandler.data()->setSingleWindow(nullptr);

	MainConfigurationWindow::unregisterUiFile(Application::instance()->pathsProvider()->dataPath() + QLatin1String("plugins/configuration/single_window.ui"));
	m_singleWindowManager.reset();
	m_singleWindowChatWidgetContainerHandler.reset();
}

Q_EXPORT_PLUGIN2(single_window, SingleWindowPlugin)

#include "moc_single-window-plugin.cpp"
