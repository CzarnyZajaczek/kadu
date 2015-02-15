/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef LANGUAGES_MANAGER_H
#define LANGUAGES_MANAGER_H

#include <QtCore/QMap>
#include <QtCore/QStringList>

#include "exports.h"

class KADUAPI LanguagesManager
{
	Q_DISABLE_COPY(LanguagesManager)

	LanguagesManager();

	static QMap<QString, QString> Languages;

	static void loadLanguages();

public:
	static const QMap<QString, QString> & languages();

};

#endif // LANGUAGES_MANAGER_H
