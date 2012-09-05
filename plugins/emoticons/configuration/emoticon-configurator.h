/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef EMOTICON_CONFIGURATOR
#define EMOTICON_CONFIGURATOR

#include <QtCore/QScopedPointer>
#include <QtCore/QWeakPointer>

#include "theme/emoticon-theme-manager.h"
#include "configuration/emoticon-configuration.h"

#include "configuration/configuration-holder.h"

class EmoticonExpanderDomVisitorProvider;
class InsertEmoticonAction;

class EmoticonConfigurator : public ConfigurationHolder
{
	Q_OBJECT

	QString LastLoadedThemeName;
	EmoticonConfiguration Configuration;

	QScopedPointer<EmoticonThemeManager> ThemeManager;
	QWeakPointer<InsertEmoticonAction> InsertAction;
	QWeakPointer<EmoticonExpanderDomVisitorProvider> EmoticonExpanderProvider;

	void createDefaultConfiguration();

protected:
	virtual void configurationUpdated();

public:
	EmoticonConfigurator();

	void setInsertAction(InsertEmoticonAction *insertAction);
	void setEmoticonExpanderProvider(EmoticonExpanderDomVisitorProvider *emoticonExpanderProvider);

	void configure();

};

#endif // EMOTICON_CONFIGURATOR

