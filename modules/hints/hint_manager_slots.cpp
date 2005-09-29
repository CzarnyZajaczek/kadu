/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qapplication.h>
#include <qcheckbox.h>
#include <qcolordialog.h>
#include <qfontdialog.h>
#include <qlabel.h>
#include <qspinbox.h>
#include <qvbuttongroup.h>

#include "hint_manager_slots.h"
#include "config_dialog.h"
#include "config_file.h"
#include "debug.h"
#include "misc.h"

HintManagerSlots::HintManagerSlots(QObject *parent, const char *name) : QObject(parent, name)
{
	kdebugf();
	ConfigDialog::connectSlot("Hints", "Show message content in hint", SIGNAL(toggled(bool)), this, SLOT(toggled_ShowMessageContent(bool)));
	ConfigDialog::connectSlot("Hints", "Use custom syntax", SIGNAL(toggled(bool)), this, SLOT(toggled_UseNotifySyntax(bool)));
	ConfigDialog::connectSlot("Hints", "Own hints position", SIGNAL(toggled(bool)), this, SLOT(toggled_UseOwnPosition(bool)));
	ConfigDialog::connectSlot("Hints", "Set for all", SIGNAL(toggled(bool)), this, SLOT(toggled_SetAll(bool)));
	ConfigDialog::connectSlot("Hints", "Hint type", SIGNAL(clicked(int)), this, SLOT(clicked_HintType(int)));

	ConfigDialog::connectSlot("Hints", "Change font color", SIGNAL(clicked()), this, SLOT(clicked_ChangeFgColor()));
	ConfigDialog::connectSlot("Hints", "Change background color", SIGNAL(clicked()), this, SLOT(clicked_ChangeBgColor()));
	ConfigDialog::connectSlot("Hints", "Change font", SIGNAL(clicked()), this, SLOT(clicked_ChangeFont()));

	ConfigDialog::connectSlot("Hints", "Hint timeout", SIGNAL(valueChanged(int)), this, SLOT(changed_Timeout(int)));

	config_opts_prefixes<<"HintOnline"<<"HintOnlineD"<<"HintBusy"<<"HintBusyD"<<
					"HintInvisible"<<"HintInvisibleD"<<"HintOffline"<<"HintOfflineD"<<
					"HintBlocking"<<"HintNewChat"<<"HintNewMessage"<<"HintError"<<"HintMessage";
	CONST_FOREACH(prefix, config_opts_prefixes)
	{
		//wczytanie starego configa
		HintProperties prop=HintProperties::fromString(config_file.readEntry("Hints", *prefix));
		config_file.addVariable("Hints", (*prefix)+"_font", prop.font);
		config_file.addVariable("Hints", (*prefix)+"_fgcolor", prop.fgcolor);
		config_file.addVariable("Hints", (*prefix)+"_bgcolor", prop.bgcolor);
		config_file.addVariable("Hints", (*prefix)+"_timeout", (int)prop.timeout);
	}

	kdebugf2();
}

HintManagerSlots::~HintManagerSlots()
{
	kdebugf();
	ConfigDialog::disconnectSlot("Hints", "Show message content in hint", SIGNAL(toggled(bool)), this, SLOT(toggled_ShowMessageContent(bool)));
	ConfigDialog::disconnectSlot("Hints", "Use custom syntax", SIGNAL(toggled(bool)), this, SLOT(toggled_UseNotifySyntax(bool)));
	ConfigDialog::disconnectSlot("Hints", "Own hints position", SIGNAL(toggled(bool)), this, SLOT(toggled_UseOwnPosition(bool)));
	ConfigDialog::disconnectSlot("Hints", "Set for all", SIGNAL(toggled(bool)), this, SLOT(toggled_SetAll(bool)));

	ConfigDialog::disconnectSlot("Hints", "Hint type", SIGNAL(clicked(int)), this, SLOT(clicked_HintType(int)));

	ConfigDialog::disconnectSlot("Hints", "Change font color", SIGNAL(clicked()), this, SLOT(clicked_ChangeFgColor()));
	ConfigDialog::disconnectSlot("Hints", "Change background color", SIGNAL(clicked()), this, SLOT(clicked_ChangeBgColor()));
	ConfigDialog::disconnectSlot("Hints", "Change font", SIGNAL(clicked()), this, SLOT(clicked_ChangeFont()));

	ConfigDialog::disconnectSlot("Hints", "Hint timeout", SIGNAL(valueChanged(int)), this, SLOT(changed_Timeout(int)));
	kdebugf2();
}

void HintManagerSlots::onCreateTabHints()
{
	kdebugf();
	ConfigDialog::getSpinBox("Hints", "Hint timeout")->setSuffix(" s");
	ConfigDialog::getLabel("Hints", "<b>Text</b> preview")->setAlignment(Qt::AlignCenter);;

	toggled_ShowMessageContent(config_file.readBoolEntry("Hints", "ShowContentMessage"));
	toggled_UseNotifySyntax(config_file.readBoolEntry("Hints", "NotifyHintUseSyntax"));

	toggled_UseOwnPosition(config_file.readBoolEntry("Hints", "UseUserPosition"));
	toggled_SetAll(config_file.readBoolEntry("Hints", "SetAll"));
	
	config_hint_properties.clear();

	CONST_FOREACH(prefix, config_opts_prefixes)
	{
		HintProperties prop;
		prop.font=config_file.readFontEntry("Hints", (*prefix)+"_font");
		prop.fgcolor=config_file.readColorEntry("Hints", (*prefix)+"_fgcolor");
		prop.bgcolor=config_file.readColorEntry("Hints", (*prefix)+"_bgcolor");
		prop.timeout=config_file.readUnsignedNumEntry("Hints", (*prefix)+"_timeout");
		
		config_hint_properties[*prefix]=prop;
	}
	currentOptionPrefix.truncate(0);
	
	QVButtonGroup *group=ConfigDialog::getVButtonGroup("Hints", "Hint type");
	clicked_HintType(group->id(group->selected()));

	kdebugf2();
}

void HintManagerSlots::onApplyTabHints()
{
	kdebugf();
	
	CONST_FOREACH(prop, config_hint_properties)
	{
		config_file.writeEntry("Hints", prop.key()+"_font", prop.data().font);
		config_file.writeEntry("Hints", prop.key()+"_fgcolor", prop.data().fgcolor);
		config_file.writeEntry("Hints", prop.key()+"_bgcolor", prop.data().bgcolor);
		config_file.writeEntry("Hints", prop.key()+"_timeout", (int)prop.data().timeout);
	}
	kdebugf2();
}

void HintManagerSlots::onCloseTabHints()
{
	kdebugf();
	config_hint_properties.clear();
	kdebugf2();
}

void HintManagerSlots::clicked_HintType(int id)
{
	kdebugf();
	if (id == -1 || config_opts_prefixes[id] == currentOptionPrefix)
		return;
	currentOptionPrefix=config_opts_prefixes[id];
	QLabel *preview=ConfigDialog::getLabel("Hints", "<b>Text</b> preview");
	HintProperties prop=config_hint_properties[currentOptionPrefix];
	
	preview->setPaletteBackgroundColor(prop.bgcolor);
	preview->setPaletteForegroundColor(prop.fgcolor);
	preview->setFont(prop.font);
	ConfigDialog::getSpinBox("Hints", "Hint timeout")->setValue(prop.timeout);
	
	kdebugf2();
}

void HintManagerSlots::clicked_ChangeFont()
{
	kdebugf();
	bool ok;
	QLabel *preview=ConfigDialog::getLabel("Hints", "<b>Text</b> preview");
	QFont font = QFontDialog::getFont(&ok, preview->font(), 0);
	if (ok)
	{
		preview->setFont(font);
		if (ConfigDialog::getCheckBox("Hints", "Set for all")->isChecked())
			FOREACH(prop, config_hint_properties)
				prop.data().font = font;
		else
			config_hint_properties[currentOptionPrefix].font = font;
	}
	
	kdebugf2();
}

void HintManagerSlots::clicked_ChangeFgColor()
{
	kdebugf();
	QLabel *preview=ConfigDialog::getLabel("Hints", "<b>Text</b> preview");
	QColor color = QColorDialog::getColor(preview->paletteForegroundColor(), 0, tr("Color dialog"));
	if (color.isValid())
	{
		preview->setPaletteForegroundColor(color);
		if (ConfigDialog::getCheckBox("Hints", "Set for all")->isChecked())
			FOREACH(prop, config_hint_properties)
				prop.data().fgcolor = color;
		else
			config_hint_properties[currentOptionPrefix].fgcolor = color;
	}
	kdebugf2();
}

void HintManagerSlots::clicked_ChangeBgColor()
{
	kdebugf();
	QLabel *preview=ConfigDialog::getLabel("Hints", "<b>Text</b> preview");
	QColor color = QColorDialog::getColor(preview->paletteBackgroundColor(), 0, tr("Color dialog"));
	if (color.isValid())
	{
		preview->setPaletteBackgroundColor(color);
		if (ConfigDialog::getCheckBox("Hints", "Set for all")->isChecked())
			FOREACH(prop, config_hint_properties)
				prop.data().bgcolor = color;
		else
			config_hint_properties[currentOptionPrefix].bgcolor = color;
	}
	kdebugf2();
}

void HintManagerSlots::changed_Timeout(int value)
{
	kdebugf();

	if (ConfigDialog::getCheckBox("Hints", "Set for all")->isChecked())
		FOREACH(prop, config_hint_properties)
			prop.data().timeout = value;
	else
		config_hint_properties[currentOptionPrefix].timeout=value;

	kdebugf2();
}

void HintManagerSlots::toggled_ShowMessageContent(bool val)
{
	kdebugf();
	ConfigDialog::getSpinBox("Hints", "Number of quoted characters")->setEnabled(val);
	kdebugf2();
}

void HintManagerSlots::toggled_UseNotifySyntax(bool val)
{
	kdebugf();
	ConfigDialog::getLineEdit("Hints", "Hint syntax")->setEnabled(val);
	kdebugf2();
}

void HintManagerSlots::toggled_UseOwnPosition(bool val)
{
	kdebugf();
	ConfigDialog::getSpinBox("Hints", "x=")->setEnabled(val);
	ConfigDialog::getSpinBox("Hints", "y=")->setEnabled(val);
	ConfigDialog::getVButtonGroup("Hints", "Corner")->setEnabled(val);
	kdebugf2();
}

void HintManagerSlots::toggled_SetAll(bool val)
{
	kdebugf();
	ConfigDialog::getVButtonGroup("Hints", "Hint type")->setEnabled(!val);
	kdebugf2();
}

HintProperties HintProperties::fromString(const QString &oldstring)
{
	kdebugf();
	HintProperties prop;
	QStringList slist=QStringList::split(",", oldstring);
	int s=slist.size();
	if (s>=2)
		prop.font=QFont(slist[0], slist[1].toInt());
	else
		prop.font=QApplication::font();
	
	if (s>=3)
		prop.fgcolor=QColor(slist[2]);
	else
		prop.fgcolor=QColor("#000000");
	
	if (s>=4)
		prop.bgcolor=QColor(slist[3]);
	else
		prop.bgcolor=QColor("#F0F0F0");
	
	if (s>=5)
	{
		prop.timeout=slist[4].toUInt();
		if (prop.timeout==0)
			prop.timeout=10;
	}
	else
		prop.timeout=10;

	kdebugf2();
	return prop;
}
