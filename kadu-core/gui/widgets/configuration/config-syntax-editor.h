/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CONFIG_SYNTAX_EDITOR_H
#define CONFIG_SYNTAX_EDITOR_H

#include "gui/widgets/configuration/config-widget-value.h"
#include "gui/widgets/syntax-editor.h"
#include "configuration/configuration-window-data-manager.h"

class QLabel;
class QDomElement;
class ConfigGroupBox;

/**
	&lt;syntax-editro caption="caption" id="id" /&gt;
 **/
class KADUAPI ConfigSyntaxEditor : public SyntaxEditor, public ConfigWidgetValue
{
	QLabel *label;

protected:
	virtual void createWidgets();

public:
	ConfigSyntaxEditor(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	ConfigSyntaxEditor(ConfigGroupBox *parentConfigGroupBox, ConfigurationWindowDataManager *dataManager);
	virtual ~ConfigSyntaxEditor();

	virtual void loadConfiguration();
	virtual void saveConfiguration();

	virtual void show();
	virtual void hide();
	virtual bool fromDomElement(QDomElement domElement);

};

#endif // CONFIGURATION_WINDOW_WIDGETS_H
