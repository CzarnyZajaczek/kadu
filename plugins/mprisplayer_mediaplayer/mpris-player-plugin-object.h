/*
 * %kadu copyright begin%
 * Copyright 2015 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "plugin/plugin-object.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class ConfigurationUiHandlerRepository;
class MPRISPlayerConfigurationUiHandler;
class MPRISPlayer;
class PathsProvider;
class PluginRepository;

class MprisPlayerPluginObject : public PluginObject
{
	Q_OBJECT

public:
	Q_INVOKABLE explicit MprisPlayerPluginObject(QObject *parent = nullptr);
	virtual ~MprisPlayerPluginObject();

	virtual void done() override;

private:
	QPointer<ConfigurationUiHandlerRepository> m_configurationUiHandlerRepository;
	QPointer<MPRISPlayerConfigurationUiHandler> m_mprisPlayerConfigurationUiHandler;
	QPointer<MPRISPlayer> m_mprisPlayer;
	QPointer<PathsProvider> m_pathsProvider;
	QPointer<PluginRepository> m_pluginRepository;

private slots:
	INJEQT_INIT void init();
	INJEQT_SETTER void setConfigurationUiHandlerRepository(ConfigurationUiHandlerRepository *configurationUiHandlerRepository);
	INJEQT_SETTER void setMPRISPlayerConfigurationUiHandler(MPRISPlayerConfigurationUiHandler *mprisPlayerConfigurationUiHandler);
	INJEQT_SETTER void setMPRISPlayer(MPRISPlayer *mprisPlayer);
	INJEQT_SETTER void setPathsProvider(PathsProvider *pathsProvider);
	INJEQT_SETTER void setPluginRepository(PluginRepository *pluginRepository);

};
