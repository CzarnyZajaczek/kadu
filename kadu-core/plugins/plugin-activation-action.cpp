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

#include "plugin-activation-action.h"

#include "plugins/plugins-common.h"

PluginActivationAction::PluginActivationAction() :
		m_plugin{}
{
}

PluginActivationAction::PluginActivationAction(Plugin *plugin, PluginActivationReason activationReason) :
		m_plugin{plugin}, m_type{PluginActivationType::Activation}, m_activationReason{activationReason}
{
}

PluginActivationAction::PluginActivationAction(Plugin *plugin, PluginDeactivationReason deactivationReason) :
		m_plugin{plugin}, m_type{PluginActivationType::Deactivation}, m_deactivationReason{deactivationReason}
{
}

Plugin * PluginActivationAction::plugin() const
{
	return m_plugin;
}

PluginActivationType PluginActivationAction::type() const
{
	return m_type;
}

PluginActivationReason PluginActivationAction::activationReason() const
{
	return m_activationReason;
}

PluginDeactivationReason PluginActivationAction::deactivationReason() const
{
	return m_deactivationReason;
}
