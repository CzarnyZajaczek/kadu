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

#include "plugin-translations-loader.h"

#include "configuration/configuration-file.h"
#include "misc/kadu-paths.h"

#include <QtCore/QLatin1String>
#include <QtCore/QTranslator>
#include <QtGui/QApplication>

PluginTranslationsLoader::PluginTranslationsLoader(const QString &pluginName) noexcept
{
	auto const lang = config_file.readEntry("General", "Language");

	m_translator.reset(new QTranslator);

	if (m_translator->load(pluginName + '_' + lang, KaduPaths::instance()->dataPath() + QLatin1String("plugins/translations")))
		qApp->installTranslator(m_translator.data());
	else
		m_translator.reset();
}

PluginTranslationsLoader::~PluginTranslationsLoader() noexcept
{
	qApp->removeTranslator(m_translator.data());
}
