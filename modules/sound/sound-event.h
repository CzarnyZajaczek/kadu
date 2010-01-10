/*
 * %kadu copyright begin%
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SOUND_EVENT_H
#define SOUND_EVENT_H

#include <QtCore/QEvent>

#include "sound.h"

class SoundEvent : public QEvent
{
	SoundDevice Data;
	static int EventNumber;

public:
	SoundEvent(SoundDevice data) : QEvent((QEvent::Type)EventNumber) { Data = data; }
	SoundDevice data() { return Data; }

	static int eventNumber() { return EventNumber; }

};

#endif // SOUND_EVENT_H
