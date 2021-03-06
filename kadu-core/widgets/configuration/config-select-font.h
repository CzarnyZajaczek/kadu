/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2008, 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef CONFIG_SELECT_FONT_H
#define CONFIG_SELECT_FONT_H

#include "configuration/configuration-window-data-manager.h"
#include "widgets/configuration/config-widget-value.h"
#include "widgets/select-font.h"

class QLabel;

class ConfigGroupBox;

/**
        &lt;select-font caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigSelectFont : public SelectFont, public ConfigWidgetValue
{
    Q_OBJECT

    QLabel *label;

protected:
    virtual void createWidgets();

public:
    ConfigSelectFont(
        const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
        ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
    ConfigSelectFont(ConfigGroupBox *parentConfigGroupBox0, ConfigurationWindowDataManager *dataManager);
    virtual ~ConfigSelectFont();

    virtual void setVisible(bool visible);

    virtual void loadConfiguration();
    virtual void saveConfiguration();
};

#endif
