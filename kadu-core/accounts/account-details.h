/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef ACCOUNT_DETAILS_H
#define ACCOUNT_DETAILS_H

#include <QtCore/QObject>

#include "accounts/account.h"
#include "configuration/storable-object.h"

#include "exports.h"

class AccountShared;

class KADUAPI AccountDetails : public QObject, public StorableObject
{
	Q_OBJECT

	AccountShared *MyAccountData;

public:
	explicit AccountDetails(StoragePoint *storagePoint, Account parent);
	virtual ~AccountDetails();

	Account account() { return Account(MyAccountData); }

};

#endif // ACCOUNT_DETAILS_H
