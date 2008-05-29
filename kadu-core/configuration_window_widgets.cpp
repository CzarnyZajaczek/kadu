/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QApplication>
#include <QToolTip>

#include "config_file.h"
#include "configuration_window.h"
#include "debug.h"
#include "misc.h"

#include "configuration_window_widgets.h"

ConfigWidget::ConfigWidget(ConfigGroupBox *parentConfigGroupBox)
	: parentConfigGroupBox(parentConfigGroupBox)
{
}

ConfigWidget::ConfigWidget(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox)
	: parentConfigGroupBox(parentConfigGroupBox), widgetCaption(widgetCaption), toolTip(toolTip)
{
}

bool ConfigWidget::fromDomElement(QDomElement domElement)
{
	widgetCaption = domElement.attribute("caption");

	if (widgetCaption.isEmpty())
		return false;

	ConfigWidget::toolTip = domElement.attribute("tool-tip");

	createWidgets();
	return true;
}

ConfigWidgetValue::ConfigWidgetValue(ConfigGroupBox *parentConfigGroupBox)
	: ConfigWidget(parentConfigGroupBox)
{
}

ConfigWidgetValue::ConfigWidgetValue(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox)
	: ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox), section(section), item(item)
{
}

bool ConfigWidgetValue::fromDomElement(QDomElement domElement)
{
	section = domElement.attribute("config-section");
	item = domElement.attribute("config-item");

	return ConfigWidget::fromDomElement(domElement);
}

ConfigLineEdit::ConfigLineEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: QLineEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigLineEdit::ConfigLineEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QLineEdit(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigLineEdit::~ConfigLineEdit()
{
	if (label)
		delete label;
}

void ConfigLineEdit::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::ConfigWidget::toolTip.isEmpty())
	{
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
		QToolTip::add(label, qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigLineEdit::loadConfiguration()
{
	setText(config_file.readEntry(section, item));
}

void ConfigLineEdit::saveConfiguration()
{
	config_file.writeEntry(section, item, text());
}

void ConfigLineEdit::show()
{
	label->show();
	QLineEdit::show();
}

void ConfigLineEdit::hide()
{
	label->hide();
	QLineEdit::hide();
}

ConfigGGPasswordEdit::ConfigGGPasswordEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: ConfigLineEdit(section, item, widgetCaption, toolTip, parentConfigGroupBox, name)
{
	setEchoMode(QLineEdit::Password);
}

ConfigGGPasswordEdit::ConfigGGPasswordEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: ConfigLineEdit(parentConfigGroupBox, name)
{
	setEchoMode(QLineEdit::Password);
}

void ConfigGGPasswordEdit::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setText(pwHash(config_file.readEntry(section, item)));
}

void ConfigGGPasswordEdit::saveConfiguration()
{
	if (section.isEmpty())
		return;

	config_file.writeEntry(section, item, pwHash(text()));
}

ConfigCheckBox::ConfigCheckBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: QCheckBox(widgetCaption, parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox)
{
	createWidgets();
}

ConfigCheckBox::ConfigCheckBox(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QCheckBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox)
{
}

void ConfigCheckBox::createWidgets()
{
	kdebugf();

	setText(qApp->translate("@default", widgetCaption));
	parentConfigGroupBox->addWidget(this, true);

	if (!ConfigWidget::toolTip.isEmpty())
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
}

void ConfigCheckBox::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setChecked(config_file.readBoolEntry(section, item));
	emit toggled(isChecked());
}

void ConfigCheckBox::saveConfiguration()
{
	if (section.isEmpty())
		return;

	config_file.writeEntry(section, item, isChecked());
}

void ConfigCheckBox::show()
{
	QCheckBox::show();
}

void ConfigCheckBox::hide()
{
	QCheckBox::hide();
}

ConfigSpinBox::ConfigSpinBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		int minValue, int maxValue, int step, ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QSpinBox(minValue, maxValue, step, parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox),
		label(0)
{
}

ConfigSpinBox::ConfigSpinBox(ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QSpinBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigSpinBox::~ConfigSpinBox()
{
	if (label)
		delete label;
}

void ConfigSpinBox::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
		QToolTip::add(label, qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigSpinBox::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setValue(config_file.readNumEntry(section, item));
	emit valueChanged(value());
}

void ConfigSpinBox::saveConfiguration()
{
	if (section.isEmpty())
		return;

	config_file.writeEntry(section, item, value());
}

void ConfigSpinBox::show()
{
	label->show();
	QSpinBox::show();
}

void ConfigSpinBox::hide()
{
	label->hide();
	QSpinBox::hide();
}

bool ConfigSpinBox::fromDomElement(QDomElement domElement)
{
	QString minValue = domElement.attribute("min-value");
	QString maxValue = domElement.attribute("max-value");
	QString step = domElement.attribute("step");
	setSuffix(domElement.attribute("suffix"));

	bool ok;

	setMinValue(minValue.toInt(&ok));
	if (!ok)
		return false;

	setMaxValue(maxValue.toInt(&ok));
	if (!ok)
		return false;

	setLineStep(step.toInt(&ok));
	if (!ok)
		setLineStep(1);

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigComboBox::ConfigComboBox(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QComboBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigComboBox::ConfigComboBox(ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QComboBox(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigComboBox::~ConfigComboBox()
{
	if (label)
		delete label;
}

void ConfigComboBox::setItems(const QStringList &itemValues, const QStringList &itemCaptions)
{
	this->itemValues = itemValues;
	this->itemCaptions = itemCaptions;

	clear();
	insertStringList(itemCaptions);
}

QString ConfigComboBox::currentItemValue()
{
	int index = currentItem();

	if ((index < 0) || (index >= itemValues.size()))
		return QString::null;

	return itemValues[index];
}

void ConfigComboBox::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	clear();
	insertStringList(itemCaptions);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
		QToolTip::add(label, qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigComboBox::loadConfiguration()
{
	if (section.isEmpty())
		return;

	QString currentValue = config_file.readEntry(section, item);
	setCurrentItem(itemValues.findIndex(currentValue));

	emit activated(currentItem());
}

void ConfigComboBox::saveConfiguration()
{
	if (section.isEmpty())
		return;

	int index = currentItem();

	if ((index < 0) || (index >= itemValues.size()))
		return;

	config_file.writeEntry(section, item, itemValues[currentItem()]);
}

void ConfigComboBox::show()
{
	label->show();
	QComboBox::show();
}

void ConfigComboBox::hide()
{
	label->hide();
	QComboBox::hide();
}

bool ConfigComboBox::fromDomElement(QDomElement domElement)
{
	QDomNodeList children = domElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
	{
		QDomNode node = children.item(i);
		if (node.isElement())
		{
			QDomElement element = node.toElement();
			if (element.tagName() != "item")
				continue;

			itemValues.append(element.attribute("value"));
			itemCaptions.append(qApp->translate("@default", element.attribute("caption")));

			insertItem(qApp->translate("@default", element.attribute("caption")));
		}
	}

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigHotKeyEdit::ConfigHotKeyEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: HotKeyEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigHotKeyEdit::ConfigHotKeyEdit(ConfigGroupBox *parentConfigGroupBox, char *name)
	: HotKeyEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigHotKeyEdit::~ConfigHotKeyEdit()
{
	if (label)
		delete label;
}

void ConfigHotKeyEdit::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
		QToolTip::add(label, qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigHotKeyEdit::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setShortCut(config_file.readEntry(section, item));
}

void ConfigHotKeyEdit::saveConfiguration()
{
	if (section.isEmpty())
		return;

	config_file.writeEntry(section, item, shortCutString());
}

void ConfigHotKeyEdit::show()
{
	label->show();
	HotKeyEdit::show();
}

void ConfigHotKeyEdit::hide()
{
	label->hide();
	HotKeyEdit::hide();
}

ConfigPathListEdit::ConfigPathListEdit(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox)
	: PathListEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigPathListEdit::ConfigPathListEdit(ConfigGroupBox *parentConfigGroupBox)
	: PathListEdit(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigPathListEdit::~ConfigPathListEdit()
{
	if (label)
		delete label;
}

void ConfigPathListEdit::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
		QToolTip::add(label, qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigPathListEdit::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setPathList(QStringList::split(QRegExp("(;|:)"), config_file.readEntry(section, item)));
}

void ConfigPathListEdit::saveConfiguration()
{
	if (section.isEmpty())
		return;

	config_file.writeEntry(section, item, pathList().join(":"));
}

void ConfigPathListEdit::show()
{
	label->show();
	PathListEdit::show();
}

void ConfigPathListEdit::hide()
{
	label->hide();
	PathListEdit::hide();
}

ConfigColorButton::ConfigColorButton(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: ColorButton(parentConfigGroupBox->widget()), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigColorButton::ConfigColorButton(ConfigGroupBox *parentConfigGroupBox, char *name)
	: ColorButton(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigColorButton::~ConfigColorButton()
{
	if (label)
		delete label;
}

void ConfigColorButton::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
		QToolTip::add(label, qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigColorButton::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setColor(config_file.readColorEntry(section, item));
}

void ConfigColorButton::saveConfiguration()
{
	if (section.isEmpty())
		return;

	config_file.writeEntry(section, item, color());
}

void ConfigColorButton::show()
{
	label->show();
	ColorButton::show();
}

void ConfigColorButton::hide()
{
	label->hide();
	ColorButton::hide();
}

ConfigSelectFont::ConfigSelectFont(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox)
	: SelectFont(parentConfigGroupBox->widget()), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigSelectFont::ConfigSelectFont(ConfigGroupBox *parentConfigGroupBox)
	: SelectFont(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigSelectFont::~ConfigSelectFont()
{
	if (label)
		delete label;
}

void ConfigSelectFont::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
		QToolTip::add(label, qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigSelectFont::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setFont(config_file.readFontEntry(section, item));
}

void ConfigSelectFont::saveConfiguration()
{
	if (section.isEmpty())
		return;

	config_file.writeEntry(section, item, font());
}

void ConfigSelectFont::show()
{
	label->show();
	SelectFont::show();
}

void ConfigSelectFont::hide()
{
	label->hide();
	SelectFont::hide();
}

ConfigSyntaxEditor::ConfigSyntaxEditor(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		ConfigGroupBox *parentConfigGroupBox, char *name)
	: SyntaxEditor(parentConfigGroupBox->widget(), name), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigSyntaxEditor::ConfigSyntaxEditor(ConfigGroupBox *parentConfigGroupBox, char *name)
	: SyntaxEditor(parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigSyntaxEditor::~ConfigSyntaxEditor()
{
	if (label)
		delete label;
}

void ConfigSyntaxEditor::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
		QToolTip::add(label, qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigSyntaxEditor::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setCurrentSyntax(config_file.readEntry(section, item));
}

void ConfigSyntaxEditor::saveConfiguration()
{
	if (section.isEmpty())
		return;

	config_file.writeEntry(section, item, currentSyntax());
}

void ConfigSyntaxEditor::show()
{
	label->show();
	SyntaxEditor::show();
}

void ConfigSyntaxEditor::hide()
{
	label->hide();
	SyntaxEditor::hide();
}

bool ConfigSyntaxEditor::fromDomElement(QDomElement domElement)
{
	QString category = domElement.attribute("category");
	QString syntaxHint = domElement.attribute("syntax-hint");
	if (category.isEmpty())
		return false;

	setCategory(category);
	setSyntaxHint(syntaxHint);

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigActionButton::ConfigActionButton(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, char *name)
	: QPushButton(parentConfigGroupBox->widget(), name), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox)
{
	createWidgets();
}

ConfigActionButton::ConfigActionButton(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QPushButton(parentConfigGroupBox->widget(), name), ConfigWidget(parentConfigGroupBox)
{
}

void ConfigActionButton::createWidgets()
{
	kdebugf();

	setText(qApp->translate("@default", widgetCaption));
	parentConfigGroupBox->addWidget(this);

	if (!ConfigWidget::toolTip.isEmpty())
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
}

void ConfigActionButton::show()
{
	QPushButton::show();
}

void ConfigActionButton::hide()
{
	QPushButton::hide();
}

ConfigSelectFile::ConfigSelectFile(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		const QString &type, ConfigGroupBox *parentConfigGroupBox)
	: SelectFile(type, parentConfigGroupBox->widget()), ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigSelectFile::ConfigSelectFile(ConfigGroupBox *parentConfigGroupBox)
	: SelectFile(parentConfigGroupBox->widget()), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigSelectFile::~ConfigSelectFile()
{
	if (label)
		delete label;
}

void ConfigSelectFile::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
		QToolTip::add(label, qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigSelectFile::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setFile(config_file.readEntry(section, item));
}

void ConfigSelectFile::saveConfiguration()
{
	if (section.isEmpty())
		return;

	config_file.writeEntry(section, item, file());
}

void ConfigSelectFile::show()
{
	label->show();
	SelectFile::show();
}

void ConfigSelectFile::hide()
{
	label->hide();
	SelectFile::hide();
}

bool ConfigSelectFile::fromDomElement(QDomElement domElement)
{
	QString type = domElement.attribute("type");
	if (type.isEmpty())
		return false;

	setType(type);

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigPreview::ConfigPreview(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox)
	: Preview(parentConfigGroupBox->widget()), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigPreview::ConfigPreview(ConfigGroupBox *parentConfigGroupBox)
	: Preview(parentConfigGroupBox->widget()), ConfigWidget(parentConfigGroupBox), label(0)
{
}

ConfigPreview::~ConfigPreview()
{
	if (label)
		delete label;
}

void ConfigPreview::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);
}

void ConfigPreview::show()
{
	label->show();
	Preview::show();
}

// TODO: czy Preview dzieczyczy z QToolTip?
void ConfigPreview::hide()
{
	label->hide();
	Preview::QWidget::hide();
}

ConfigSlider::ConfigSlider(const QString &section, const QString &item, const QString &widgetCaption, const QString &toolTip,
		int minValue, int maxValue, int pageStep, ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QSlider(minValue, maxValue, pageStep, 0, Qt::Horizontal, parentConfigGroupBox->widget(), name),
		ConfigWidgetValue(widgetCaption, toolTip, section, item, parentConfigGroupBox), label(0)
{
}

ConfigSlider::ConfigSlider(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QSlider(Qt::Horizontal, parentConfigGroupBox->widget(), name), ConfigWidgetValue(parentConfigGroupBox), label(0)
{
}

ConfigSlider::~ConfigSlider()
{
	if (label)
		delete label;
}

void ConfigSlider::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
		QToolTip::add(label, qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigSlider::loadConfiguration()
{
	if (section.isEmpty())
		return;

	setValue(config_file.readNumEntry(section, item));
	emit valueChanged(value());
}

void ConfigSlider::saveConfiguration()
{
	if (section.isEmpty())
		return;

	config_file.writeEntry(section, item, value());
}

void ConfigSlider::show()
{
	label->show();
	QSlider::show();
}

void ConfigSlider::hide()
{
	label->hide();
	QSlider::hide();
}

bool ConfigSlider::fromDomElement(QDomElement domElement)
{
	QString minValue = domElement.attribute("min-value");
	QString maxValue = domElement.attribute("max-value");
	QString pageStep = domElement.attribute("page-step");

	bool ok;

	setMinValue(minValue.toInt(&ok));
	if (!ok)
		return false;

	setMaxValue(maxValue.toInt(&ok));
	if (!ok)
		return false;

	setPageStep(pageStep.toInt(&ok));
	if (!ok)
		return false;

	return ConfigWidgetValue::fromDomElement(domElement);
}

ConfigLabel::ConfigLabel(const QString &widgetCaption, const QString &toolTip, ConfigGroupBox *parentConfigGroupBox, char *name)
	: QLabel(parentConfigGroupBox->widget(), name), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox)
{
	createWidgets();
}

ConfigLabel::ConfigLabel(ConfigGroupBox *parentConfigGroupBox, char *name)
	: QLabel(parentConfigGroupBox->widget(), name), ConfigWidget(parentConfigGroupBox)
{
}

void ConfigLabel::createWidgets()
{
	kdebugf();

	setText(qApp->translate("@default", widgetCaption));
	parentConfigGroupBox->addWidget(this);

	if (!ConfigWidget::toolTip.isEmpty())
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
}

void ConfigLabel::show()
{
	QLabel::show();
}

void ConfigLabel::hide()
{
	QLabel::hide();
}

ConfigListWidget::ConfigListWidget(const QString &widgetCaption, const QString &toolTip,
		const QStringList &itemValues, const QStringList &itemCaptions, ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QListWidget(parentConfigGroupBox->widget()), ConfigWidget(widgetCaption, toolTip, parentConfigGroupBox), label(0)
{
	createWidgets();
}

ConfigListWidget::ConfigListWidget(ConfigGroupBox *parentConfigGroupBox, const char *name)
	: QListWidget(parentConfigGroupBox->widget()), ConfigWidget(parentConfigGroupBox), label(0)
{
}

ConfigListWidget::~ConfigListWidget()
{
	if (label)
		delete label;
}

void ConfigListWidget::setItems(const QStringList &itemValues, const QStringList &itemCaptions)
{
	this->itemValues = itemValues;
	this->itemCaptions = itemCaptions;

	clear();
	addItems(itemCaptions);
}

void ConfigListWidget::createWidgets()
{
	kdebugf();

	label = new QLabel(this, qApp->translate("@default", widgetCaption) + ":", parentConfigGroupBox->widget());
	parentConfigGroupBox->addWidgets(label, this);

	clear();
	addItems(itemCaptions);

	if (!ConfigWidget::toolTip.isEmpty())
	{
		QToolTip::add(this, qApp->translate("@default", ConfigWidget::toolTip));
		QToolTip::add(label, qApp->translate("@default", ConfigWidget::toolTip));
	}
}

void ConfigListWidget::show()
{
	label->show();
	QListWidget::show();
}

void ConfigListWidget::hide()
{
	label->hide();
	QListWidget::hide();
}

bool ConfigListWidget::fromDomElement(QDomElement domElement)
{
	QDomNodeList children = domElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
	{
		QDomNode node = children.item(i);
		if (node.isElement())
		{
			QDomElement element = node.toElement();
			if (element.tagName() != "item")
				continue;

			itemValues.append(element.attribute("value"));
			itemCaptions.append(element.attribute("caption"));

			addItem(qApp->translate("@default", element.attribute("caption")));
		}
	}

	return ConfigWidget::fromDomElement(domElement);
}
