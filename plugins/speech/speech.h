/*
 * %kadu copyright begin%
 * Copyright 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef SPEECH_H
#define SPEECH_H

#include <QtCore/QTime>

#include "notification/notifier.h"

class Notification;

/**
 * @defgroup speech Speech
 * @{
 */

class Speech : public Notifier
{
	static Speech *Instance;

	QTime lastSpeech;

public:
	Speech();
	virtual ~Speech();

	virtual void notify(Notification *notification);
	NotifierConfigurationWidget * createConfigurationWidget(QWidget *parent);

	void say(const QString &s,
		const QString &path = QString(),
		bool klatt = false, bool melodie = false,
		const QString &sound_system = QString(), const QString &device = QString(),
		int freq = 0, int tempo = 0, int basefreq = 0);

	static Speech * instance();
	static void createInstance();
	static void destroyInstance();
};

/** @} */

#endif // SPEECH_H
