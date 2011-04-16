/*
 * %kadu copyright begin%
 * Copyright 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef JABBER_FILE_TRANSFER_PRIVATE_H
#define JABBER_FILE_TRANSFER_PRIVATE_H

#include <QtCore/QObject>

#include <s5b.h>

using namespace XMPP;

class JabberFileTransferHandlerPrivate : public QObject
{
	Q_OBJECT

public:
	JabberFileTransferHandlerPrivate(QObject *parent) : QObject(parent) {}

public slots:
	void s5b_tryingHosts(const StreamHostList &hosts);

};

#endif // JABBER_FILE_TRANSFER_PRIVATE_H
