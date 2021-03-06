/*
 * %kadu copyright begin%
 * Copyright 2012 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "buddies/buddy.h"
#include "exports.h"

#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <injeqt/injeqt.h>

class BuddyListModel;
class BuddyManager;

/**
 * @addtogroup Buddy
 * @{
 */

/**
 * @class BuddyManagerAdapter
 * @short Adapter that makes BuddyListModel to use copy of buddies from BuddiesManager.
 *
 * This adapter created with a @link BuddyListModel @endlink as parent replicates list of buddies from
 * @link BuddiesManager @endlink on this model. After creation other, non-managable, buddies can be
 * added and removed from @link BuddyListModel @endlink. Removing or adding managable buddies is undefined.
 */
class KADUAPI BuddyManagerAdapter : public QObject
{
    Q_OBJECT

public:
    /**
     * @short Create new BuddyManagerAdapter on given @link BuddyListModel @endlink.
     * @param model model to adapt @link BuddiesManager @endlink to
     *
     * Given @link BuddyListModel @endlink will now have exactly the same data as @link BuddiesManager @endlink
     * singleton.
     * Non-managable buddies can be added or removed from this model.
     */
    explicit BuddyManagerAdapter(BuddyListModel *model);
    virtual ~BuddyManagerAdapter();

private:
    QPointer<BuddyManager> m_buddyManager;

    BuddyListModel *Model;

private slots:
    INJEQT_SET void setBuddyManager(BuddyManager *buddyManager);
    INJEQT_INIT void init();

    /**
     * @short Slot called after a buddy is added to @link BuddiesManager @endlink singleton.
     * @param buddy added buddy
     *
     * Buddy is added to @link BuddyListModel @endlink instance.
     */
    void buddyAdded(const Buddy &buddy);

    /**
     * @short Slot called after a buddy is removed from @link BuddiesManager @endlink singleton.
     * @param buddy removed buddy
     *
     * Buddy is removed from @link BuddyListModel @endlink instance.
     */
    void buddyRemoved(const Buddy &buddy);
};

/**
 * @}
 */
