/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Tomasz Rostański (rozteck@interia.pl)
 * Copyright 2010, 2011, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef GROUP_H
#define GROUP_H

#include "buddies/group-shared.h"
#include "exports.h"
#include "storage/shared-base.h"

class KADUAPI Group : public SharedBase<GroupShared>
{
    KaduSharedBaseClass(Group)

        public : static Group null;

    Group();
    Group(GroupShared *data);
    explicit Group(QObject *data);
    Group(const Group &copy);

    virtual ~Group();

    KaduSharedBase_PropertyCRW(QString, name, Name);
    KaduSharedBase_PropertyCRW(QString, icon, Icon);
    KaduSharedBase_Property(bool, notifyAboutStatusChanges, NotifyAboutStatusChanges);
    KaduSharedBase_Property(bool, showInAllGroup, ShowInAllGroup);
    KaduSharedBase_Property(bool, offlineToGroup, OfflineToGroup);
    KaduSharedBase_Property(bool, showIcon, ShowIcon);
    KaduSharedBase_Property(bool, showName, ShowName);
    KaduSharedBase_Property(int, tabPosition, TabPosition);
};

Q_DECLARE_METATYPE(Group)

#endif   // GROUP_H
