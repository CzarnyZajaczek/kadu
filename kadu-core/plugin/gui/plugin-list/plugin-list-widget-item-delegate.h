/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "plugin/gui/plugin-list/plugin-list-view-delegate.h"

#include <QtCore/QPointer>
#include <injeqt/injeqt.h>
#include <memory>

class IconsManager;
class PluginListWidget;

class QCheckBox;
class QPushButton;

class PluginListWidgetItemDelegate : public PluginListWidgetDelegate
{
    Q_OBJECT

public:
    explicit PluginListWidgetItemDelegate(PluginListWidget *pluginSelector, QObject *parent = nullptr);
    virtual ~PluginListWidgetItemDelegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

protected:
    virtual QList<QWidget *> createItemWidgets() const override;
    virtual void updateItemWidgets(
        const QList<QWidget *> widgets, const QStyleOptionViewItem &option,
        const QPersistentModelIndex &index) const override;

private:
    QPointer<IconsManager> m_iconsManager;

    std::unique_ptr<QCheckBox> m_checkBox;
    std::unique_ptr<QPushButton> m_pushButton;
    PluginListWidget *m_pluginSelector;

    QFont titleFont(const QFont &baseFont) const;
    QFont subtitleFont(const QFont &baseFont) const;

private slots:
    INJEQT_SET void setIconsManager(IconsManager *iconsManager);
    INJEQT_INIT void init();

    void slotStateChanged(bool state);
    void slotAboutClicked();
};
