/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2012, 2013 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2012, 2013 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * %kadu copyright end%
 *
 * This file is derived from part of the KDE project
 * Copyright (C) 2007, 2006 Rafael Fernández López <ereslibre@kde.org>
 * Copyright (C) 2002-2003 Matthias Kretz <kretz@kde.org>
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

#include <QtGui/QBoxLayout>
#include <QtGui/QStyleOptionViewItemV4>

#include "configuration/configuration-manager.h"
#include "gui/widgets/categorized-list-view-painter.h"
#include "gui/widgets/categorized-list-view.h"
#include "gui/widgets/configuration/config-section.h"
#include "gui/widgets/configuration/configuration-widget.h"
#include "gui/widgets/filter-widget.h"
#include "gui/widgets/plugin-list/plugin-list-view-delegate.h"
#include "gui/widgets/plugin-list/plugin-list-widget-item-delegate.h"
#include "gui/windows/main-configuration-window.h"
#include "plugin/model/plugin-model.h"
#include "plugin/model/plugin-proxy-model.h"
#include "plugin/activation/plugin-activation-service.h"
#include "plugin/plugin-manager.h"
#include "plugin/state/plugin-state.h"
#include "plugin/state/plugin-state-service.h"

#include "plugin-list-widget.h"


PluginListWidget::PluginListWidget(MainConfigurationWindow *window) :
		QWidget(window), ListView(0), ShowIcons(false), m_processingChange{false}
{
	QVBoxLayout *layout = new QVBoxLayout;
	layout->setMargin(0);
	setLayout(layout);

	LineEdit = new FilterWidget(this);
	LineEdit->setAutoVisibility(false);
	ListView = new CategorizedListView(this);
	ListView->setVerticalScrollMode(QListView::ScrollPerPixel);
	ListView->setAlternatingRowColors(true);
	CategoryDrawer = new CategorizedListViewPainter(ListView);
	ListView->setCategoryDrawer(CategoryDrawer);

	Model = new PluginModel(this, this);
	connect(Model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(modelDataChanged(QModelIndex,QModelIndex)));

	Proxy = new PluginProxyModel(this);
	Proxy->setCategorizedModel(true);
	Proxy->setSourceModel(Model);
	ListView->setModel(Proxy);
	ListView->setAlternatingRowColors(true);

	Delegate = new PluginListWidgetItemDelegate(this, this);
	ListView->setItemDelegate(Delegate);

	ListView->setMouseTracking(true);
	ListView->viewport()->setAttribute(Qt::WA_Hover);

	LineEdit->setView(ListView);

	connect(LineEdit, SIGNAL(textChanged(QString)), Proxy, SLOT(setFilterText(QString)));
	connect(Delegate, SIGNAL(changed(bool)), this, SIGNAL(changed(bool)));

	layout->addWidget(LineEdit);
	layout->addWidget(ListView);

	ConfigSection *pluginsSection = window->widget()->configSection("Plugins");
	if (pluginsSection)
		pluginsSection->addFullPageWidget("Plugins", this);

	connect(window, SIGNAL(configurationWindowApplied()), this, SLOT(configurationApplied()));
}

PluginListWidget::~PluginListWidget()
{
	delete ListView->itemDelegate();
	delete ListView;
	delete CategoryDrawer;
}

void PluginListWidget::setPluginActivationService(PluginActivationService *pluginActivationService)
{
	m_pluginActivationService = pluginActivationService;
	Model->setPluginActivationService(pluginActivationService);
	Model->loadPluginData();
}

void PluginListWidget::setPluginStateService(PluginStateService *pluginStateService)
{
	m_pluginStateService = pluginStateService;
}

void PluginListWidget::setPluginManager(PluginManager *pluginManager)
{
	m_pluginManager = pluginManager;
	Model->setPluginManager(pluginManager);
	Model->loadPluginData();
}

int PluginListWidget::dependantLayoutValue(int value, int width, int totalWidth) const
{
	if (ListView->layoutDirection() == Qt::LeftToRight)
		return value;

	return totalWidth - width - value;
}

void PluginListWidget::applyChanges()
{
	auto storeList = false;

	if (m_pluginManager)
	{
		for (auto const &pluginName : pluginsWithNewActiveState(false))
		{
			storeList = true;
			m_pluginManager->deactivatePluginWithDependents(pluginName);
			if (m_pluginStateService)
				for (auto const &dependentPlugin : m_pluginManager->withDependents(pluginName))
					m_pluginStateService->setPluginState(dependentPlugin, PluginState::Disabled);
		}

		for (auto const &pluginName : pluginsWithNewActiveState(true))
			if (m_pluginManager->activatePluginWithDependencies(pluginName))
			{
				storeList = true;
				if (m_pluginStateService)
					for (auto const &dependencyPlugin : m_pluginManager->withDependencies(pluginName))
						m_pluginStateService->setPluginState(dependencyPlugin, PluginState::Enabled);
			}
	}

	Model->loadPluginData();

	if (storeList)
	{
		m_pluginManager->storePluginStates();
		ConfigurationManager::instance()->flush();
	}

	emit changed(false);
}

QVector<QString> PluginListWidget::pluginsWithNewActiveState(bool newActiveState) const
{
	auto result = QVector<QString>{};

	int count = Model->rowCount();
	for (int i = 0; i < count; i++)
	{
		auto pluginEntry = static_cast<PluginEntry*>(Model->index(i, 0).internalPointer());
		if ((m_pluginActivationService->isActive(pluginEntry->pluginName) != pluginEntry->checked) && (newActiveState == pluginEntry->checked))
				result.append(pluginEntry->pluginName);
	}

	return result;
}

void PluginListWidget::configurationApplied()
{
	applyChanges();
}

void PluginListWidget::modelDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
	if (m_processingChange)
		return;

	if (!m_pluginManager)
		return;

	// we do not know how to work with multiple rows!
	if (topLeft.row() != bottomRight.row())
		return;

	m_processingChange = true;

	auto pluginName = topLeft.data(PluginModel::NameRole).toString();
	auto checked = topLeft.data(Qt::CheckStateRole).toBool();

	auto otherPlugins = checked
			? m_pluginManager.data()->withDependencies(pluginName)
			: m_pluginManager.data()->withDependents(pluginName);
	setAllChecked(otherPlugins, checked);

	m_processingChange = false;
}

void PluginListWidget::setAllChecked(const QVector<QString> &plugins, bool checked)
{
	auto count = Model->rowCount();
	for (auto i = 0; i < count; i++)
	{
		auto index = Model->index(i, 0);
		auto pluginName = index.data(PluginModel::NameRole).toString();
		if (contains(plugins, pluginName))
			Model->setData(index, checked, Qt::CheckStateRole);
	}
}

#include "moc_plugin-list-widget.cpp"
