/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration-file.h"
#include "misc/kadu-paths.h"
#include "kadu-application.h"

#include "screen-shot-configuration.h"

ScreenShotConfiguration * ScreenShotConfiguration::Instance = 0;

ScreenShotConfiguration * ScreenShotConfiguration::instance()
{
	return Instance;
}

void ScreenShotConfiguration::createInstance()
{
	if (!Instance)
		Instance = new ScreenShotConfiguration();
}

void ScreenShotConfiguration::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

ScreenShotConfiguration::ScreenShotConfiguration()
{
	createDefaultConfiguration();
	configurationUpdated();
}

ScreenShotConfiguration::~ScreenShotConfiguration()
{
}

void ScreenShotConfiguration::createDefaultConfiguration()
{
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ScreenShot", "fileFormat", "PNG");
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ScreenShot", "use_short_jpg", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ScreenShot", "quality", -1);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ScreenShot", "path", KaduPaths::instance()->profilePath() + QLatin1String("images/"));
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ScreenShot", "filenamePrefix", "shot");
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ScreenShot", "paste_clause", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ScreenShot", "dir_size_warns", true);
	KaduApplication::instance()->deprecatedConfigurationApi()->addVariable("ScreenShot", "dir_size_limit", 10000);
}

void ScreenShotConfiguration::configurationUpdated()
{
	FileFormat = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("ScreenShot", "fileFormat", "PNG");
	UseShortJpgExtension = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("ScreenShot", "use_short_jpg", true);
	Quality = KaduApplication::instance()->deprecatedConfigurationApi()->readNumEntry("ScreenShot", "quality", -1);
	ImagePath = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("ScreenShot", "path", KaduPaths::instance()->profilePath() + QLatin1String("images/"));
	FileNamePrefix = KaduApplication::instance()->deprecatedConfigurationApi()->readEntry("ScreenShot", "filenamePrefix", "shot");
	PasteImageClauseIntoChatWidget = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("ScreenShot", "paste_clause", true);
	WarnAboutDirectorySize = KaduApplication::instance()->deprecatedConfigurationApi()->readBoolEntry("ScreenShot", "dir_size_warns", true);
	DirectorySizeLimit = KaduApplication::instance()->deprecatedConfigurationApi()->readNumEntry("ScreenShot", "dir_size_limit", 10000);
}

QString ScreenShotConfiguration::screenshotFileNameExtension()
{
	bool useShortJpg = useShortJpgExtension();
	QString extension = fileFormat();
	if (useShortJpg && extension == "jpeg")
		return QLatin1String("jpg");

	return extension;
}
