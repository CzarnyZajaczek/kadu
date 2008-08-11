/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtGui/QApplication>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QScrollArea>
#include <QtGui/QVBoxLayout>

#include "config_file.h"
#include "configuration_aware_object.h"
#include "configuration_window_widgets.h"
#include "debug.h"
#include "emoticons.h"
#include "icons_manager.h"
#include "kadu.h"
#include "misc.h"

#include "configuration_window.h"

class ConfigSection;

class ConfigTab
{
	QString name;
	ConfigSection *configSection;

	QMap<QString, ConfigGroupBox *> configGroupBoxes;

	QScrollArea *scrollArea;
	QVBoxLayout *mainLayout;
	QWidget *mainWidget;

public:
	ConfigTab(const QString &name, ConfigSection *configSection, QTabWidget *tabWidget);
	~ConfigTab();

	ConfigGroupBox * configGroupBox(const QString &name, bool create = true);

	void removedConfigGroupBox(const QString &groupBoxName);

	QWidget *widget() { return mainWidget; }
	QWidget *tabWidget() { return mainWidget; }

};

ConfigGroupBox::ConfigGroupBox(const QString &name, ConfigTab *configTab, QGroupBox *groupBox)
	: name(name), configTab(configTab), groupBox(groupBox)
{
	container = new QWidget(groupBox);
	groupBox->layout()->addWidget(container);
	gridLayout = new QGridLayout(container);
	gridLayout->setAutoAdd(false);
	gridLayout->setSpacing(5);
	gridLayout->setColStretch(1, 100);
}

ConfigGroupBox::~ConfigGroupBox()
{
	delete groupBox;

	configTab->removedConfigGroupBox(name);
}

bool ConfigGroupBox::empty()
{
	return container->children().count() == 1;
}

void ConfigGroupBox::addWidget(QWidget *widget, bool fullSpace)
{
	int numRows = gridLayout->numRows();

	if (fullSpace)
		gridLayout->addMultiCellWidget(widget, numRows, numRows, 0, 1);
	else
		gridLayout->addWidget(widget, numRows, 1);
}

void ConfigGroupBox::addWidgets(QWidget *widget1, QWidget *widget2)
{
	int numRows = gridLayout->numRows();

	if (widget1)
		gridLayout->addWidget(widget1, numRows, 0, Qt::AlignRight);

	if (widget2)
		gridLayout->addWidget(widget2, numRows, 1);
}

ConfigTab::ConfigTab(const QString &name, ConfigSection *configSection, QTabWidget *tabWidget)
	: name(name), configSection(configSection)
{
	scrollArea = new QScrollArea(tabWidget);
	scrollArea->setFrameStyle(QFrame::NoFrame);
	scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	mainWidget = new QWidget(tabWidget);
	mainLayout = new QVBoxLayout(mainWidget);
	mainLayout->addStretch(1);

	tabWidget->addTab(scrollArea, name);
	scrollArea->setWidget(mainWidget);
	scrollArea->setWidgetResizable(true);
}

ConfigTab::~ConfigTab()
{
	delete scrollArea;
}

ConfigGroupBox *ConfigTab::configGroupBox(const QString &name, bool create)
{
	if (configGroupBoxes.contains(name))
		return configGroupBoxes[name];

	if (!create)
		return 0;

	QGroupBox *groupBox = new QGroupBox(name, mainWidget);
	groupBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	new QHBoxLayout(groupBox);
	
	mainLayout->insertWidget(configGroupBoxes.count(), groupBox);

	ConfigGroupBox *newConfigGroupBox = new ConfigGroupBox(name, this, groupBox);
	configGroupBoxes[name] = newConfigGroupBox;

	groupBox->show();

	return newConfigGroupBox;
}

void ConfigTab::removedConfigGroupBox(const QString &groupBoxName)
{
	configGroupBoxes.remove(groupBoxName);

	if (!configGroupBoxes.count())
	{
		configSection->removedConfigTab(name);
		delete this;
	}
}

ConfigSection::ConfigSection(const QString &name, ConfigurationWindow *configurationWindow, QListWidgetItem *listWidgetItem, QWidget *parentConfigGroupBoxWidget,
		const QString &pixmap)
	: name(name), configurationWindow(configurationWindow), pixmap(pixmap), listWidgetItem(listWidgetItem), activated(false)
{
	mainWidget = new QTabWidget(parentConfigGroupBoxWidget);
	parentConfigGroupBoxWidget->layout()->addWidget(mainWidget);
	mainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	mainWidget->hide();

	connect(icons_manager, SIGNAL(themeChanged()), this, SLOT(iconThemeChanged()));
}

ConfigSection::~ConfigSection()
{
	config_file.writeEntry("General", "ConfigurationWindow_" + configurationWindow->name() + "_" + name,
		mainWidget->label(mainWidget->currentPageIndex()));
	delete mainWidget;
}

ConfigGroupBox * ConfigSection::configGroupBox(const QString &tab, const QString &groupBox, bool create)
{
	ConfigTab *ct = configTab(tab, create);
	if (!ct)
		return 0;

	return ct->configGroupBox(groupBox, create);
}

void ConfigSection::activate()
{
	listWidgetItem->listWidget()->setCurrentItem(listWidgetItem);

	if (activated)
		return;

	QString tab = config_file.readEntry("General", "ConfigurationWindow_" + configurationWindow->name() + "_" + name);
	if (configTabs.contains(tab))
		mainWidget->setCurrentPage(mainWidget->indexOf(configTabs[tab]->tabWidget()));
	activated = true;
}

ConfigTab *ConfigSection::configTab(const QString &name, bool create)
{
	if (configTabs.contains(name))
		return configTabs[name];

	if (!create)
		return 0;

	ConfigTab *newConfigTab = new ConfigTab(name, this, mainWidget);
	configTabs[name] = newConfigTab;

	return newConfigTab;
}

void ConfigSection::removedConfigTab(const QString &configTabName)
{
	mainWidget->removePage(configTabs[configTabName]->widget());

	configTabs.remove(configTabName);
	if (!configTabs.count())
	{
		configurationWindow->removedConfigSection(name);
// 		delete this;
	}
}

void ConfigSection::iconThemeChanged()
{
	QListWidget *listWidget = listWidgetItem->listWidget();
	bool current = listWidgetItem->isSelected();
	delete listWidgetItem;

	listWidgetItem = new QListWidgetItem(icons_manager->loadPixmap(pixmap), name, listWidget);
	if (current)
		listWidget->setCurrentItem(listWidgetItem);
}

ConfigurationWindow::ConfigurationWindow(const QString &name, const QString &caption)
	: QWidget(kadu, Qt::Window), Name(name), currentSection(0)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(caption);

	QVBoxLayout *main_layout = new QVBoxLayout(this);

	QWidget *center = new QWidget();
	QHBoxLayout *center_layout = new QHBoxLayout(center);
	center_layout->setMargin(0);
	center_layout->setSpacing(0);

	left = new QWidget();
	left->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	left->hide();
	QVBoxLayout *left_layout = new QVBoxLayout(left);
	left_layout->setMargin(0);
	left_layout->setSpacing(0);

	container = new QWidget;
	QHBoxLayout *container_layout = new QHBoxLayout(container);
	Q_UNUSED(container_layout)
	container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	QWidget *buttons = new QWidget;
	QHBoxLayout *buttons_layout = new QHBoxLayout(buttons);
	buttons_layout->setMargin(0);
	buttons_layout->setSpacing(0);
	QWidget *what = new QWidget;
	what->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
	buttons_layout->addWidget(what);
	QPushButton *okButton = new QPushButton(icons_manager->loadIcon("OkWindowButton"), tr("Ok"), this);
	buttons_layout->addWidget(okButton);
	QPushButton *applyButton = new QPushButton(icons_manager->loadIcon("ApplyWindowButton"), tr("Apply"), this);
	buttons_layout->addWidget(applyButton);
	QPushButton *cancelButton = new QPushButton(icons_manager->loadIcon("CloseWindowButton"), tr("Cancel"), this);
	buttons_layout->addWidget(cancelButton);

	connect(okButton, SIGNAL(clicked()), this, SLOT(updateAndCloseConfig()));
	connect(applyButton, SIGNAL(clicked()), this, SLOT(updateConfig()));
	connect(cancelButton, SIGNAL(clicked()), this, SLOT(close()));

	sectionsListWidget = new QListWidget(this);
	sectionsListWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
 	sectionsListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	sectionsListWidget->setIconSize(QSize(32, 32));
	connect(sectionsListWidget, SIGNAL(currentTextChanged(const QString &)), this, SLOT(changeSection(const QString &)));
	left_layout->addWidget(sectionsListWidget);

	main_layout->addWidget(center);
	main_layout->addWidget(buttons);
	center_layout->addWidget(left);
	center_layout->addWidget(container);
}

ConfigurationWindow::~ConfigurationWindow()
{
	if (sectionsListWidget->currentItem())
		config_file.writeEntry("General", "ConfigurationWindow_" + Name, sectionsListWidget->currentItem()->text());
	qDeleteAll(configSections);
}

void ConfigurationWindow::show()
{
	if (!isShown())
	{
		QString lastSection = config_file.readEntry("General", "ConfigurationWindow_" + Name);
		if (configSections.contains(lastSection))
			configSections[lastSection]->activate();
		else
			sectionsListWidget->setCurrentItem(0);

		loadConfiguration(this);
		QWidget::show();
	}
	else
	{
		setActiveWindow();
		raise();
	}
}

QList<ConfigWidget *> ConfigurationWindow::appendUiFile(const QString &fileName, bool load)
{
	QList<ConfigWidget *> widgets = processUiFile(fileName);

	if (load)
		foreach(ConfigWidget *widget, widgets)
			if (widget)
				widget->loadConfiguration();

	return widgets;
}

void ConfigurationWindow::removeUiFile(const QString &fileName)
{
	processUiFile(fileName, false);
}

QList<ConfigWidget *>  ConfigurationWindow::processUiFile(const QString &fileName, bool append)
{
	kdebugf();

	QList<ConfigWidget *> result;
	QFile file(fileName);

	QDomDocument uiFile;
	file.open(QIODevice::ReadOnly);

	if (!uiFile.setContent(&file))
	{
		kdebugf2();
		file.close();
		return result;
	}

	file.close();

	QDomElement kaduConfigurationUi = uiFile.documentElement();
	if (kaduConfigurationUi.tagName() != "configuration-ui")
	{
		kdebugf2();
		return result;
	}

	QDomNodeList children = kaduConfigurationUi.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		result += processUiSectionFromDom(children.item(i), append);

	kdebugf2();
	return result;
}

QList<ConfigWidget *> ConfigurationWindow::processUiSectionFromDom(QDomNode sectionNode, bool append)
{
	kdebugf();

	QList<ConfigWidget *> result;
	if (!sectionNode.isElement())
	{
		kdebugf2();
		return result;
	}

	const QDomElement &sectionElement = sectionNode.toElement();
	if (sectionElement.tagName() != "section")
	{
		kdebugf2();
		return result;
	}

	const QString &iconName = sectionElement.attribute("icon");

	const QString &sectionName = sectionElement.attribute("name");
	if (sectionName.isEmpty())
	{
		kdebugf2();
		return result;
	}

	configSection(iconName, qApp->translate("@default", sectionName), true);

	const QDomNodeList children = sectionElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		result += processUiTabFromDom(children.item(i), iconName, sectionName, append);

	kdebugf2();
	return result;
}

QList<ConfigWidget *> ConfigurationWindow::processUiTabFromDom(QDomNode tabNode, const QString &iconName,
	const QString &sectionName, bool append)
{
	kdebugf();

	QList<ConfigWidget *> result;
	if (!tabNode.isElement())
	{
		kdebugf2();
		return result;
	}

	const QDomElement &tabElement = tabNode.toElement();
	if (tabElement.tagName() != "tab")
	{
		kdebugf2();
		return result;
	}

	const QString tabName = tabElement.attribute("name");
	if (tabName.isEmpty())
	{
		kdebugf2();
		return result;
	}

	const QDomNodeList &children = tabElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		result += processUiGroupBoxFromDom(children.item(i), sectionName, tabName, append);

	kdebugf2();
	return result;
}

QList<ConfigWidget *> ConfigurationWindow::processUiGroupBoxFromDom(QDomNode groupBoxNode, const QString &sectionName, const QString &tabName, bool append)
{
	kdebugf();

	QList<ConfigWidget *> result;
	if (!groupBoxNode.isElement())
	{
		kdebugf2();
		return result;
	}

	const QDomElement &groupBoxElement = groupBoxNode.toElement();
	if (groupBoxElement.tagName() != "group-box")
	{
		kdebugf2();
		return result;
	}

	const QString groupBoxName = groupBoxElement.attribute("name");
	if (groupBoxName.isEmpty())
	{
		kdebugf2();
		return result;
	}

	const QString groupBoxId = groupBoxElement.attribute("id");

	ConfigGroupBox *configGroupBoxWidget = configGroupBox(sectionName, tabName, groupBoxName, append);
	if (!configGroupBoxWidget)
	{
		kdebugf2();
		return result;
	}

	if (!groupBoxId.isEmpty())
		widgets[groupBoxId] = dynamic_cast<QWidget *>(configGroupBoxWidget->widget());

	const QDomNodeList &children = groupBoxElement.childNodes();
	int length = children.length();
	for (int i = 0; i < length; i++)
		if (append)
			result.append(appendUiElementFromDom(children.item(i), configGroupBoxWidget));
		else
			removeUiElementFromDom(children.item(i), configGroupBoxWidget);

	kdebugf2();
	return result;
}

ConfigWidget * ConfigurationWindow::appendUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox)
{
	kdebugf();

	if (!uiElementNode.isElement())
	{
		kdebugf2();
		return 0;
	}

	const QDomElement &uiElement = uiElementNode.toElement();
	const QString &tagName = uiElement.tagName();
	ConfigWidget *widget = 0;

	if (tagName == "line-edit")
		widget = new ConfigLineEdit(configGroupBox);
	else if (tagName == "gg-password-edit")
		widget = new ConfigGGPasswordEdit(configGroupBox);
	else if (tagName == "check-box")
		widget = new ConfigCheckBox(configGroupBox);
	else if (tagName == "spin-box")
		widget = new ConfigSpinBox(configGroupBox);
	else if (tagName == "combo-box")
		widget = new ConfigComboBox(configGroupBox);
	else if (tagName == "hot-key-edit")
		widget = new ConfigHotKeyEdit(configGroupBox);
	else if (tagName == "path-list-edit")
		widget = new ConfigPathListEdit(configGroupBox);
	else if (tagName == "color-button")
		widget = new ConfigColorButton(configGroupBox);
	else if (tagName == "select-font")
		widget = new ConfigSelectFont(configGroupBox);
	else if (tagName == "syntax-editor")
		widget = new ConfigSyntaxEditor(configGroupBox);
	else if (tagName == "action-button")
		widget = new ConfigActionButton(configGroupBox);
	else if (tagName == "select-file")
		widget = new ConfigSelectFile(configGroupBox);
	else if (tagName == "preview")
		widget = new ConfigPreview(configGroupBox);
	else if (tagName == "slider")
		widget = new ConfigSlider(configGroupBox);
	else if (tagName == "label")
		widget = new ConfigLabel(configGroupBox);
	else if (tagName == "list-box")
		widget = new ConfigListWidget(configGroupBox);
	else
	{
		kdebugf2();
		return 0;
	}

	if (!widget->fromDomElement(uiElement))
	{
		delete widget;
		kdebugf2();
		return 0;
	}

	QString id = uiElement.attribute("id");
	if (!id.isEmpty())
		widgets[id] = dynamic_cast<QWidget *>(widget);

	widget->show();

	kdebugf2();
	return widget;
}

void ConfigurationWindow::removeUiElementFromDom(QDomNode uiElementNode, ConfigGroupBox *configGroupBox)
{
	kdebugf();

	if (!uiElementNode.isElement())
	{
		kdebugf2();
		return;
	}

	const QDomElement &uiElement = uiElementNode.toElement();
	const QString &caption = uiElement.attribute("caption");

	foreach(QObject *child, configGroupBox->widget()->children())
	{
		ConfigWidget *configWidget = dynamic_cast<ConfigWidget *>(child);
		if (!configWidget)
			continue;

		if (configWidget->widgetCaption == caption)
		{
			delete configWidget;
			break;
		}
	}

	if (configGroupBox->empty())
		delete configGroupBox;

	kdebugf2();
}

QWidget * ConfigurationWindow::widgetById(const QString &id)
{
	if (widgets.contains(id))
		return widgets[id];

	return 0;
}

ConfigGroupBox * ConfigurationWindow::configGroupBox(const QString &section, const QString &tab, const QString &groupBox, bool create)
{
	ConfigSection *s = configSection(qApp->translate("@default", section));
	if (!s)
		return 0;

	return s->configGroupBox(qApp->translate("@default", tab), qApp->translate("@default", groupBox), create);
}

ConfigSection *ConfigurationWindow::configSection(const QString &name)
{
	return configSections[name];
}

ConfigSection *ConfigurationWindow::configSection(const QString &pixmap, const QString &name, bool create)
{
	if (configSections.contains(name))
		return configSections[name];

	if (!create)
		return 0;

	QListWidgetItem *newConfigSectionListWidgetItem = new QListWidgetItem(icons_manager->loadPixmap(pixmap), name, sectionsListWidget);

	QFontMetrics fontMetrics = sectionsListWidget->fontMetrics();
	// TODO: 48 = margins + scrollbar - get real scrollbar width
	int width = fontMetrics.width(name) + icons_manager->loadPixmap(pixmap).width() + 48;

	ConfigSection *newConfigSection = new ConfigSection(name, this, newConfigSectionListWidgetItem, container, pixmap);
	configSections[name] = newConfigSection;

	if (configSections.count() == 1)
		sectionsListWidget->setFixedWidth(width);

	if (configSections.count() > 1)
	{
		if (sectionsListWidget->width() < width)
			sectionsListWidget->setFixedWidth(width);
		left->show();
	}

	return newConfigSection;
}

void ConfigurationWindow::loadConfiguration(QObject *object)
{
	kdebugf();

	if (!object)
		return;

	const QObjectList children = object->children();
	foreach(QObject *child, children)
		loadConfiguration(child);

	ConfigWidget *configWidget = dynamic_cast<ConfigWidget *>(object);
	if (configWidget)
		configWidget->loadConfiguration();
}

void ConfigurationWindow::saveConfiguration(QObject *object)
{
	kdebugf();

	if (!object)
		return;

	const QObjectList children = object->children();
	foreach(QObject *child, children)
		saveConfiguration(child);

	ConfigWidget *configWidget = dynamic_cast<ConfigWidget *>(object);
	if (configWidget)
		configWidget->saveConfiguration();
}

void ConfigurationWindow::updateAndCloseConfig()
{
	updateConfig();
	close();
}

void ConfigurationWindow::updateConfig()
{
	emit configurationWindowApplied();
	saveConfiguration(this);

	ConfigurationAwareObject::notifyAll();
}

void ConfigurationWindow::changeSection(const QString &newSectionName)
{
	if (!configSections.contains(newSectionName))
		return;

	ConfigSection *newSection = configSections[newSectionName];
	if (newSection == currentSection)
		return;

	if (currentSection)
		currentSection->hide();

	currentSection = newSection;
	newSection->show();
	newSection->activate();
}

void ConfigurationWindow::removedConfigSection(const QString &sectionName)
{
	configSections.remove(sectionName);

	for (unsigned int i = 0; i < sectionsListWidget->count(); i++)
		if (sectionsListWidget->item(i)->text() == tr(sectionName))
		{
			QListWidgetItem *item = sectionsListWidget->takeItem(i);
			delete item;
			break;
		}
}

void ConfigurationWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QWidget::keyPressEvent(e);
}

#ifdef HAVE_OPENSSL
// 	ConfigDialog::addCheckBox("Network", "servergrid", QT_TRANSLATE_NOOP("@default", "Use TLSv1"), "UseTLS", false);
#endif
