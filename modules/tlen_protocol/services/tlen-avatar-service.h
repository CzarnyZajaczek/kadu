/*
 * %kadu copyright begin%
 * Copyright 2009 Bartlomiej Zimon (uzi18@o2.pl)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef TLEN_AVATAR_SERVICE_H
#define TLEN_AVATAR_SERVICE_H

#include <QList>

#include "contacts/contact.h"
#include "protocols/services/avatar-service.h"

class TlenAvatarService : public AvatarService
{
	Q_OBJECT

	QList<Contact> InProgress;

public:
	TlenAvatarService(QObject *parent = 0) : AvatarService(parent) {}

	void fetchAvatar(Contact contact);

private slots:
	void avatarReady(Contact contact, const QByteArray &avatar);

};

#endif // TLEN_AVATAR_SERVICE_H
