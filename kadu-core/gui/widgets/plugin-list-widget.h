/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef PLUGIN_LIST_WIDGET_H
#define PLUGIN_LIST_WIDGET_H

#include <QtGui/QWidget>

class QLineEdit;

class CategorizedListView;
class CategorizedListViewPainter;
class PluginModel;
class PluginListWidgetItemDelegate;
class ProxyModel;


/**
  * @short A widget to select what plugins to load and configure the plugins.
  *
  * It shows the list of available plugins
  *
  * @author Matthias Kretz <kretz@kde.org>
  * @author Rafael Fernández López <ereslibre@kde.org>
  */

class PluginListWidget : public QWidget
{
        Q_OBJECT

public:
        PluginListWidget(QWidget *parent = 0);
        ~PluginListWidget();

	void applyChanges();

        int dependantLayoutValue(int value, int width, int totalWidth) const;

	QWidget *parent;
        QLineEdit *lineEdit;
        CategorizedListView *listView;
        CategorizedListViewPainter *categoryDrawer;
        PluginModel *pluginModel;
        ProxyModel *proxyModel;
        PluginListWidgetItemDelegate *pluginDelegate;
        bool showIcons;

Q_SIGNALS:
        /**
          * Tells you whether the configuration is changed or not.
          */
        void changed(bool hasChanged);

        /**
          * Emitted after the config of an embedded KCM has been saved. The
          * argument is the name of the parent component that needs to reload
          * its config
          */
        void configCommitted(const QByteArray &componentName);
};

#endif
