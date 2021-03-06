/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "exports.h"

#include <QtCore/QStringList>

class ExecutionArguments;

/**
 * @addtogroup Core
 * @{
 */

/**
 * @class ExecutionArgumentsParser
 * @short Parser of arguments used to run Kadu.
 *
 * Parse arguments used to run Kadu into nice and readable ExecutionArguments
 * class.
 */
class KADUAPI ExecutionArgumentsParser final
{
public:
    /**
     * @short Parse arguments used to run Kadu into nice and readable
     * ExecutionArguments class.
     * @param arguments arguments used to run Kadu
     *
     * Please not that arguments variable should not contain executable name as
     * its first value.
     * It should be QCoreApplication::arguments().mid(1) instead of
     * QCoreApplication::arguments().
     */
    ExecutionArguments parse(const QStringList &arguments) const;
};

/**
 * @}
 */
