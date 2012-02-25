/*
 * %kadu copyright begin%
 * Copyright 2008, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2008 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2007, 2008, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2007, 2008 Dawid Stawiarski (neeo@kadu.net)
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

#ifndef ENCRYPTION_CHAT_DATA_H
#define ENCRYPTION_CHAT_DATA_H

#include <QtCore/QObject>
#include <QtGui/QAction>

#include "chat/chat.h"

class Decryptor;
class Encryptor;

class EncryptionChatData : public QObject
{
	Q_OBJECT

public:
	enum EncryptState {
		EncryptStateDefault,
		EncryptStateEnabled,
		EncryptStateDisabled,
	};

private:
	Chat MyChat;
	Encryptor *ChatEncryptor;
	Decryptor *ChatDecryptor;

	EncryptState Encrypt;

	EncryptState importEncrypt();

private slots:
	void encryptorDestroyed();
	void decryptorDestroyed();

public:
	explicit EncryptionChatData(const Chat &chat, QObject *parent);
	virtual ~EncryptionChatData();

	void setEncrypt(EncryptState encrypt);
	EncryptState encrypt();

	void setEncryptor(Encryptor *encryptor);
	Encryptor * encryptor();

	void setDecryptor(Decryptor *decryptor);
	Decryptor * decryptor();

};

#endif // ENCRYPTION_CHAT_DATA_H
