/*
 * %kadu copyright begin%
 * Copyright 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HIDE_ANONYMOUS_TALKABLE_FILTER_H
#define HIDE_ANONYMOUS_TALKABLE_FILTER_H

#include "talkable/filter/talkable-filter.h"

/**
 * @addtogroup Talkable
 * @{
 */

/**
 * @class HideAnonymousTalkableFilter
 * @short Filter that removes items that are anonymous.
 *
 * This filter removes items that are anonymous. Anonymous Chat items have empty display() value.
 * Anonymous Buddy and Contact items have true isAnonymous() value.
 */
class HideAnonymousTalkableFilter : public TalkableFilter
{
    Q_OBJECT

    bool Enabled;

public:
    /**
     * @short Create new instance of HideAnonymousTalkableFilter with given parent.
     * @param parent QObject parent of new object
     */
    explicit HideAnonymousTalkableFilter(QObject *parent = nullptr);
    virtual ~HideAnonymousTalkableFilter();

    void setEnabled(bool enabled);

    virtual FilterResult filterChat(const Chat &chat);
    virtual FilterResult filterBuddy(const Buddy &buddy);
    virtual FilterResult filterContact(const Contact &contact);
};

/**
 * @}
 */

#endif   // HIDE_ANONYMOUS_TALKABLE_FILTER_H
