/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef ENCRYPTION_NG_OTR_PLUGIN_H
#define ENCRYPTION_NG_OTR_PLUGIN_H

#include <QtCore/QObject>

#include "plugins/generic-plugin.h"

class EncryptionNgOtrAppOpsWrapper;
class EncryptionNgOtrPrivateKeyService;
class EncryptionNgOtrRawMessageTransformer;
class EncryptionNgOtrUserStateService;

class EncryptionNgOtrPlugin : public QObject, public GenericPlugin
{
	Q_OBJECT
	Q_INTERFACES(GenericPlugin)

	QScopedPointer<EncryptionNgOtrAppOpsWrapper> OtrAppOpsWrapper;
	QScopedPointer<EncryptionNgOtrPrivateKeyService> OtrPrivateKeyService;
	QScopedPointer<EncryptionNgOtrUserStateService> OtrUserStateService;
	QScopedPointer<EncryptionNgOtrRawMessageTransformer> OtrRawMessageTransformer;

	void registerOtrAppOpsWrapper();
	void unregisterOtrAppOpsWrapper();

	void registerOtrPrivateKeyService();
	void unregisterOtrPrivateKeyService();

	void registerOtrUserStateService();
	void unregisterOtrUserStateService();

	void registerOtrRawMessageTransformer();
	void unregisterOtrRawMessageTransformer();

public:
	explicit EncryptionNgOtrPlugin();
	virtual ~EncryptionNgOtrPlugin();

	virtual int init(bool firstLoad);
	virtual void done();

};

#endif // ENCRYPTION_NG_OTR_PLUGIN_H
