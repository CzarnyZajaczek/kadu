/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_CONTACT_LIST_SERVICE_H
#define GADU_CONTACT_LIST_SERVICE_H

#include "protocols/services/contact-list-service.h"

class GaduProtocol;
class GaduProtocolSocketNotifiers;

class GaduContactListService : public ContactListService
{
	Q_OBJECT

	bool AsFile;

	GaduProtocol *Protocol;
	QString ImportReply;

	friend class GaduProtocolSocketNotifiers;
	void handleEventUserlistGetReply(struct gg_event *e);
	void handleEventUserlistPutReply(struct gg_event *e);
	void handleEventUserlist(struct gg_event *e);

public:
	GaduContactListService(GaduProtocol *protocol);

	virtual void importContactList();
	virtual void importContactListAsFile();
	virtual void exportContactList();
	virtual void exportContactList(BuddyList buddies);

signals:
	void contactListDownloaded(const QString &contactList);

};

#endif // GADU_CONTACT_LIST_SERVICE_H
