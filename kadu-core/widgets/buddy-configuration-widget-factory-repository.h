/*
 * %kadu copyright begin%
 * Copyright 2013 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef BUDDY_CONFIGURATION_WIDGET_FACTORY_REPOSITORY_H
#define BUDDY_CONFIGURATION_WIDGET_FACTORY_REPOSITORY_H

#include <QtCore/QObject>

#include "exports.h"

class BuddyConfigurationWidgetFactory;

class KADUAPI BuddyConfigurationWidgetFactoryRepository : public QObject
{
    Q_OBJECT

    QList<BuddyConfigurationWidgetFactory *> Factories;

public:
    Q_INVOKABLE explicit BuddyConfigurationWidgetFactoryRepository(QObject *parent = nullptr);
    virtual ~BuddyConfigurationWidgetFactoryRepository();

    void registerFactory(BuddyConfigurationWidgetFactory *factory);
    void unregisterFactory(BuddyConfigurationWidgetFactory *factory);

    QList<BuddyConfigurationWidgetFactory *> factories() const;

signals:
    void factoryRegistered(BuddyConfigurationWidgetFactory *factory);
    void factoryUnregistered(BuddyConfigurationWidgetFactory *factory);
};

#endif   // BUDDY_CONFIGURATION_WIDGET_FACTORY_REPOSITORY_H
