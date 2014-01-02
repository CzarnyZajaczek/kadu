/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "plugin-state-storage.h"

#include "configuration/xml-configuration-file.h"
#include "storage/storage-point.h"

#include <QtCore/QVector>
#include <QtXml/QDomElement>

PluginStateStorage::PluginStateStorage(StoragePoint *storagePoint) :
		m_storagePoint{storagePoint}
{
}

PluginStateStorage::~PluginStateStorage()
{
}

QMap<QString, PluginState> PluginStateStorage::load() const
{
	if (!m_storagePoint)
		return {};

	auto result = QMap<QString, PluginState>();
	auto elements = m_storagePoint->storage()->getNodes(m_storagePoint->point(), QLatin1String("Plugin"));
	for (const auto &element : elements)
	{
		auto name = element.attribute("name");
		auto state = stringToState(m_storagePoint->storage()->getTextNode(element, QLatin1String("State")));
		result.insert(name, state);
	}

	return result;
}

PluginState PluginStateStorage::stringToState(const QString &string) const
{
	if (string == QLatin1String{"Loaded"})
		return PluginState::Enabled;
	else if (string == QLatin1String{"NotLoaded"})
		return PluginState::Disabled;
	else
		return PluginState::New;
}

void PluginStateStorage::store(const QMap<QString, PluginState> &pluginStates) const
{
	if (!m_storagePoint)
		return;

	m_storagePoint->storage()->removeChildren(m_storagePoint->point());

	for (const auto &name : pluginStates.keys())
	{
		auto stateString = stateToString(pluginStates.value(name));
		if (!stateString.isEmpty())
		{
			auto node = m_storagePoint->storage()->getNamedNode(m_storagePoint->point(), QLatin1String{"Plugin"}, name, XmlConfigFile::ModeAppend);
			m_storagePoint->storage()->appendTextNode(node, QLatin1String{"State"}, stateString);
		}
	}
}

QString PluginStateStorage::stateToString(PluginState state) const
{
	switch (state)
	{
		case PluginState::Enabled:
			return QLatin1String{"Loaded"};
		case PluginState::Disabled:
			return QLatin1String{"NotLoaded"};
		default:
			return {};
	}
}
