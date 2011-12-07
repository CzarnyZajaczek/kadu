/*
 * %kadu copyright begin%
 * Copyright 2011 Sławomir Stępień (s.stepien@interia.pl)
 * Copyright 2011 Michał Ziąbkowski (mziab@o2.pl)
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include "spellchecker-plugin.h"
#include "spellchecker.h" 

#include "spellchecker-configuration.h"

SpellcheckerConfiguration *SpellcheckerConfiguration::Instance = 0;

SpellcheckerConfiguration *SpellcheckerConfiguration::instance()
{
	return Instance;
}

void SpellcheckerConfiguration::createInstance()
{
	if (!Instance)
		Instance = new SpellcheckerConfiguration();

	Instance->configurationUpdated();
}

void SpellcheckerConfiguration::destroyInstance()
{
	delete Instance;
	Instance = 0;
}

SpellcheckerConfiguration::SpellcheckerConfiguration()
{
	createDefaultConfiguration();
}

SpellcheckerConfiguration::~SpellcheckerConfiguration()
{

}

void SpellcheckerConfiguration::createDefaultConfiguration()
{
	config_file.addVariable("ASpell", "Bold", "false");
	config_file.addVariable("ASpell", "Italic", "false");
	config_file.addVariable("ASpell", "Underline", "true");
	config_file.addVariable("ASpell", "Color", "#FF0101");
	config_file.addVariable("ASpell", "Checked", config_file.readEntry("General", "Language"));
	config_file.addVariable("ASpell", "Accents", "false");
	config_file.addVariable("ASpell", "Case", "false");
	config_file.addVariable("ASpell", "Suggester", "true");
	config_file.addVariable("ASpell", "SuggesterWordCount", "10");
}

void SpellcheckerConfiguration::configurationUpdated()
{
	bool bold = config_file.readBoolEntry("ASpell", "Bold", false);
	bool italic = config_file.readBoolEntry("ASpell", "Italic", false);
	bool underline = config_file.readBoolEntry("ASpell", "Underline", false);
	bool accents = config_file.readBoolEntry("ASpell", "Accents", false);
	bool caseSensivity = config_file.readBoolEntry("ASpell", "Case", false);
	bool suggester = config_file.readBoolEntry("ASpell", "Suggester", true);
	QColor colorMark("#FF0101");
	QColor color = config_file.readColorEntry("ASpell", "Color", &colorMark);
	QStringList checked = config_file.readEntry("ASpell", "Checked", config_file.readEntry("General", "Language")).split(',', QString::SkipEmptyParts);
	int suggesterWordCount = config_file.readNumEntry("ASpell", "SuggesterWordCount");

	if (bold == Bold && italic == Italic && underline == Underline && accents == Accents &&
			caseSensivity == Case && suggester == Suggester && color == Color &&
			checked == Checked && suggesterWordCount == SuggesterWordCount)
		return;

	Bold = bold;
	Italic = italic;
	Underline = underline;
	Accents = accents;
	Case = caseSensivity;
	Suggester = suggester;
	Color = color;
	Checked = checked;
	SuggesterWordCount = suggesterWordCount;

	SpellCheckerPlugin::instance()->spellChecker()->buildMarkTag();
	SpellCheckerPlugin::instance()->spellChecker()->buildCheckers();
}

void SpellcheckerConfiguration::setChecked(const QStringList &checked)
{
	config_file.writeEntry("ASpell", "Checked", checked.join(","));
}
