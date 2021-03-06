/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtCore/QVariant>

#include "configuration/configuration.h"
#include "configuration/deprecated-configuration-api.h"
#include "core/core.h"

#include "config-file-variant-wrapper.h"

ConfigFileVariantWrapper::ConfigFileVariantWrapper(
    Configuration *configuration, const QString &group, const QString &name)
        : m_configuration{configuration}, Group(group), Name(name)
{
}

ConfigFileVariantWrapper::~ConfigFileVariantWrapper()
{
}

QVariant ConfigFileVariantWrapper::get(const QVariant &defaultValue) const
{
    return m_configuration->deprecatedApi()->readEntry(Group, Name, defaultValue.toString());
}

void ConfigFileVariantWrapper::set(const QVariant &value)
{
    m_configuration->deprecatedApi()->writeEntry(Group, Name, value.toString());
}
