/*
 * %kadu copyright begin%
 * Copyright 2009 Bartlomiej Zimon (uzi18@o2.pl)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef AVATAR_SERVICE_H
#define AVATAR_SERVICE_H

#include <QtCore/QObject>
#include <QtGui/QPixmap>

#include "exports.h"

class Contact;

class KADUAPI AvatarService : public QObject
{
	Q_OBJECT

public:
	AvatarService(QObject *parent = 0) : QObject(parent) {}

	virtual void fetchAvatar(Contact contact) = 0;

signals:
	void avatarFetched(Contact contact, const QByteArray &avatar);

};

// for MOC
#include "contacts/contact.h"

#endif // AVATAR_SERVICE_H
