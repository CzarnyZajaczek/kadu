/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FILE_TRANSFER_MANAGER_H
#define FILE_TRANSFER_MANAGER_H

#include <QtCore/QObject>
#include <QtCore/QUuid>

#include "configuration/storable-object.h"

#include "exports.h"
/*
class FileTransfer;
class XmlConfigFile;

class KADUAPI FileTransferManager : public QObject, public StorableObject
{
	Q_OBJECT
	Q_DISABLE_COPY(FileTransferManager)

	static FileTransferManager *Instance;

	FileTransferManager();
	virtual ~FileTransferManager();

	QList<FileTransfer *> FileTransfers;

protected:
	virtual StoragePoint * createStoragePoint() const;

public:
	static FileTransferManager * instance();

	void loadConfiguration();
	void storeConfiguration();

	void addFileTransfer(FileTransfer *fileTransfer);

};*/

#endif // FILE_TRANSFER_MANAGER_H
