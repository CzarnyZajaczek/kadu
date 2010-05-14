/*
 * %kadu copyright begin%
 * Copyright 2009, 2009, 2010 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2009, 2010 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * Copyright 2009, 2009, 2010 Bartłomiej Zimoń (uzi18@o2.pl)
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

#include "buddies/buddy-manager.h"
#include "buddies/group-manager.h"
#include "contacts/contact-manager.h"
#include "core/core.h"
#include "debug.h"
#include "gui/windows/message-dialog.h"
#include "gui/windows/password-window.h"
#include "os/generic/system-info.h"
#include "protocols/protocols-manager.h"
#include "status/status-type-manager.h"
#include "url-handlers/url-handler-manager.h"

#include "gui/windows/subscription-window.h"
#include "resource/jabber-resource-pool.h"
#include "utils/pep-manager.h"
#include "utils/server-info-manager.h"
#include "iris/filetransfer.h"
#include "services/jabber-subscription-service.h"
#include "iris-status-adapter.h"
#include "jabber-account-details.h"
#include "jabber-contact-details.h"
#include "jabber-protocol-factory.h"
#include "jabber-url-handler.h"

#include "jabber-protocol.h"

bool JabberProtocol::ModuleUnloading = false;

int JabberProtocol::initModule()
{
	kdebugf();

	if (ProtocolsManager::instance()->hasProtocolFactory("jabber"))
		return 0;

	ProtocolsManager::instance()->registerProtocolFactory(JabberProtocolFactory::instance());
	UrlHandlerManager::instance()->registerUrlHandler("Jabber", new JabberUrlHandler());

	kdebugf2();
	return 0;
}

void JabberProtocol::closeModule()
{
	kdebugf();

	ModuleUnloading = true;

	UrlHandlerManager::instance()->unregisterUrlHandler("Jabber");
	ProtocolsManager::instance()->unregisterProtocolFactory(JabberProtocolFactory::instance());
	kdebugf2();
}

JabberProtocol::JabberProtocol(Account account, ProtocolFactory *factory) :
		Protocol(account, factory), JabberClient(0), ResourcePool(0), serverInfoManager(0), PepManager(0)
{
	kdebugf();

	initializeJabberClient();

	CurrentAvatarService = new JabberAvatarService(account, this);
	CurrentChatService = new JabberChatService(this);
	CurrentChatStateService = new JabberChatStateService(this);
	CurrentFileTransferService = new JabberFileTransferService(this);
	CurrentSubscriptionService = new JabberSubscriptionService(this);

	connectContactManagerSignals();
		
	kdebugf2();
}

JabberProtocol::~JabberProtocol()
{
	disconnectContactManagerSignals();
	logout();
	
	delete JabberClient;
	JabberClient = 0;
	
	delete serverInfoManager;
	serverInfoManager = 0;
	
	delete PepManager;
	PepManager = 0;
	pepAvailable = false;
}

void JabberProtocol::connectContactManagerSignals()
{
	connect(ContactManager::instance(), SIGNAL(contactDetached(Contact)),
			this, SLOT(contactDetached(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactAttached(Contact)),
			this, SLOT(contactAttached(Contact)));
	connect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));
	connect(ContactManager::instance(), SIGNAL(contactReattached(Contact)),
			this, SLOT(contactUpdated(Contact)));
	
	connect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(buddyUpdated(Buddy &))); 
}

void JabberProtocol::disconnectContactManagerSignals()
{
	disconnect(ContactManager::instance(), SIGNAL(contactDetached(Contact)),
			this, SLOT(contactDetached(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactAttached(Contact)),
			this, SLOT(contactAttached(Contact)));
	disconnect(ContactManager::instance(), SIGNAL(contactIdChanged(Contact, const QString &)),
			this, SLOT(contactIdChanged(Contact, const QString &)));
	disconnect(ContactManager::instance(), SIGNAL(contactReattached(Contact)),
			this, SLOT(contactUpdated(Contact)));
	
	disconnect(BuddyManager::instance(), SIGNAL(buddyUpdated(Buddy &)),
			this, SLOT(buddyUpdated(Buddy &))); 
}

void JabberProtocol::initializeJabberClient()
{
	JabberClient = new XMPP::JabberClient(this);
	connect(JabberClient, SIGNAL(csDisconnected()), this, SLOT(disconnectedFromServer()));
	connect(JabberClient, SIGNAL(connected()), this, SLOT(connectedToServer()));

	connect(JabberClient, SIGNAL(newContact(const XMPP::RosterItem &)),
		   this, SLOT(slotContactUpdated(const XMPP::RosterItem &)));
	connect(JabberClient, SIGNAL(contactUpdated(const XMPP::RosterItem &)),
		   this, SLOT(slotContactUpdated(const XMPP::RosterItem &)));
	connect(JabberClient, SIGNAL(contactDeleted(const XMPP::RosterItem &)),
		   this, SLOT(slotContactDeleted(const XMPP::RosterItem &)));
	connect(JabberClient, SIGNAL(rosterRequestFinished(bool)),
		   this, SLOT(rosterRequestFinished(bool)));

	connect(JabberClient, SIGNAL(resourceAvailable(const XMPP::Jid &, const XMPP::Resource &)),
		   this, SLOT(clientResourceReceived(const XMPP::Jid &, const XMPP::Resource &)));
	connect(JabberClient, SIGNAL(resourceUnavailable(const XMPP::Jid &, const XMPP::Resource &)),
		   this, SLOT(clientResourceReceived(const XMPP::Jid &, const XMPP::Resource &)));

	connect(JabberClient, SIGNAL(incomingFileTransfer()), this, SLOT(slotIncomingFileTransfer()));

		/*//TODO: implement in the future
		connect( JabberClient, SIGNAL ( groupChatJoined ( const XMPP::Jid & ) ),
				   this, SLOT ( slotGroupChatJoined ( const XMPP::Jid & ) ) );
		connect( JabberClient, SIGNAL ( groupChatLeft ( const XMPP::Jid & ) ),
				   this, SLOT ( slotGroupChatLeft ( const XMPP::Jid & ) ) );
		connect( JabberClient, SIGNAL ( groupChatPresence ( const XMPP::Jid &, const XMPP::Status & ) ),
				   this, SLOT ( slotGroupChatPresence ( const XMPP::Jid &, const XMPP::Status & ) ) );
		connect( JabberClient, SIGNAL ( groupChatError ( const XMPP::Jid &, int, const QString & ) ),
				   this, SLOT ( slotGroupChatError ( const XMPP::Jid &, int, const QString & ) ) );
		*/
	connect(JabberClient, SIGNAL( debugMessage(const QString &)),
		   this, SLOT(slotClientDebugMessage(const QString &)));
}

void JabberProtocol::login(const QString &password, bool permanent)
{
	account().setPassword(password);
	account().setRememberPassword(permanent);
	account().setHasPassword(!password.isEmpty());

	connectToServer();
}

void JabberProtocol::connectToServer()
{
	kdebugf();

	JabberAccountDetails *jabberAccountDetails = dynamic_cast<JabberAccountDetails *>(account().details());
	if (!jabberAccountDetails)
		return;

	if (account().id().isEmpty())
	{
		MessageDialog::msg(tr("Jabber ID not set!"), false, "32x32/dialog-warning.png");
		setStatus(Status());
		kdebugmf(KDEBUG_FUNCTION_END, "end: Jabber ID not set\n");
		return;
	}

	if (!account().hasPassword())
	{
		QString message = tr("Please provide password for %1 (%2) account")
				.arg(account().accountIdentity().name())
				.arg(account().id());
		PasswordWindow::getPassword(message, this, SLOT(login(const QString &, bool)));
		return;
	}

	JabberClient->disconnect();

	JabberClient->setOSName(SystemInfo::instance()->osFullName());
	JabberClient->setTimeZone(SystemInfo::instance()->timezone(), SystemInfo::instance()->timezoneOffset());
	JabberClient->setClientName("Kadu");
	JabberClient->setClientVersion(Core::instance()->version());

	// Set caps node information
	JabberClient->setCapsNode("http://psi-im.org/caps");
	JabberClient->setCapsVersion("0.12");

	JabberClient->setForceTLS(jabberAccountDetails->encryptionMode() != JabberAccountDetails::Encryption_No);

	// override server and port (this should be dropped when using the new protocol and no direct SSL)
	JabberClient->setUseSSL(jabberAccountDetails->encryptionMode() == JabberAccountDetails::Encryption_Legacy);
	JabberClient->setOverrideHost(jabberAccountDetails->useCustomHostPort(), jabberAccountDetails->customHost(), jabberAccountDetails->customPort());

	JabberClient->setFileTransfersEnabled(true); // i haz it
	rosterRequestDone = false;
	jabberID = account().id();

/*
//TODO: do nowej klasy dostosowa?
	XMPP::AdvancedConnector::Proxy p;
	if(config_file.readBoolEntry("Network", "UseProxy"))
	{
		p.setHttpConnect(config_file.readEntry("Network", "ProxyHost"), config_file.readNumEntry("Network", "ProxyPort"));
		if (!config_file.readEntry("Network", "ProxyUser").isEmpty())
			p.setUserPass(config_file.readEntry("Network", "ProxyUser"), config_file.readEntry("Network", "ProxyPassword"));
	}
	connector = new XMPP::AdvancedConnector;
	if(confUseSSL && QCA::isSupported("tls"))
	{
		tls = new QCA::TLS;
		tls->setTrustedCertificates(CertUtil::allCertificates());
		tlsHandler = new XMPP::QCATLSHandler(tls);
		tlsHandler->setXMPPCertCheck(true);
		connect(tlsHandler, SIGNAL(tlsHandshaken()), SLOT(tlsHandshaken()));
	}
	connector->setProxy(p);
	connector->setOptHostPort(host, port);
	connector->setOptSSL(confUseSSL);

	stream = new XMPP::ClientStream(connector, tlsHandler);
	stream->setAllowPlain(XMPP::ClientStream::AllowPlainOverTLS);

*/

	networkStateChanged(NetworkConnecting);
	jabberID = jabberID.withResource(jabberAccountDetails->resource());
	JabberClient->connect(jabberID, account().password(), true);
	
	// Initialize server info stuff
	serverInfoManager = new ServerInfoManager(JabberClient->client());
	connect(serverInfoManager, SIGNAL(featuresChanged()),
		this, SLOT(serverFeaturesChanged()));

	// Initialize PubSub stuff
	PepManager = new PEPManager(JabberClient->client(), serverInfoManager);
	connect(PepManager, SIGNAL(itemPublished(const XMPP::Jid&, const QString&, const XMPP::PubSubItem&)),
		this, SLOT(itemPublished(const XMPP::Jid&, const QString&, const XMPP::PubSubItem&)));
	connect(PepManager, SIGNAL(itemRetracted(const XMPP::Jid&, const QString&, const XMPP::PubSubRetraction&)),
		this, SLOT(itemRetracted(const XMPP::Jid&, const QString&, const XMPP::PubSubRetraction&)));
	pepAvailable = false;
	
	kdebugf2();
}

void JabberProtocol::connectedToServer()
{
	kdebugf();

	networkStateChanged(NetworkConnected);
	
	// flag roster for delete
	ContactsForDelete = ContactManager::instance()->contacts(account());
	ContactsForDelete.removeAll(account().accountContact());

	// ask for roster
	JabberClient->requestRoster();
	kdebugf2();
}

// disconnect or stop reconnecting
void JabberProtocol::logout()
{
	kdebugf();

	Status newstat = status();
	if (!status().isDisconnected())
	{
		newstat.setType("Offline");
		setStatus(newstat);
	}

	disconnectFromServer(IrisStatusAdapter::toIrisStatus(newstat));
	setAllOffline();

	kdebugf2();
}

void JabberProtocol::disconnectFromServer(const XMPP::Status &s)
{
	kdebugf();

	if (isConnected())
	{
		kdebug("Still connected, closing connection...\n");
		// make sure that the connection animation gets stopped if we're still
		// in the process of connecting
		JabberClient->setPresence(s);

		/* Tell backend class to disconnect. */
		JabberClient->disconnect();
	}

	/* FIXME:
	 * We should delete the JabberClient instance here,
	 * but active timers in Iris prevent us from doing so.
	 * (in a failed connection attempt, these timers will
	 * try to access an already deleted object).
	 * Instead, the instance will lurk until the next
	 * connection attempt.
	 */
	kdebug("Disconnected.\n");

	if (serverInfoManager)
	{
		disconnect(serverInfoManager, SIGNAL(featuresChanged()),
			this, SLOT(serverFeaturesChanged()));
	}
	
	delete serverInfoManager;
	serverInfoManager = 0;

	if (!ModuleUnloading && PepManager)
	{
		delete PepManager;
		PepManager = 0;
		pepAvailable = false;
	}
	
	networkStateChanged(NetworkDisconnected);
	kdebugf2();
}

void JabberProtocol::slotClientDebugMessage(const QString &msg)
{
	kdebugm(KDEBUG_WARNING, "Jabber Client debug:  %s\n", qPrintable(msg));
}

void JabberProtocol::rosterRequestFinished(bool success)
{
	kdebugf();
	if (success)
	{
		// the roster was imported successfully, clear
		// all "dirty" items from the contact list
		foreach (Contact contact, ContactsForDelete)
		{
			Buddy owner = contact.ownerBuddy();
			contact.setOwnerBuddy(Buddy::null);
			if (owner.contacts().size() == 0)
				BuddyManager::instance()->removeItem(owner);	
		}
		
	}
	rosterRequestDone = true;

	/* Since we are online now, set initial presence. Don't do this
	* before the roster request or we will receive presence
	* information before we have updated our roster with actual
	* contacts from the server! (Iris won't forward presence
	* information in that case either). */
	kdebug("Setting initial presence...\n");

	changeStatus(nextStatus());

	kdebugf2();
}

void JabberProtocol::disconnectedFromServer()
{
	kdebugf();

	setAllOffline();

	networkStateChanged(NetworkDisconnected);
	
	if (!nextStatus().isDisconnected()) // user still wants to login
		QTimer::singleShot(1000, this, SLOT(login())); // try again after one second
	else if (!nextStatus().isDisconnected())
		setStatus(Status());
	
	kdebugf2();
}

void JabberProtocol::login()
{
	if (isConnected())
		return;
	connectToServer();
}

void JabberProtocol::changeStatus(Status status)
{
	XMPP::Status xmppStatus = IrisStatusAdapter::toIrisStatus(status);
	JabberClient->setPresence(xmppStatus);

	if (status.isDisconnected())
	{
		networkStateChanged(NetworkDisconnected);

		setAllOffline();

		if (!nextStatus().isDisconnected())
			setStatus(Status());
	}

	statusChanged(IrisStatusAdapter::fromIrisStatus(xmppStatus));
}

void JabberProtocol::slotIncomingFileTransfer()
{
	XMPP::FileTransfer *jTransfer = client()->fileTransferManager()->takeIncoming();
	if (!jTransfer)
		return;

	Contact peer = ContactManager::instance()->byId(account(), jTransfer->peer().bare(), ActionCreateAndAdd);
	FileTransfer transfer = FileTransfer::create();
	transfer.setPeer(peer);
	transfer.setTransferType(TypeReceive);
	transfer.setRemoteFileName(jTransfer->fileName());

	transfer.createHandler();

	JabberFileTransferHandler *handler = dynamic_cast<JabberFileTransferHandler *>(transfer.handler());
	if (handler)
		handler->setJTransfer(jTransfer);

	CurrentFileTransferService->incomingFile(transfer);
}

void JabberProtocol::clientResourceReceived(const XMPP::Jid &jid, const XMPP::Resource &resource)
{
	kdebugf();
	kdebug("New resource available for %s\n", jid.full().toLocal8Bit().data());
	resourcePool()->addResource(jid, resource);

	Status status(IrisStatusAdapter::fromIrisStatus(resource.status()));
	Contact contact = ContactManager::instance()->byId(account(), jid.bare(), ActionReturnNull);

	if (contact)
	{
		Status oldStatus = contact.currentStatus();
		contact.setCurrentStatus(status);

		emit contactStatusChanged(contact, oldStatus);
	}
	kdebugf2();
}

void JabberProtocol::addContactToRoster(Contact contact, bool requestAuth)
{	
	if (!isConnected() || contact.contactAccount() != account() || contact.ownerBuddy().isAnonymous())
		return;

	Buddy buddy = contact.ownerBuddy();
	QStringList groupsList;

	foreach (Group group, buddy.groups())
		groupsList.append(group.name());
	
	//TODO last parameter: automagic authorization request - make it configurable
	JabberClient->addContact(contact.id(), buddy.display(), groupsList, requestAuth);
}

void JabberProtocol::contactAttached(Contact contact)
{
	return addContactToRoster(contact, true);
}

void JabberProtocol::contactDetached(Contact contact)
{
	if (!isConnected() || contact.contactAccount() != account())
		return;

	JabberClient->removeContact(contact.id());
}

void JabberProtocol::buddyUpdated(Buddy &buddy)
{
	if (!isConnected())
		return;

	QList<Contact> contacts = buddy.contacts(account());
	if (contacts.isEmpty() || buddy.isAnonymous())
		return;

	QStringList groupsList;
	foreach (Group group, buddy.groups())
		groupsList.append(group.name());

	foreach (const Contact &contact, contacts)
		JabberClient->updateContact(contact.id(), buddy.display(), groupsList);
}

void JabberProtocol::contactUpdated(Contact contact)
{
	if (!isConnected() || contact.contactAccount() != account() || contact.ownerBuddy().isAnonymous())
		return;

	Buddy buddy = contact.ownerBuddy();
	if (buddy.isAnonymous())
		return;

	QStringList groupsList;
	foreach (Group group, buddy.groups())
		groupsList.append(group.name());
	
	JabberClient->updateContact(contact.id(), buddy.display(), groupsList);
}

void JabberProtocol::contactIdChanged(Contact contact, const QString &oldId)
{
  	if (!isConnected() || contact.contactAccount() != account())
		return;
	
	JabberClient->removeContact(oldId);
	contactAttached(contact);
}

void JabberProtocol::slotContactUpdated(const XMPP::RosterItem &item)
{
	kdebugf();
	/**
	 * Subscription types are: Both, From, To, Remove, None.
	 * Both:   Both sides have authed each other, each side
	 *         can see each other's presence
	 * From:   The other side can see us.
	 * To:     We can see the other side. (implies we are
	 *         authed)
	 * Remove: Other side revoked our subscription request.
	 *         Not to be handled here.
	 * None:   No subscription.
	 *
	 * Regardless of the subscription type, we have to add
	 * a roster item here.
	 */
	
	if (item.subscription().toString() == "none")
		return;

	disconnectContactManagerSignals();

	kdebug("New roster item: %s (Subscription: %s )\n", item.jid().full().toLocal8Bit().data(), item.subscription().toString().toLocal8Bit().data());

	Contact contact = ContactManager::instance()->byId(account(), item.jid().bare(), ActionReturnNull);
	if (contact)
	{
		ContactsForDelete.removeAll(contact);
	}
	else
		Contact contact = ContactManager::instance()->byId(account(), item.jid().bare(), ActionCreateAndAdd);
	
	JabberContactDetails *data = jabberContactDetails(contact);	
	data->setSubscription(item.subscription());


	Buddy buddy = BuddyManager::instance()->byContact(contact, ActionCreateAndAdd);

	// if contact has name set it to display
	if (!item.name().isNull())
	{
		if (item.name() != buddy.display())
			buddy.setDisplay(item.name());
	}
	else
		buddy.setDisplay(item.jid().bare());

	if (buddy.isAnonymous()) // always false!!
	{
		// TODO: add some logic here?
		buddy.setAnonymous(false);

		GroupManager *gm = GroupManager::instance();
		// add this contact to all groups the contact is a member of
		foreach (QString group, item.groups())
			buddy.addToGroup(gm->byName(group,true /* create group */));
	}
	else
	{
		//TODO: synchronize groups
	}
	
	connectContactManagerSignals();

	kdebugf2();
}

void JabberProtocol::slotContactDeleted(const XMPP::RosterItem &item)
{
	kdebug("Deleting contact %s", item.jid().bare().toLocal8Bit().data());
	
	Contact contact = ContactManager::instance()->byId(account(), item.jid().bare(), ActionReturnNull);
	if (contact)
	{
	  	Buddy owner = contact.ownerBuddy();
		contact.setOwnerBuddy(Buddy::null);
		if (owner.contacts().size() == 0)
			BuddyManager::instance()->removeItem(owner);
		
	}
}

void JabberProtocol::authorizeContact(Contact contact, bool authorized)
{
	const XMPP::Jid jid = XMPP::Jid(contact.id());
	
	if (authorized)
	{
		addContactToRoster(contact, false);
		JabberClient->resendSubscription(jid);
	}
	else
		JabberClient->rejectSubscription(jid);
}

bool JabberProtocol::validateUserID(const QString& uid)
{
	XMPP::Jid j = XMPP::Jid(uid);
	return j.isValid();
}

JabberResourcePool *JabberProtocol::resourcePool()
{
	if (!ResourcePool)
		ResourcePool = new JabberResourcePool(this);
	return ResourcePool;
}

void JabberProtocol::changeStatus()
{
	Status newStatus = nextStatus();

	if (newStatus.isDisconnected() && status().isDisconnected())
	{
		networkStateChanged(NetworkDisconnected);

		setAllOffline();

		if (!nextStatus().isDisconnected())
			setStatus(Status());
		return;
	}

	if (isConnecting())
		return;

	if (status().isDisconnected())
	{
		login();
		return;
	}

	changeStatus(newStatus);
}

void JabberProtocol::changePrivateMode()
{
	changeStatus();
}

QPixmap JabberProtocol::statusPixmap(Status status)
{
	return StatusTypeManager::instance()->statusPixmap("xmpp", "16x16", status.type(),
			!status.description().isEmpty(), false);
}

QPixmap JabberProtocol::statusPixmap(const QString &statusType)
{
	return StatusTypeManager::instance()->statusPixmap("xmpp", "16x16", statusType, false, false);
}

QIcon JabberProtocol::statusIcon(Status status)
{
	return StatusTypeManager::instance()->statusIcon("xmpp", status.type(),
			!status.description().isEmpty(), false);
}

QIcon JabberProtocol::statusIcon(const QString &statusType)
{
	return StatusTypeManager::instance()->statusIcon("xmpp", statusType, false, false);
}

JabberContactDetails * JabberProtocol::jabberContactDetails(Contact contact) const
{
	if (contact.isNull())
		return 0;
	return dynamic_cast<JabberContactDetails *>(contact.details());
}

void JabberProtocol::serverFeaturesChanged()
{
	setPEPAvailable(serverInfoManager->hasPEP());
}

void JabberProtocol::setPEPAvailable(bool b)
{
	if (pepAvailable == b)
		return;

	pepAvailable = b;

	// Publish support
	if (b && JabberClient->client()->extensions().contains("ep"))
	{
		QStringList pepNodes;
		/*pepNodes += "http://jabber.org/protocol/mood";
		pepNodes += "http://jabber.org/protocol/tune";
		pepNodes += "http://jabber.org/protocol/physloc";
		pepNodes += "http://jabber.org/protocol/geoloc";*/
		pepNodes += "http://www.xmpp.org/extensions/xep-0084.html#ns-data";
		pepNodes += "http://www.xmpp.org/extensions/xep-0084.html#ns-metadata";
		JabberClient->client()->addExtension("ep", XMPP::Features(pepNodes));
		//setStatusActual(d->loginStatus);
	}
	else if (!b && JabberClient->client()->extensions().contains("ep"))
	{
		JabberClient->client()->removeExtension("ep");
		//setStatusActual(d->loginStatus);
	}

	// Publish current tune information
// 	if (b && d->psi->tuneController() && d->options->getOption("options.extended-presence.tune.publish").toBool()) {
// 		Tune current = d->psi->tuneController()->currentTune();
// 		if (!current.isNull())
// 			publishTune(current);
// 	}
}

void JabberProtocol::itemPublished(const XMPP::Jid& j, const QString& n, const XMPP::PubSubItem& item)
{
	/*
	// User Tune
	if (n == "http://jabber.org/protocol/tune") {
		// Parse tune
		QDomElement element = item.payload();
		QDomElement e;
		QString tune;
		bool found;

		e = findSubTag(element, "artist", &found);
		if (found)
			tune += e.text() + " - ";

		e = findSubTag(element, "title", &found);
		if (found)
			tune += e.text();

		foreach(UserListItem* u, findRelevant(j)) {
			// FIXME: try to find the right resource using JEP-33 'replyto'
			//UserResourceList::Iterator rit = u->userResourceList().find(<resource>);
			//bool found = (rit == u->userResourceList().end()) ? false: true;
			//if(found)
			//	(*rit).setTune(tune);
			u->setTune(tune);
			cpUpdate(*u);
		}
	}
	else if (n == "http://jabber.org/protocol/mood") {
		Mood mood(item.payload());
		foreach(UserListItem* u, findRelevant(j)) {
			u->setMood(mood);
			cpUpdate(*u);
		}
	}
	else if (n == "http://jabber.org/protocol/geoloc") {
		// FIXME: try to find the right resource using JEP-33 'replyto'
		// see tune case above
		GeoLocation geoloc(item.payload());
		foreach(UserListItem* u, findRelevant(j)) {
			u->setGeoLocation(geoloc);
			cpUpdate(*u);
		}
	}
	else if (n == "http://jabber.org/protocol/physloc") {
		// FIXME: try to find the right resource using JEP-33 'replyto'
		// see tune case above
		PhysicalLocation physloc(item.payload());
		foreach(UserListItem* u, findRelevant(j)) {
			u->setPhysicalLocation(physloc);
			cpUpdate(*u);
		}
	}
	*/
}

void JabberProtocol::itemRetracted(const XMPP::Jid& j, const QString& n, const XMPP::PubSubRetraction& item)
{
	Q_UNUSED(item);
	// User Tune
	/*if (n == "http://jabber.org/protocol/tune") {
		// Parse tune
		foreach(UserListItem* u, findRelevant(j)) {
			// FIXME: try to find the right resource using JEP-33 'replyto'
			//UserResourceList::Iterator rit = u->userResourceList().find(<resource>);
			//bool found = (rit == u->userResourceList().end()) ? false: true;
			//if(found) 
			//	(*rit).setTune(tune);
			u->setTune(QString());
			cpUpdate(*u);
		}
	}
	else if (n == "http://jabber.org/protocol/mood") {
		foreach(UserListItem* u, findRelevant(j)) {
			u->setMood(Mood());
			cpUpdate(*u);
		}
	}
	else if (n == "http://jabber.org/protocol/geoloc") {
		// FIXME: try to find the right resource using JEP-33 'replyto'
		// see tune case above
		foreach(UserListItem* u, findRelevant(j)) {
			u->setGeoLocation(GeoLocation());
			cpUpdate(*u);
		}
	}
	else if (n == "http://jabber.org/protocol/physloc") {
		// FIXME: try to find the right resource using JEP-33 'replyto'
		// see tune case above
		foreach(UserListItem* u, findRelevant(j)) {
			u->setPhysicalLocation(PhysicalLocation());
			cpUpdate(*u);
		}
	}
	*/
}


