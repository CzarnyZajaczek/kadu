/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QIntValidator>

#ifdef Q_WS_WIN
#include <winsock2.h>
#undef MessageBox
#else
#include <netinet/in.h>
#endif

#include <time.h>

#include "accounts/account.h"
#include "accounts/account_manager.h"

#include "contacts/contact-manager.h"
#include "contacts/ignored-helper.h"

#include "protocols/protocols_manager.h"
#include "protocols/status.h"

#include "config_file.h"
#include "debug.h"
#include "icons_manager.h"
#include "kadu.h"
#include "message.h"
#include "message_box.h"
#include "misc.h"

#include "gadu_account_data.h"
#include "gadu-contact-account-data.h"
#include "gadu_formatter.h"
#include "gadu_images_manager.h"
#include "gadu-importer.h"
#include "gadu_protocol_factory.h"
#include "gadu-protocol-socket-notifiers.h"
#include "gadu-pubdir-socket-notifiers.h"
#include "gadu-token-socket-notifiers.h"
#include "gadu_search.h"
#include "gadu_status.h"
#include "uins_list.h"
#include "xml_config_file.h"

#include "gadu.h"

extern "C" int gadu_protocol_init(bool firstLoad)
{
	if (ProtocolsManager::instance()->hasProtocolFactory("gadu"))
		return 0;

	ProtocolsManager::instance()->registerProtocolFactory("gadu", new GaduProtocolFactory());

	if (!xml_config_file->hasNode("Accounts"))
		GaduImporter::instance()->importAccounts();

	GaduImporter::instance()->importContacts();

	return 0;
}

extern "C" void gadu_protocol_close()
{
	ProtocolsManager::instance()->unregisterProtocolFactory("gadu");
}


static QList<QHostAddress> gg_servers;

#define GG_SERVERS_COUNT 22
static const char *gg_servers_ip[GG_SERVERS_COUNT] = {
	"91.197.13.2",
	"91.197.12.4",
	"91.197.12.5",
	"91.197.12.6",
	"91.197.13.11",
	"91.197.13.12",
	"91.197.13.13",
	"91.197.13.14",
	"91.197.13.17",
	"91.197.13.18",
	"91.197.13.19",
	"91.197.13.20",
	"91.197.13.21",
	"91.197.13.24",
	"91.197.13.25",
	"91.197.13.26",
	"91.197.13.27",
	"91.197.13.28",
	"91.197.13.29",
	"91.197.13.31",
	"91.197.13.32",
	"91.197.13.33"
};

// ------------------------------------
//              Timers
// ------------------------------------

class ConnectionTimeoutTimer : public QTimer
{
	ConnectionTimeoutTimer(QObject *parent = 0);
	static ConnectionTimeoutTimer *connectiontimeout_object;

public:
	static void on();
	static void off();
	static bool connectTimeoutRoutine(const QObject *receiver, const char *member);

};

ConnectionTimeoutTimer *ConnectionTimeoutTimer::connectiontimeout_object = 0;

// ------------------------------------
//        Timers - implementation
// ------------------------------------

ConnectionTimeoutTimer::ConnectionTimeoutTimer(QObject *parent)
	: QTimer(parent)
{
	start(config_file.readUnsignedNumEntry("Network", "TimeoutInMs"), true);
}

bool ConnectionTimeoutTimer::connectTimeoutRoutine(const QObject *receiver, const char *member)
{
	return connect(connectiontimeout_object, SIGNAL(timeout()), receiver, member);
}

void ConnectionTimeoutTimer::on()
{
	if (!connectiontimeout_object)
		connectiontimeout_object = new ConnectionTimeoutTimer(kadu);
}

void ConnectionTimeoutTimer::off()
{
	if (connectiontimeout_object)
	{
		connectiontimeout_object->stop();
		connectiontimeout_object->deleteLater();
		connectiontimeout_object = 0;
	}
}

SearchResult::SearchResult() :
	Uin(), First(), Last(), Nick(), Born(), City(),
	FamilyName(), FamilyCity(), Gender(0), Stat()
{
}

SearchResult::SearchResult(const SearchResult &copyFrom) :
	Uin(copyFrom.Uin),
	First(copyFrom.First),
	Last(copyFrom.Last),
	Nick(copyFrom.Nick),
	Born(copyFrom.Born),
	City(copyFrom.City),
	FamilyName(copyFrom.FamilyName),
	FamilyCity(copyFrom.FamilyCity),
	Gender(copyFrom.Gender),
	Stat(copyFrom.Stat)
{
}

void SearchResult::setData(const char *uin, const char *first, const char *last, const char *nick, const char *born,
	const char *city, const char *familyName, const char *familyCity, const char *gender, const char *status)
{
	kdebugf();
	Uin = cp2unicode(uin);
	First = cp2unicode(first);
	Last = cp2unicode(last);
	Nick = cp2unicode(nick);
	Born = cp2unicode(born);
	City = cp2unicode(city);
	FamilyName = cp2unicode(familyName);
	FamilyCity = cp2unicode(familyCity);
	if (status)
		Stat.fromStatusNumber(atoi(status) & 127, QString::null);
	if (gender)
		Gender = atoi(gender);
	else
		Gender = 0;
	kdebugf2();
}

SearchRecord::SearchRecord() :
	Seq(0), FromUin(0), Uin(), FirstName(), LastName(), NickName(), City(),
	BirthYearFrom(), BirthYearTo(), Gender(0), Active(false), IgnoreResults(false)
{
	kdebugf();
	kdebugf2();
}

SearchRecord::~SearchRecord()
{
}

void SearchRecord::reqUin(const QString &uin)
{
	Uin = uin;
}

void SearchRecord::reqFirstName(const QString &firstName)
{
	FirstName = firstName;
}

void SearchRecord::reqLastName(const QString &lastName)
{
	LastName = lastName;
}

void SearchRecord::reqNickName(const QString &nickName)
{
	NickName = nickName;
}

void SearchRecord::reqCity(const QString &city)
{
	City = city;
}

void SearchRecord::reqBirthYear(const QString &birthYearFrom, const QString &birthYearTo)
{
	BirthYearFrom = birthYearFrom;
	BirthYearTo = birthYearTo;
}

void SearchRecord::reqGender(bool female)
{
	Gender = (female ? 2 : 1);
}

void SearchRecord::reqActive()
{
	Active = true;
}

void SearchRecord::clearData()
{
	kdebugf();
	FromUin = 0;
	Uin.truncate(0);
	FirstName.truncate(0);
	LastName.truncate(0);
	NickName.truncate(0);
	City.truncate(0);
	BirthYearFrom.truncate(0);
	BirthYearTo.truncate(0);
	Gender = 0;
	Active = false;
	IgnoreResults = false;
	kdebugf2();
}

/* GaduProtocol */

QList<QHostAddress> GaduProtocol::ConfigServers;

static inline int getRand(int min, int max)
{
	int i = int((double(rand()) / RAND_MAX) * (max - min)) + min;
	if (i < min)
		i = min;
	if (i > max)
		i = max;
	return i;
}

void GaduProtocol::initModule()
{
	kdebugf();

	QHostAddress ip;
	for (int i = 0; i < GG_SERVERS_COUNT; ++i)
	{
		ip.setAddress(QString(gg_servers_ip[i]));
		gg_servers.append(ip);
	}
	srand(time(NULL));
	for (int i = 0; i < GG_SERVERS_COUNT * 2; ++i)
	{
		int idx1 = getRand(0, GG_SERVERS_COUNT - 1);
		int idx2 = getRand(0, GG_SERVERS_COUNT - 1);

		if (idx1 != idx2)
		{
			QHostAddress a = gg_servers[idx1];
			gg_servers[idx1] = gg_servers[idx2];
			gg_servers[idx2] = a;
		}
	}

	gg_proxy_host = NULL;
	gg_proxy_username = NULL;
	gg_proxy_password = NULL;

#ifndef DEBUG_ENABLED
	gg_debug_level = 1;
#endif

	defaultdescriptions = QStringList::split("<-->", config_file.readEntry("General","DefaultDescription", tr("I am busy.")), true);

	QStringList servers;
	QHostAddress ip2;
	servers = QStringList::split(";", config_file.readEntry("Network", "Server"));
	ConfigServers.clear();
	foreach(const QString &server, servers)
		if (ip2.setAddress(server))
			ConfigServers.append(ip2);

	kdebugf2();
}

GaduProtocol::GaduProtocol(Account *account, ProtocolFactory *factory)
	: Protocol(account, factory),
		GaduData(0),
		Mode(Register), DataUin(0), DataEmail(), DataPassword(), DataNewPassword(), TokenId(), TokenValue(),
		ServerNr(0), ActiveServer(), LoginParams(), Sess(0), sendImageRequests(0), seqNumber(0), whileConnecting(false),
		DccExternalIP(), SocketNotifiers(new GaduProtocolSocketNotifiers(account, this)), PingTimer(0),
		SendUserListTimer(new QTimer(this, "SendUserListTimer")), UserListClear(false), ImportReply()
{
	kdebugf();

	CurrentStatus = new GaduStatus();
	NextStatus = new GaduStatus();

	connect(NextStatus, SIGNAL(goOnline(const QString &)), this, SLOT(iWantGoOnline(const QString &)));
	connect(NextStatus, SIGNAL(goBusy(const QString &)), this, SLOT(iWantGoBusy(const QString &)));
	connect(NextStatus, SIGNAL(goInvisible(const QString &)), this, SLOT(iWantGoInvisible(const QString &)));
	connect(NextStatus, SIGNAL(goOffline(const QString &)), this, SLOT(iWantGoOffline(const QString &)));

	connect(SocketNotifiers, SIGNAL(ackReceived(int, uin_t, int)), this, SLOT(ackReceived(int, uin_t, int)));
	connect(SocketNotifiers, SIGNAL(connected()), this, SLOT(connectedSlot()));
	connect(SocketNotifiers, SIGNAL(dccConnectionReceived(Contact)),
		this, SIGNAL(dccConnectionReceived(Contact)));
	connect(SocketNotifiers, SIGNAL(serverDisconnected()), this, SLOT(socketDisconnectedSlot()));
	connect(SocketNotifiers, SIGNAL(error(GaduError)), this, SLOT(errorSlot(GaduError)));
	connect(SocketNotifiers, SIGNAL(imageReceived(UinType, uint32_t, uint32_t, const QString &, const char *)),
		this, SLOT(imageReceived(UinType, uint32_t, uint32_t, const QString &, const char *)));
	connect(SocketNotifiers, SIGNAL(imageRequestReceived(UinType, uint32_t, uint32_t)),
		this, SLOT(imageRequestReceivedSlot(UinType, uint32_t, uint32_t)));
	connect(SocketNotifiers, SIGNAL(imageRequestReceived(UinType, uint32_t, uint32_t)),
		this, SIGNAL(imageRequestReceived(UinType, uint32_t, uint32_t)));
	connect(SocketNotifiers, SIGNAL(messageReceived(int, ContactList, QString &, time_t, QByteArray &)),
		this, SLOT(messageReceivedSlot(int, ContactList, QString &, time_t, QByteArray &)));
	connect(SocketNotifiers, SIGNAL(pubdirReplyReceived(gg_pubdir50_t)), this, SLOT(newResults(gg_pubdir50_t)));
	connect(SocketNotifiers, SIGNAL(systemMessageReceived(QString &, QDateTime &, int, void *)),
		this, SLOT(systemMessageReceived(QString &, QDateTime &, int, void *)));
	connect(SocketNotifiers, SIGNAL(userlistReceived(const struct gg_event *)),
		this, SLOT(userListReceived(const struct gg_event *)));
	connect(SocketNotifiers, SIGNAL(userlistReplyReceived(char, char *)),
		this, SLOT(userListReplyReceived(char, char *)));
	connect(SocketNotifiers, SIGNAL(userStatusChanged(const struct gg_event *)),
		this, SLOT(userStatusChanged(const struct gg_event *)));
	connect(SocketNotifiers, SIGNAL(dcc7New(struct gg_dcc7 *)), this, SIGNAL(dcc7New(struct gg_dcc7 *)));
	connect(SocketNotifiers, SIGNAL(dcc7Accepted(struct gg_dcc7 *)), this, SIGNAL(dcc7Accepted(struct gg_dcc7 *)));
	connect(SocketNotifiers, SIGNAL(dcc7Rejected(struct gg_dcc7 *)), this, SIGNAL(dcc7Rejected(struct gg_dcc7 *)));

	connect(CurrentStatus, SIGNAL(changed(const UserStatus &, const UserStatus &)),
			this, SLOT(currentStatusChanged(const UserStatus &, const UserStatus &)));

	connect(userlist, SIGNAL(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(userDataChanged(UserListElement, QString, QVariant, QVariant, bool, bool)));
	connect(userlist, SIGNAL(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool)),
			this, SLOT(protocolUserDataChanged(QString, UserListElement, QString, QVariant, QVariant, bool, bool)));
	connect(userlist, SIGNAL(userAdded(UserListElement, bool, bool)),
			this, SLOT(userAdded(UserListElement, bool, bool)));
	connect(userlist, SIGNAL(removingUser(UserListElement, bool, bool)),
			this, SLOT(removingUser(UserListElement, bool, bool)));
	connect(userlist, SIGNAL(protocolAdded(UserListElement, QString, bool, bool)),
			this, SLOT(protocolAdded(UserListElement, QString, bool, bool)));
	connect(userlist, SIGNAL(removingProtocol(UserListElement, QString, bool, bool)),
			this, SLOT(removingProtocol(UserListElement, QString, bool, bool)));

	connect(SendUserListTimer, SIGNAL(timeout()), this, SLOT(sendUserList()));

	useLastServer = lastServerIP.setAddress(config_file.readEntry("Network", "LastServerIP"));
	lastServerPort = config_file.readNumEntry("Network", "LastServerPort");
	lastTriedServerPort = config_file.readNumEntry("Network", "DefaultPort");

	kdebugf2();
}

GaduProtocol::~GaduProtocol()
{
	kdebugf();

	disconnectedSlot();
	delete SocketNotifiers;

	kdebugf2();
}

bool GaduProtocol::validateUserID(QString &uid)
{
	return true;
/*
	QIntValidator v(1, 99999999, this);
	int pos = 0;

	if ((uid != id) && (v.validate(uid, pos) == QValidator::Acceptable))
		return true;

	return false;
*/
}

void GaduProtocol::setData(AccountData *data)
{
	if (0 == data)
		GaduData = 0;
	else
		GaduData = dynamic_cast<GaduAccountData *>(data);
}

void GaduProtocol::setAccount(Account* account) {
	Protocol::setAccount(account);

	SocketNotifiers->setAccount(account);
}


UserStatus * GaduProtocol::newStatus() const
{
	return new GaduStatus();
}

void GaduProtocol::currentStatusChanged(const UserStatus &/*status*/, const UserStatus &/*oldStatus*/)
{
// TODO: 0.6.6
// 	Contact current = CurrentAccount->getContactById(QString::number(LoginParams.uin), false);
// 	if (!current.isNull())
// 		current.
//
// 	if (userlist->contains("Gadu", QString::number(LoginParams.uin)))
// 		userlist->byID("Gadu", QString::number(LoginParams.uin)).setStatus("Gadu", *CurrentStatus);
}

unsigned int GaduProtocol::maxDescriptionLength()
{
#ifdef GG_STATUS_DESCR_MAXSIZE_PRE_8_0
	if (LoginParams.protocol_version <= 0x2a)
		return GG_STATUS_DESCR_MAXSIZE_PRE_8_0;
#endif

	return GG_STATUS_DESCR_MAXSIZE;
}

void GaduProtocol::setStatus(Status status)
{
	NextStatus->setStatus(status);
	switch (status.type())
	{
		case Status::Online:
			iWantGoOnline(status.description());
			break;
		case Status::Busy:
			iWantGoBusy(status.description());
			break;
		case Status::Invisible:
			iWantGoInvisible(status.description());
			break;
		case Status::Offline:
			iWantGoOffline(status.description());
			break;
	}
}

void GaduProtocol::iWantGoOnline(const QString &desc)
{
	kdebugf();

	//nie pozwalamy na zmian� statusu lub ponowne logowanie gdy jeste�my
	//w trakcie ��czenia si� z serwerem, bo serwer zwr�ci nam g�upoty
	if (whileConnecting)
		return;

	if (CurrentStatus->isOffline())
	{
		login();
		return;
	}

	int friends = (NextStatus->isFriendsOnly() ? GG_STATUS_FRIENDS_MASK : 0);

	if (!desc.isEmpty())
		gg_change_status_descr(Sess, GG_STATUS_AVAIL_DESCR | friends, unicode2cp(desc));
	else
		gg_change_status(Sess, GG_STATUS_AVAIL | friends);

	CurrentStatus->setStatus(*NextStatus);

	kdebugf2();
}

void GaduProtocol::iWantGoBusy(const QString &desc)
{
	kdebugf();

	//patrz iWantGoOnline()
	if (whileConnecting)
		return;

	if (CurrentStatus->isOffline())
	{
		login();
		return;
	}

	int friends = (NextStatus->isFriendsOnly() ? GG_STATUS_FRIENDS_MASK : 0);

	if (!desc.isEmpty())
		gg_change_status_descr(Sess, GG_STATUS_BUSY_DESCR | friends, unicode2cp(desc));
	else
		gg_change_status(Sess, GG_STATUS_BUSY | friends);

	CurrentStatus->setStatus(*NextStatus);

	kdebugf2();
}

void GaduProtocol::iWantGoInvisible(const QString &desc)
{
	kdebugf();

	//patrz iWantGoOnline()
	if (whileConnecting)
		return;

	if (CurrentStatus->isOffline())
	{
		login();
		return;
	}

	int friends = (NextStatus->isFriendsOnly() ? GG_STATUS_FRIENDS_MASK : 0);

	if (!desc.isEmpty())
		gg_change_status_descr(Sess, GG_STATUS_INVISIBLE_DESCR | friends, unicode2cp(desc));
	else
		gg_change_status(Sess, GG_STATUS_INVISIBLE | friends);

	CurrentStatus->setStatus(*NextStatus);

	kdebugf2();
}

void GaduProtocol::iWantGoOffline(const QString &desc)
{
	kdebugf();

	if (CurrentStatus->isOffline())
	{
		if (whileConnecting)
		{
			whileConnecting = false;
			disconnectedSlot();
		}
		return;
	}

	if (!desc.isEmpty())
		gg_change_status_descr(Sess, GG_STATUS_NOT_AVAIL_DESCR, unicode2cp(desc));
	else
		gg_change_status(Sess, GG_STATUS_NOT_AVAIL);

	CurrentStatus->setStatus(*NextStatus);
	disconnectedSlot();

	kdebugf2();
}

void GaduProtocol::protocolUserDataChanged(QString protocolName, UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool /*last*/)
{
	kdebugf();
	/*
	   je�eli list� kontakt�w b�dziemy wysy�a� po kawa�ku, to serwer zgubi cz��� danych!
	   musimy wi�c wys�a� j� w ca�o�ci (poprzez sendUserList())
	   w takim w�a�nie przypadku (massively==true) nie robimy nic
	*/

	Contact contact = elem.toContact(account());

	if (protocolName != "Gadu")
		return;
	if (CurrentStatus->isOffline())
		return;
	if (name != "OfflineTo" && name != "Blocking")
		return;

	if (massively)
	{
		sendUserListLater();
		return;
	}

	UinType contactUin = uin(contact);
	if (name == "OfflineTo")
	{
		if (currentValue.toBool() && !oldValue.toBool())
		{
			gg_add_notify_ex(Sess, contactUin, GG_USER_OFFLINE);
			gg_remove_notify_ex(Sess, contactUin, GG_USER_NORMAL);
			gg_remove_notify_ex(Sess, contactUin, GG_USER_BLOCKED);
		}
		else if (!currentValue.toBool() && oldValue.toBool())
		{
			gg_add_notify_ex(Sess, contactUin, GG_USER_NORMAL);
			gg_remove_notify_ex(Sess, contactUin, GG_USER_OFFLINE);
			gg_remove_notify_ex(Sess, contactUin, GG_USER_BLOCKED);
		}
	}
	else if (name == "Blocking")
	{
		if (currentValue.toBool() && !oldValue.toBool())
		{
			gg_add_notify_ex(Sess, contactUin, GG_USER_BLOCKED);
			gg_remove_notify_ex(Sess, contactUin, GG_USER_NORMAL);
			gg_remove_notify_ex(Sess, contactUin, GG_USER_OFFLINE);
			elem.setStatus(protocolName, GaduStatus());
		}
		else if (!currentValue.toBool() && oldValue.toBool())
		{
			gg_add_notify_ex(Sess, contactUin, GG_USER_NORMAL);
			gg_remove_notify_ex(Sess, contactUin, GG_USER_OFFLINE);
			gg_remove_notify_ex(Sess, contactUin, GG_USER_BLOCKED);
		}
	}

	kdebugf2();
}

void GaduProtocol::userDataChanged(UserListElement elem, QString name, QVariant oldValue, QVariant currentValue, bool massively, bool /*last*/)
{
	kdebugf();

	Contact contact = elem.toContact(account());
	if (!contact.accountData(account()))
		return;

	if (CurrentStatus->isOffline())
		return;
	if (name != "Anonymous")
		return;

	if (massively)
		sendUserListLater();
	else
	{
		if (!currentValue.toBool() && oldValue.toBool())
			gg_add_notify(Sess, uin(contact));
	}
	kdebugf2();
}

void GaduProtocol::userAdded(UserListElement elem, bool massively, bool /*last*/)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: '%s' %d\n", qPrintable(elem.altNick()), massively/*, last*/);

	Contact contact = elem.toContact(account());
	if (!contact.accountData(account()))
		return;

	if (CurrentStatus->isOffline())
		return;

	if (massively)
		sendUserListLater();
	else
		if (!contact.isAnonymous())
			gg_add_notify(Sess, uin(contact));
	kdebugf2();
}

void GaduProtocol::removingUser(UserListElement elem, bool massively, bool /*last*/)
{
	kdebugmf(KDEBUG_FUNCTION_START, "start: '%s' %d\n", qPrintable(elem.altNick()), massively/*, last*/);

	Contact contact = elem.toContact(account());
	if (!contact.accountData(account()))
		return;

	if (CurrentStatus->isOffline())
		return;
	if (massively)
		sendUserListLater();
	else
		if (!contact.isAnonymous())
			gg_remove_notify(Sess, uin(contact));
	kdebugf2();
}

void GaduProtocol::protocolAdded(UserListElement elem, QString protocolName, bool massively, bool /*last*/)
{
	kdebugf();

	Contact contact = elem.toContact(account());
	if (contact.isNull())
		return;

	if (protocolName != "Gadu")
		return;
	if (CurrentStatus->isOffline())
		return;

	if (massively)
		sendUserListLater();
	else
		if (!contact.isAnonymous())
			gg_add_notify(Sess, uin(contact));
	kdebugf2();
}

void GaduProtocol::removingProtocol(UserListElement elem, QString protocolName, bool massively, bool /*last*/)
{
	kdebugf();

	Contact contact = elem.toContact(account());
	if (contact.isNull())
		return;

	if (protocolName != "Gadu")
		return;
	if (CurrentStatus->isOffline())
		return;

	if (massively)
		sendUserListLater();
	else
		if (!contact.isAnonymous())
			gg_remove_notify(Sess, uin(contact));
	kdebugf2();
}

QHostAddress GaduProtocol::activeServer()
{
	return ActiveServer;
}

void GaduProtocol::setDccExternalIP(const QHostAddress &ip)
{
	DccExternalIP = ip;
}

void GaduProtocol::connectedSlot()
{
	kdebugf();
	ConnectionTimeoutTimer::off();
	ConnectionTime = QDateTime::currentDateTime();

	whileConnecting = false;
	sendUserList();

	lastServerIP = QHostAddress(ntohl(Sess->server_addr));
	lastServerPort = Sess->port;
	useLastServer = true;
	config_file.writeEntry("Network", "LastServerIP", lastServerIP.toString());
	config_file.writeEntry("Network", "LastServerPort", lastServerPort);

	/* jezeli sie rozlaczymy albo stracimy polaczenie, proces laczenia sie z serwerami zaczyna sie od poczatku */
	ServerNr = 0;
	PingTimer = new QTimer(NULL, "PingTimer");
	connect(PingTimer, SIGNAL(timeout()), this, SLOT(everyMinuteActions()));
	PingTimer->start(60000);

	CurrentStatus->setStatus(*NextStatus);
	emit connected();

	// po po��czeniu z sewerem niestety trzeba ponownie ustawi�
	// status, inaczej nie b�dziemy widoczni - raczej b��d serwer�w
	if (NextStatus->isInvisible() || (LoginParams.status&~GG_STATUS_FRIENDS_MASK) != static_cast<GaduStatus *>(NextStatus)->toStatusNumber())
		NextStatus->refresh();

	/*
		UWAGA: je�eli robimy refresh(), to przy przechodzeniu z niedost�pnego z opisem
		na niewidoczny z opisem ta zmiana jest ujawniana naszym kontaktom!
		przy przechodzeniu z niedost�pnego na niewidoczny efekt nie wyst�puje

		je�eli NIE zrobimy refresh(), to powy�szy efekt nie wyst�puje, ale przy
		przechodzeniu z niedost�pnego z opisem na niewidoczny (bez opisu), nasz
		opis u innych pozostaje! (a� do czasu naszej zmiany statusu lub ich
		roz��czenia i po��czenia)
	*/

	/*
		UWAGA 2: procedura ��czenia si� z serwerem w chwili obecnej wykorzystuje
		fakt ponownego ustawienia statusu po zalogowaniu, bo iWantGo* blokuj�
		zmiany status�w w trakcie ��czenia si� z serwerem
	*/

	kdebugf2();
}

void GaduProtocol::disconnectedSlot()
{
	kdebugf();
	ConnectionTimeoutTimer::off();

	if (PingTimer)
	{
		PingTimer->stop();
		delete PingTimer;
		PingTimer = 0;
	}

	SocketNotifiers->stop();

	if (Sess)
	{
		gg_logoff(Sess);
		gg_free_session(Sess);
		Sess = 0;
	}

	// du�o bezsensownej roboty, wi�c gdy jeste�my w trakcie wy��czania,
	// to jej nie wykonujemy
	// dla ka�dego kontaktu po ustawieniu statusu emitowane s� sygna�y,
	// kt�re powoduj� od�wie�enie panelu informacyjnego, zapisanie status�w,
	// od�wie�enie okien chat�w, od�wie�enie userboksa
	if (!Kadu::closing())
		setAllOffline();

	if (!CurrentStatus->isOffline())
		CurrentStatus->setOffline(QString::null);

	emit disconnected();
	kdebugf2();
}

void GaduProtocol::socketDisconnectedSlot()
{
	kdebugf();

	NextStatus->setOffline(QString::null);
	disconnectedSlot();

	kdebugf2();
}

void GaduProtocol::connectionTimeoutTimerSlot()
{
	kdebugf();

	kdebugm(KDEBUG_INFO, "Timeout, breaking connection\n");
	errorSlot(ConnectionTimeout);

	kdebugf2();
}

void GaduProtocol::errorSlot(GaduError err)
{
	kdebugf();
	QString msg = QString::null;

	disconnectedSlot();

	emit error(err);

	bool continue_connecting = true;
	switch (err)
	{
		case ConnectionServerNotFound:
			msg = tr("Unable to connect, server has not been found");
			break;

		case ConnectionCannotConnect:
			msg = tr("Unable to connect");
			break;

		case ConnectionNeedEmail:
			msg = tr("Please change your email in \"Change password / email\" window. "
				"Leave new password field blank.");
			continue_connecting = false;
			MessageBox::msg(msg, false, "Warning");
			break;

		case ConnectionInvalidData:
			msg = tr("Unable to connect, server has returned unknown data");
			break;

		case ConnectionCannotRead:
			msg = tr("Unable to connect, connection break during reading");
			break;

		case ConnectionCannotWrite:
			msg = tr("Unable to connect, connection break during writing");
			break;

		case ConnectionIncorrectPassword:
			msg = tr("Unable to connect, incorrect password");
			continue_connecting = false;
			MessageBox::msg(tr("Connection will be stopped\nYour password is incorrect!"), false, "Critical");
			break;

		case ConnectionTlsError:
			msg = tr("Unable to connect, error of negotiation TLS");
			break;

		case ConnectionIntruderError:
			msg = tr("Too many connection attempts with bad password!");
			continue_connecting = false;
			MessageBox::msg(tr("Connection will be stopped\nToo many attempts with bad password"), false, "Critical");
			break;

		case ConnectionUnavailableError:
			msg = tr("Unable to connect, servers are down");
			break;

		case ConnectionUnknow:
			kdebugm(KDEBUG_INFO, "Connection broken unexpectedly!\nUnscheduled connection termination\n");
			break;

		case ConnectionTimeout:
			msg = tr("Connection timeout!");
			break;

		case Disconnected:
			msg = tr("Disconnection has occured");
			break;

		default:
			kdebugm(KDEBUG_ERROR, "Unhandled error? (%d)\n", int(err));
			break;
	}

	if (msg != QString::null)
	{
		QHostAddress server = activeServer();
		QString host;
		if (!server.isNull())
			host = server.toString();
		else
			host = "HUB";
		kdebugm(KDEBUG_INFO, "%s %s\n", qPrintable(host), qPrintable(msg));
		emit connectionError(account(), host, msg);
	}

	if (!continue_connecting)
		NextStatus->setOffline();

	// je�li b��d kt�ry wyst�pi� umo�liwia dalsze pr�by po��czenia
	// i w mi�dzyczasie u�ytkownik nie zmieni� statusu na niedost�pny
	// to za sekund� pr�bujemy ponownie
	if (continue_connecting && !NextStatus->isOffline())
		connectAfterOneSecond();

	kdebugf2();
}

void GaduProtocol::imageReceived(UinType sender, uint32_t size, uint32_t crc32, const QString &filename, const char *data)
{
	kdebugm(KDEBUG_INFO, qPrintable(QString("Received image. sender: %1, size: %2, crc32: %3,filename: %4\n")
		.arg(sender).arg(size).arg(crc32).arg(filename)));

	QString full_path = gadu_images_manager.saveImage(sender,size,crc32,filename,data);
	emit imageReceivedAndSaved(sender, size, crc32, full_path);
}

void GaduProtocol::imageRequestReceivedSlot(UinType sender, uint32_t size, uint32_t crc32)
{
	kdebugm(KDEBUG_INFO, qPrintable(QString("Received image request. sender: %1, size: %2, crc32: %3\n")
		.arg(sender).arg(size).arg(crc32)));

	gadu_images_manager.sendImage(sender,size,crc32);
}

void GaduProtocol::messageReceivedSlot(int msgclass, ContactList senders, QString &msg, time_t time, QByteArray &formats)
{
/*
	najpierw sprawdzamy czy nie jest to wiadomosc systemowa (senders[0] rowne 0)
	potem sprawdzamy czy user jest na naszej liscie, jezeli nie to anonymous zwroci true
	i czy jest wlaczona opcja ignorowania nieznajomych
	jezeli warunek jest spelniony przerywamy dzialanie funkcji.
*/

	// TODO : 0.6.6

	UserListElements ules = UserListElements::fromContactList(senders, account());
	if (senders[0].isAnonymous() &&
			config_file.readBoolEntry("Chat", "IgnoreAnonymousUsers") &&
			((senders.size() == 1) || config_file.readBoolEntry("Chat", "IgnoreAnonymousUsersInConferences")))
	{
		kdebugmf(KDEBUG_INFO, "Ignored anonymous. %d is ignored\n", uin(senders[0]));
		return;
	}

	// ignorujemy, jesli nick na liscie ignorowanych
	// PYTANIE CZY IGNORUJEMY CALA KONFERENCJE
	// JESLI PIERWSZY SENDER JEST IGNOROWANY????
	if (IgnoredHelper::isIgnored(senders))
		return;

	bool ignore = false;
	emit rawGaduReceivedMessageFilter(account(), senders, msg, formats, ignore);
	if (ignore)
		return;

	const char* msg_c = msg;

	Message message;
	QString content = cp2unicode(msg_c);

	QDateTime datetime;
	datetime.setTime_t(time);

// 	bool grab = false;
// 	emit chatMsgReceived0(this, senders, mesg, time, grab);
// 	if (grab)
// 		return;

	// wiadomosci systemowe maja senders[0] = 0
	// FIX ME!!!
	if (uin(senders[0]) == 0)
	{
		if (msgclass <= config_file.readNumEntry("General", "SystemMsgIndex", 0))
		{
			kdebugm(KDEBUG_INFO, "Already had this message, ignoring\n");
			return;
		}

		config_file.writeEntry("General", "SystemMsgIndex", msgclass);
		kdebugm(KDEBUG_INFO, "System message index %d\n", msgclass);

		// TODO: remove
// 		emit systemMessageReceived(mesg, datetime, formats.size(), formats.data());
		return;
	}

	if (senders[0].isAnonymous() &&
		config_file.readBoolEntry("Chat","IgnoreAnonymousRichtext"))
	{
		kdebugm(KDEBUG_INFO, "Richtext ignored from anonymous user\n");
		message = GaduFormater::createMessage(account(), uin(senders[0]), content, 0, 0, false);
	}
	else
	{
		bool receiveImages =
			!senders[0].isAnonymous() &&
			(
				CurrentStatus->isOnline() ||
				CurrentStatus->isBusy() ||
				(
					CurrentStatus->isInvisible() &&
					config_file.readBoolEntry("Chat", "ReceiveImagesDuringInvisibility")
				)
			);

		message = GaduFormater::createMessage(account(), uin(senders[0]), content,
				(unsigned char *)formats.data(), formats.size(), receiveImages);
	}

	if (message.isEmpty())
		return;

	kdebugmf(KDEBUG_INFO, "Got message from %d saying \"%s\"\n",
			uin(senders[0]), qPrintable(message.toPlain()));

	emit receivedMessageFilter(account(), senders, message.toPlain(), time, ignore);
	if (ignore)
		return;

	emit messageReceived(account(), senders, message.toHtml(), time);
}

void GaduProtocol::everyMinuteActions()
{
	kdebugf();
	gg_ping(Sess);
	sendImageRequests = 0;
	kdebugf2();
}

void GaduProtocol::systemMessageReceived(QString &message, QDateTime &time, int /*formats_length*/, void * /*formats*/)
{
	kdebugf();

	if ((time.toTime_t() == 0) || message.isEmpty())
	{
		kdebugf2();
		return;
	}

	QString mesg = time.toString("hh:mm:ss (dd.MM.yyyy): ") + message;
	kdebugm(KDEBUG_INFO, "sysMsg: %s\n", qPrintable(mesg));
	emit systemMessageReceived(mesg);

	kdebugf2();
}

void GaduProtocol::login()
{
	kdebugf();

	if (0 == GaduData->uin() || QString::null == GaduData->password())
	{
		MessageBox::msg(tr("UIN or password not set!"), false, "Warning");
		NextStatus->setOffline();
		kdebugmf(KDEBUG_FUNCTION_END, "end: uin or password not set\n");
		return;
	}

	whileConnecting = true;

	emit connecting();

	memset(&LoginParams, 0, sizeof(LoginParams));
	LoginParams.async = 1;

	// maksymalny rozmiar grafiki w kb
	LoginParams.image_size = config_file.readUnsignedNumEntry("Chat", "MaxImageSize", 0);

	setupProxy();

	LoginParams.status = static_cast<GaduStatus *>(NextStatus)->toStatusNumber();
	if (NextStatus->isFriendsOnly())
		LoginParams.status |= GG_STATUS_FRIENDS_MASK;
	if (NextStatus->hasDescription())
		LoginParams.status_descr = strdup((const char *)unicode2cp(NextStatus->description()).data());

	LoginParams.uin = GaduData->uin();
	LoginParams.has_audio = config_file.readBoolEntry("Network", "AllowDCC");
	// GG 6.0 build 147 ustawia indeks ostatnio odczytanej wiadomosci systemowej na 1389
	LoginParams.last_sysmsg = config_file.readNumEntry("General", "SystemMsgIndex", 1389);

	if (config_file.readBoolEntry("Network", "AllowDCC") && DccExternalIP.ip4Addr() && config_file.readNumEntry("Network", "ExternalPort") > 1023)
	{
		LoginParams.external_addr = htonl(DccExternalIP.ip4Addr());
		LoginParams.external_port = config_file.readNumEntry("Network", "ExternalPort");
	}
	else
	{
		LoginParams.external_addr = 0;
		LoginParams.external_port = 0;
	}

	int server_port;
	int default_port = config_file.readNumEntry("Network", "DefaultPort");
	bool connectionSequenceRestarted = false;
	if (useLastServer)
	{
		useLastServer = false;
		ActiveServer = lastServerIP;
		server_port = lastServerPort;
		lastTriedServerPort = lastServerPort;
	}
	else if (!ConfigServers.isEmpty() && !config_file.readBoolEntry("Network", "isDefServers"))
	{
		connectionSequenceRestarted = ServerNr >= ConfigServers.count();
		if (connectionSequenceRestarted)
			ServerNr = 0;

		ActiveServer = ConfigServers[ServerNr++];
	}
	else
	{
		connectionSequenceRestarted = ServerNr > gg_servers.count();
		if (connectionSequenceRestarted)
			ServerNr = 0;

		if (ServerNr > 0)
			ActiveServer = gg_servers[ServerNr - 1];
		else
			ActiveServer = QHostAddress();
		++ServerNr;
	}

	if (connectionSequenceRestarted)
	{
		if (lastTriedServerPort == 0)
			server_port = GG_HTTPS_PORT;
		else if (default_port == 0)
			server_port = 0;
		else
			server_port = default_port;
		lastTriedServerPort = server_port;
	}
	else
		server_port = lastTriedServerPort;

	if (!ActiveServer.isNull())
	{
		kdebugm(KDEBUG_INFO, "port: %d\n", server_port);
		LoginParams.server_addr = htonl(ActiveServer.ip4Addr());
		LoginParams.server_port = server_port;
	}
	else
	{
		kdebugm(KDEBUG_INFO, "trying hub\n");
		LoginParams.server_addr = 0;
		LoginParams.server_port = 0;
	}

//	polaczenia TLS z serwerami GG na razie nie dzialaja
//	LoginParams.tls = config_file.readBoolEntry("Network", "UseTLS");
	LoginParams.tls = 0;
	LoginParams.client_version = "7, 7, 0, 3351"; //tego si� nie zwalnia...
		// = GG_DEFAULT_CLIENT_VERSION
	LoginParams.protocol_version = 0x2a; // we are gg 7.7 now
		// =  GG_DEFAULT_PROTOCOL_VERSION;
	if (LoginParams.tls)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "using TLS\n");
		LoginParams.server_port = 443;
	}

	ConnectionTimeoutTimer::on();
	ConnectionTimeoutTimer::connectTimeoutRoutine(this, SLOT(connectionTimeoutTimerSlot()));

	LoginParams.password = strdup(GaduData->password().toAscii().data());
		// strdup((const char *)unicode2cp(pwHash(config_file.readEntry("General", "Password"))));
	Sess = gg_login(&LoginParams);
	memset(LoginParams.password, 0, strlen(LoginParams.password));
	free(LoginParams.password);

	if (LoginParams.status_descr)
		free(LoginParams.status_descr);

	if (Sess)
	{
		SocketNotifiers->setSession(Sess);
		SocketNotifiers->start();
	}
	else
	{
		whileConnecting = false;
		NextStatus->setOffline();
		disconnectedSlot();
		emit error(Disconnected);
	}

	kdebugf2();
}

void GaduProtocol::setupProxy()
{
	kdebugf();

	if (gg_proxy_host)
	{
		free(gg_proxy_host);
		gg_proxy_host = NULL;
	}

	if (gg_proxy_username)
	{
		free(gg_proxy_username);
		free(gg_proxy_password);
		gg_proxy_username = gg_proxy_password = NULL;
	}

	gg_proxy_enabled = config_file.readBoolEntry("Network", "UseProxy");

	if (gg_proxy_enabled)
	{
		gg_proxy_host = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyHost")).data());
		gg_proxy_port = config_file.readNumEntry("Network", "ProxyPort");

		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "gg_proxy_host = %s\n", gg_proxy_host);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "gg_proxy_port = %d\n", gg_proxy_port);

		if (!config_file.readEntry("Network", "ProxyUser").isEmpty())
		{
			gg_proxy_username = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyUser")).data());
			gg_proxy_password = strdup((char *)unicode2latin(config_file.readEntry("Network", "ProxyPassword")).data());
		}
	}

	kdebugf2();
}

bool GaduProtocol::sendMessage(ContactList contacts, Message &message)
{
	kdebugf();

	message.setId(-1);
	QString plain = message.toPlain();

	unsigned int uinsCount = 0;
	unsigned int formatsSize = 0;
	unsigned char *formats = GaduFormater::createFormats(message, formatsSize);
	bool stop = false;

	plain.replace("\r\n", "\n");
	plain.replace("\r", "\n");

	kdebugmf(KDEBUG_INFO, "\n%s\n", (const char *)unicode2latin(plain));

	QByteArray data = unicode2cp(plain);

	emit sendMessageFiltering(contacts, data, stop);

	if (stop)
	{
		kdebugmf(KDEBUG_FUNCTION_END, "end: filter stopped processing\n");
		return false;
	}

	if (data.length() >= 2000)
	{
		MessageBox::msg(tr("Filtered message too long (%1>=%2)").arg(data.length()).arg(2000), false, "Warning");
		kdebugmf(KDEBUG_FUNCTION_END, "end: filtered message too long\n");
		return false;
	}

	foreach (const Contact &contact, contacts)
		if (contact.accountData(account()))
			++uinsCount;

	if (uinsCount > 1)
	{
		UinType* uins = new UinType[uinsCount];
		unsigned int i = 0;
	foreach (const Contact &contact, contacts)
		if (contact.accountData(account()))
				uins[i++] = uin(contact);
		if (formatsSize)
			seqNumber = gg_send_message_confer_richtext(Sess, GG_CLASS_CHAT, uinsCount, uins, (unsigned char *)data.data(),
				formats, formatsSize);
		else
			seqNumber = gg_send_message_confer(Sess, GG_CLASS_CHAT, uinsCount, uins,(unsigned char *)data.data());
		delete[] uins;
	}
	else
		foreach (const Contact &contact, contacts)
			if (contact.accountData(account()))
			{
				if (formatsSize)
					seqNumber = gg_send_message_richtext(Sess, GG_CLASS_CHAT, uin(contact), (unsigned char *)data.data(),
						formats, formatsSize);
				else
					seqNumber = gg_send_message(Sess, GG_CLASS_CHAT, uin(contact),(unsigned char *)data.data());

				break;
			}

	message.setId(seqNumber);

	SocketNotifiers->checkWrite();
	if (formats)
		delete[] formats;

	kdebugf2();
	return true;
}

void GaduProtocol::ackReceived(int seq, uin_t uin, int status)
{
	kdebugf();
	if (seq != seqNumber)
		return;

	switch (status)
	{
		case GG_ACK_DELIVERED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message delivered (uin: %d, seq: %d)\n", uin, seq);
			emit messageStatusChanged(seq, StatusAcceptedDelivered);
			break;
		case GG_ACK_QUEUED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message queued (uin: %d, seq: %d)\n", uin, seq);
			emit messageStatusChanged(seq, StatusAcceptedQueued);
			break;
		case GG_ACK_BLOCKED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message blocked (uin: %d, seq: %d)\n", uin, seq);
			emit messageStatusChanged(seq, StatusRejectedBlocked);
			break;
		case GG_ACK_MBOXFULL:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message box full (uin: %d, seq: %d)\n", uin, seq);
			emit messageStatusChanged(seq, StatusRejectedBoxFull);
			break;
		case GG_ACK_NOT_DELIVERED:
			kdebugm(KDEBUG_NETWORK|KDEBUG_INFO, "message not delivered (uin: %d, seq: %d)\n", uin, seq);
			emit messageStatusChanged(seq, StatusRejectedUnknown);
			break;
		default:
			kdebugm(KDEBUG_NETWORK|KDEBUG_WARNING, "unknown acknowledge! (uin: %d, seq: %d, status:%d)\n", uin, seq, status);
			break;
	}
	kdebugf2();
}

void GaduProtocol::sendUserListLater()
{
//	kdebugf();
	SendUserListTimer->start(0, true);
//	kdebugf2();
}

void GaduProtocol::sendUserList()
{
	kdebugf();
	UinType *uins;
	char *types;

	ContactList contacts = ContactManager::instance()->contacts(account());

	if (contacts.isEmpty())
	{
		gg_notify_ex(Sess, NULL, NULL, 0);
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist is empty\n");
		return;
	}

	uins = new UinType[contacts.count()];
	types = new char[contacts.count()];

	int i = 0;

	foreach (Contact contact, contacts)
	{
		uins[i] = uin(contact);

		if (contact.isOfflineTo(account()))
			types[i] = GG_USER_OFFLINE;
		else
			if (contact.isBlocked(account()))
				types[i] = GG_USER_BLOCKED;
			else
				types[i] = GG_USER_NORMAL;
		++i;
	}

	gg_notify_ex(Sess, uins, types, contacts.count());
	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Userlist sent\n");

	delete [] uins;
	delete [] types;

	kdebugf2();
}

bool GaduProtocol::sendImageRequest(Contact contact, int size, uint32_t crc32)
{
	kdebugf();
	int res = 1;
	if (contact.accountData(account()) &&
	    (sendImageRequests <= config_file.readUnsignedNumEntry("Chat", "MaxImageRequests")))
	{
		res = gg_image_request(Sess, uin(contact), size, crc32);
		sendImageRequests++;
	}
	kdebugf2();
	return (res == 0);
}

bool GaduProtocol::sendImage(Contact contact, const QString &file_name, uint32_t size, const char *data)
{
	kdebugf();
	int res = 1;
	if (contact.accountData(account()))
		res = gg_image_reply(Sess, uin(contact), qPrintable(file_name), data, size);
	kdebugf2();
	return (res == 0);
}

/* wyszukiwanie w katalogu publicznym */

void GaduProtocol::searchInPubdir(SearchRecord& searchRecord)
{
	kdebugf();
	searchRecord.FromUin = 0;
	searchNextInPubdir(searchRecord);
	kdebugf2();
}

void GaduProtocol::searchNextInPubdir(SearchRecord& searchRecord)
{
	kdebugf();
	QString bufYear;
	gg_pubdir50_t req;

	req = gg_pubdir50_new(GG_PUBDIR50_SEARCH);

	if (!searchRecord.Uin.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_UIN, (const char *)unicode2cp(searchRecord.Uin).data());
	if (!searchRecord.FirstName.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)unicode2cp(searchRecord.FirstName).data());
	if (!searchRecord.LastName.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)unicode2cp(searchRecord.LastName).data());
	if (!searchRecord.NickName.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)unicode2cp(searchRecord.NickName).data());
	if (!searchRecord.City.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)unicode2cp(searchRecord.City).data());
	if (!searchRecord.BirthYearFrom.isEmpty())
	{
		QString bufYear = searchRecord.BirthYearFrom + ' ' + searchRecord.BirthYearTo;
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)unicode2cp(bufYear).data());
	}

	switch (searchRecord.Gender)
	{
		case 1:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_MALE);
			break;
		case 2:
			gg_pubdir50_add(req, GG_PUBDIR50_GENDER, GG_PUBDIR50_GENDER_FEMALE);
			break;
	}

	if (searchRecord.Active)
		gg_pubdir50_add(req, GG_PUBDIR50_ACTIVE, GG_PUBDIR50_ACTIVE_TRUE);

	gg_pubdir50_add(req, GG_PUBDIR50_START, qPrintable(QString::number(searchRecord.FromUin)));

	searchRecord.Seq = gg_pubdir50(Sess, req);
	gg_pubdir50_free(req);
	kdebugf2();
}

void GaduProtocol::stopSearchInPubdir(SearchRecord &searchRecord)
{
	kdebugf();
	searchRecord.IgnoreResults = true;
	kdebugf2();
}

void GaduProtocol::newResults(gg_pubdir50_t res)
{
	kdebugf();
	int count, fromUin;
	SearchResult searchResult;
	SearchResults searchResults;

	count = gg_pubdir50_count(res);

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "found %d results\n", count);

	for (int i = 0; i < count; ++i)
	{
		searchResult.setData(
			gg_pubdir50_get(res, i, GG_PUBDIR50_UIN),
			gg_pubdir50_get(res, i, GG_PUBDIR50_FIRSTNAME),
			gg_pubdir50_get(res, i, GG_PUBDIR50_LASTNAME),
			gg_pubdir50_get(res, i, GG_PUBDIR50_NICKNAME),
			gg_pubdir50_get(res, i, GG_PUBDIR50_BIRTHYEAR),
			gg_pubdir50_get(res, i, GG_PUBDIR50_CITY),
			gg_pubdir50_get(res, i, GG_PUBDIR50_FAMILYNAME),
			gg_pubdir50_get(res, i, GG_PUBDIR50_FAMILYCITY),
			gg_pubdir50_get(res, i, GG_PUBDIR50_GENDER),
			gg_pubdir50_get(res, i, GG_PUBDIR50_STATUS)
		);
		searchResults.append(searchResult);
	}
	fromUin = gg_pubdir50_next(res);

	emit newSearchResults(searchResults, res->seq, fromUin);
	kdebugf2();
}

/* informacje osobiste */

void GaduProtocol::getPersonalInfo(SearchRecord &searchRecord)
{
	kdebugf();

	gg_pubdir50_t req;

	req = gg_pubdir50_new(GG_PUBDIR50_READ);
	searchRecord.Seq = gg_pubdir50(Sess, req);
	gg_pubdir50_free(req);
	kdebugf2();
}

void GaduProtocol::setPersonalInfo(SearchRecord &searchRecord, SearchResult &newData)
{
	kdebugf();

	gg_pubdir50_t req;
	req = gg_pubdir50_new(GG_PUBDIR50_WRITE);

	if (!newData.First.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FIRSTNAME, (const char *)(unicode2cp(newData.First).data()));
	if (!newData.Last.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_LASTNAME, (const char *)(unicode2cp(newData.Last).data()));
	if (!newData.Nick.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_NICKNAME, (const char *)(unicode2cp(newData.Nick).data()));
	if (!newData.City.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_CITY, (const char *)(unicode2cp(newData.City).data()));
	if (!newData.Born.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_BIRTHYEAR, (const char *)(unicode2cp(newData.Born).data()));
	if (newData.Gender)
		gg_pubdir50_add(req, GG_PUBDIR50_GENDER, QString::number(newData.Gender).latin1());
	if (!newData.FamilyName.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYNAME, (const char *)(unicode2cp(newData.FamilyName).data()));
	if (!newData.FamilyCity.isEmpty())
		gg_pubdir50_add(req, GG_PUBDIR50_FAMILYCITY, (const char *)(unicode2cp(newData.FamilyCity).data()));

	searchRecord.Seq = gg_pubdir50(Sess, req);
	gg_pubdir50_free(req);
	kdebugf2();
}

// -----------------------------
//      Zarz�dzanie kontem
// -----------------------------

void GaduProtocol::getToken()
{
	GaduTokenSocketNotifiers *sn = new GaduTokenSocketNotifiers(this);
	connect(sn, SIGNAL(tokenError()), this, SLOT(tokenError()));
	connect(sn, SIGNAL(gotToken(QString, QPixmap)), this, SLOT(gotToken(QString, QPixmap)));
	sn->start();
}

void GaduProtocol::registerAccount(const QString &mail, const QString &password)
{
	kdebugf();

	Mode = Register;
	DataEmail = mail;
	DataPassword = password;
	getToken();

	kdebugf2();
}

void GaduProtocol::unregisterAccount(UinType uin, const QString &password)
{
	kdebugf();

	Mode = Unregister;
	DataUin = uin;
	DataPassword = password;
	getToken();

	kdebugf2();
}

void GaduProtocol::remindPassword(UinType uin, const QString &mail)
{
	kdebugf();

	Mode = RemindPassword;
	DataUin = uin;
	DataEmail = mail;
	getToken();

	kdebugf2();
}

void GaduProtocol::changePassword(UinType uin, const QString &mail, const QString &password, const QString &newPassword)
{
	kdebugf();

	Mode = ChangePassword;
	DataUin = uin;
	DataEmail = mail;
	DataPassword = password;
	DataNewPassword = newPassword;
	getToken();

	kdebugf2();
}

void GaduProtocol::doRegisterAccount()
{
	kdebugf();
	struct gg_http *h = gg_register3(unicode2cp(DataEmail).data(), unicode2cp(DataPassword).data(),
		unicode2cp(TokenId).data(), unicode2cp(TokenValue).data(), 1);
	if (h)
	{
		GaduPubdirSocketNotifiers *sn = new GaduPubdirSocketNotifiers(h, this);
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(registerDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit registered(false, 0);
	kdebugf2();
}

void GaduProtocol::registerDone(bool ok, struct gg_http *h)
{
	kdebugf();
	emit registered(ok, ok ? (((struct gg_pubdir *)h->data)->uin) : 0);
	kdebugf2();
}

void GaduProtocol::doUnregisterAccount()
{
	kdebugf();
	struct gg_http* h = gg_unregister3(DataUin, unicode2cp(DataPassword).data(), unicode2cp(TokenId).data(),
		unicode2cp(TokenValue).data(), 1);
	if (h)
	{
		GaduPubdirSocketNotifiers *sn = new GaduPubdirSocketNotifiers(h, this);
		connect(sn, SIGNAL(done(bool, struct gg_http *)),
			this, SLOT(unregisterDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit unregistered(false);
	kdebugf2();
}

void GaduProtocol::unregisterDone(bool ok, struct gg_http *)
{
	kdebugf();
	emit unregistered(ok);
	kdebugf2();
}

void GaduProtocol::doRemindPassword()
{
	kdebugf();

	struct gg_http *h = gg_remind_passwd3(DataUin, unicode2cp(DataEmail).data(), unicode2cp(TokenId).data(),
		unicode2cp(TokenValue).data(), 1);
	if (h)
	{
		GaduPubdirSocketNotifiers *sn = new GaduPubdirSocketNotifiers(h, this);
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this, SLOT(remindDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit reminded(false);
	kdebugf2();
}

void GaduProtocol::remindDone(bool ok, struct gg_http *)
{
	kdebugf();
	emit reminded(ok);
	kdebugf2();
}

void GaduProtocol::doChangePassword()
{
	kdebugf();

	struct gg_http *h = gg_change_passwd4(DataUin, unicode2cp(DataEmail).data(),
		unicode2cp(DataPassword).data(), unicode2cp(DataNewPassword).data(),
		unicode2cp(TokenId).data(), unicode2cp(TokenValue).data(), 1);
	if (h)
	{
		GaduPubdirSocketNotifiers *sn = new GaduPubdirSocketNotifiers(h, this);
		connect(sn, SIGNAL(done(bool, struct gg_http *)), this,
			SLOT(changePasswordDone(bool, struct gg_http *)));
		sn->start();
	}
	else
		emit passwordChanged(false);
	kdebugf2();
}

void GaduProtocol::changePasswordDone(bool ok, struct gg_http *)
{
	kdebugf();
	emit passwordChanged(ok);
	kdebugf2();
}

/* tokeny */

void GaduProtocol::tokenError()
{
	kdebugf();
	switch (Mode)
	{
		case Register:
			emit registered(false, 0);
			break;
		case Unregister:
			emit unregistered(false);
			break;
		case RemindPassword:
			emit reminded(false);
			break;
		case ChangePassword:
			emit passwordChanged(false);
			break;
	}
	kdebugf2();
}

void GaduProtocol::gotToken(QString tokenId, QPixmap tokenImage)
{
	kdebugf();

	QString tokenValue;
	emit needTokenValue(tokenImage, tokenValue);

	TokenId = tokenId;
	TokenValue = tokenValue;

	switch (Mode)
	{
		case Register:
			doRegisterAccount();
			break;
		case Unregister:
			doUnregisterAccount();
			break;
		case RemindPassword:
			doRemindPassword();
			break;
		case ChangePassword:
			doChangePassword();
			break;
	}

	kdebugf2();
}

/* lista u�ytkownik�w */

QString GaduProtocol::userListToString(const UserList &userList) const
{
	kdebugf();
	NotifyType type;
	QString file;
	QString contacts;

	foreach(const UserListElement &user, *userlist)
		if (!user.isAnonymous() && (user.usesProtocol("Gadu") || !user.mobile().isEmpty()))
		{
			contacts += user.firstName();					contacts += ';';
			contacts += user.lastName();					contacts += ';';
			contacts += user.nickName();					contacts += ';';
			contacts += user.altNick();						contacts += ';';
			contacts += user.mobile();						contacts += ';';
			contacts += user.data("Groups").toStringList().join(";");	contacts += ';';
			if (user.usesProtocol("Gadu"))
				contacts += user.ID("Gadu");				contacts += ';';
			contacts += user.email();						contacts += ';';
			file = user.aliveSound(type);
			contacts += QString::number(type);				contacts += ';';
			contacts += file;								contacts += ';';
			file = user.messageSound(type);
			contacts += QString::number(type);				contacts += ';';
			contacts += file;								contacts += ';';
			if (user.usesProtocol("Gadu"))
				contacts += QString::number(user.protocolData("Gadu", "OfflineTo").toBool());
			contacts += ';';
			contacts += user.homePhone();					//contacts += ';';
			contacts += "\r\n";
		}

	contacts.remove("(null)");

//	kdebugm(KDEBUG_DUMP, "%s\n", qPrintable(contacts));
	kdebugf2();
	return contacts;
}

QList<UserListElement> GaduProtocol::stringToUserList(const QString &string) const
{
	QString s = string;
	QTextStream stream(&s, QIODevice::ReadOnly);
	return streamToUserList(stream);
}

QList<UserListElement> GaduProtocol::streamToUserList(QTextStream &stream) const
{
	kdebugf();

	QStringList sections, groupNames;
	QString line;
	QList<UserListElement> ret;
	unsigned int i, secCount;
	bool ok;

	stream.setCodec(codec_latin2);

	while (!stream.atEnd())
	{
		UserListElement e;
		line = stream.readLine();
//		kdebugm(KDEBUG_DUMP, ">>%s\n", qPrintable(line));
		sections = QStringList::split(";", line, true);
		secCount = sections.count();

		if (secCount < 7)
			continue;

		e.setFirstName(sections[0]);
		e.setLastName(sections[1]);
		e.setNickName(sections[2]);
		e.setAltNick(sections[3]);
		e.setMobile(sections[4]);

		groupNames.clear();
		if (!sections[5].isEmpty())
			groupNames.append(sections[5]);

		i = 6;
		ok = false;
		while (!ok && i < secCount)
		{
//			kdebugm(KDEBUG_DUMP, "checking: '%s'\n", qPrintable(sections[i]));
			sections[i].toULong(&ok);
			ok = ok || sections[i].isEmpty();
			if (!ok)
			{
//				kdebugm(KDEBUG_DUMP, "adding: '%s'\n", qPrintable(sections[i]));
				groupNames.append(sections[i]);
			}
			++i;
		}
		e.setData("Groups", groupNames);
		--i;

		if (i < secCount)
		{
			UinType uin = sections[i++].toULong(&ok);
			if (!ok)
				uin = 0;
			if (uin)
				e.addProtocol("Gadu", QString::number(uin));
		}

		if (i < secCount)
			e.setEmail(sections[i++]);
		if (i+1 < secCount)
		{
			e.setAliveSound((NotifyType)sections[i].toInt(), sections[i+1]);
			i+=2;
		}
		if (i+1 < secCount)
		{
			e.setMessageSound((NotifyType)sections[i].toInt(), sections[i+1]);
			i+=2;
		}
		if (i < secCount)
		{
			if (e.usesProtocol("Gadu"))
				e.setProtocolData("Gadu", "OfflineTo", bool(sections[i].toInt()));
			i++;
		}
		if (i < secCount)
			e.setHomePhone(sections[i++]);

		ret.append(e);
	}
	kdebugf2();
	return ret;
}

void GaduProtocol::connectAfterOneSecond()
{
	kdebugf();
	QTimer::singleShot(1000, this, SLOT(login()));
	kdebugf2();
}

bool GaduProtocol::doExportUserList(const UserList &userList)
{
	kdebugf();

	QString contacts = userListToString(userList);
	char *dup = strdup(unicode2cp(contacts));

	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "\n%s\n", dup);
//	free(dup);

	UserListClear = false;

//	dup = strdup(unicode2std(contacts));
	bool success=(gg_userlist_request(Sess, GG_USERLIST_PUT, dup)!=-1);
	if (!success)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "failed\n");
		emit userListExported(false);
	}
	free(dup);
	kdebugf2();
	return success;
}

bool GaduProtocol::doClearUserList()
{
	kdebugf();

	UserListClear = true;

	const char *dup = "";
	bool success=(gg_userlist_request(Sess, GG_USERLIST_PUT, dup) != -1);
	if (!success)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "failed\n");
		emit userListCleared(false);
	}
	kdebugf2();
	return success;
}

bool GaduProtocol::doImportUserList()
{
	kdebugf();

	ImportReply.truncate(0);

	bool success=(gg_userlist_request(Sess, GG_USERLIST_GET, NULL) != -1);
	if (!success)
		emit userListImported(false, QList<UserListElement>());
	kdebugf2();
	return success;
}

void GaduProtocol::userListReceived(const struct gg_event *e)
{
	kdebugf();

	int nr = 0;
	//kdebugm(KDEBUG_WARNING, "%s\n", qPrintable(userListToString(*userlist)));
	//return;

	int cnt = 0;
	while (e->event.notify60[nr].uin) // zliczamy najpierw ile zmian status�w zostanie wyemitowanych
	{
		if (!userlist->byID("Gadu", QString::number(e->event.notify60[nr].uin)).isAnonymous())
			++cnt;
		++nr;
	}
	nr = 0;
	//a teraz b�dziemy przetwarza�

	while (e->event.notify60[nr].uin)
	{
		UserListElement user = userlist->byID("Gadu", QString::number(e->event.notify60[nr].uin));
		Contact contact = user.toContact(account());

		if (contact.isAnonymous())
		{
			kdebugmf(KDEBUG_INFO, "buddy %d not in list. Damned server!\n",
					e->event.notify60[nr].uin);
			gg_remove_notify(Sess, e->event.notify60[nr].uin);
			++nr;
			continue;
		}

		GaduContactAccountData *accountData = gaduContactAccountData(contact);

		user.setProtocolData("Gadu", "DNSName", "", nr + 1 == cnt);
		user.setProtocolData("Gadu", "IP", (unsigned int)ntohl(e->event.notify60[nr].remote_ip), nr + 1 == cnt);
		user.setProtocolData("Gadu", "Port", e->event.notify60[nr].remote_port, nr + 1 == cnt);
		user.refreshDNSName("Gadu");

		user.setProtocolData("Gadu", "Version", e->event.notify60[nr].version, true, nr + 1 == cnt);
		user.setProtocolData("Gadu", "MaxImageSize", e->event.notify60[nr].image_size, true, nr + 1 == cnt);

		Status oldStatus = accountData->status();

		int gadu_status_id;
		QString description;

		if (e->event.notify60[nr].descr)
		{
			gadu_status_id = e->event.notify60[nr].status;
			description = cp2unicode(e->event.notify60[nr].descr);

			description.replace("\r\n", "\n");
			description.replace("\r", "\n");
		}
		else
			gadu_status_id = e->event.notify60[nr].status;

		Status status(statusTypeFromIndex(gadu_status_id), description);
		accountData->setStatus(status);

		emit contactStatusChanged(account(), contact, oldStatus);

#ifdef DEBUG_ENABLED
#define PRINT_STAT(str) kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, str, e->event.notify60[nr].uin);
		switch (e->event.notify60[nr].status)
		{
			case GG_STATUS_AVAIL:			PRINT_STAT("User %d went online\n");					break;
			case GG_STATUS_BUSY:			PRINT_STAT("User %d went busy\n");						break;
			case GG_STATUS_NOT_AVAIL:		PRINT_STAT("User %d went offline\n");					break;
			case GG_STATUS_BLOCKED:			PRINT_STAT("User %d has blocked us\n");					break;
			case GG_STATUS_BUSY_DESCR:		PRINT_STAT("User %d went busy with description\n");		break;
			case GG_STATUS_NOT_AVAIL_DESCR:	PRINT_STAT("User %d went offline with description\n");	break;
			case GG_STATUS_AVAIL_DESCR:		PRINT_STAT("User %d went online with description\n");	break;
			case GG_STATUS_INVISIBLE_DESCR:	PRINT_STAT("User %d went invisible with description\n");break;
			default:
				kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Unknown status for user %d: %d\n", e->event.notify60[nr].uin, e->event.notify60[nr].status);
				break;
		}
#endif

		++nr;
	}

	kdebugf2();
}

void GaduProtocol::userListReplyReceived(char type, char *reply)
{
	kdebugf();

	if (type == GG_USERLIST_PUT_REPLY)
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "Done\n");

		if (UserListClear)
			emit userListCleared(true);
		else
			emit userListExported(true);

	}
	else if ((type == GG_USERLIST_GET_REPLY) || (type == GG_USERLIST_GET_MORE_REPLY))
	{
		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "get\n");

		if (!reply)
		{
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "error!\n");

			emit userListImported(false, QList<UserListElement>());
			return;
		}

		if (reply[0] != 0)
			ImportReply += cp2unicode(reply);

		if (type == GG_USERLIST_GET_MORE_REPLY)
		{
			kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "next portion\n");
			return;
		}

		kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "\n%s\n", unicode2latin(ImportReply).data());

		emit userListImported(true, stringToUserList(ImportReply));
	}

	kdebugf2();
}

#define GG_STATUS_INVISIBLE2 0x0009
Status::StatusType GaduProtocol::statusTypeFromIndex(unsigned int index) const
{
	switch (index)
	{
		case GG_STATUS_AVAIL_DESCR:
		case GG_STATUS_AVAIL:
			return Status::Online;

		case GG_STATUS_BUSY_DESCR:
		case GG_STATUS_BUSY:
			return Status::Busy;

		case GG_STATUS_INVISIBLE_DESCR:
		case GG_STATUS_INVISIBLE:
		case GG_STATUS_INVISIBLE2:
			return Status::Invisible;

		case GG_STATUS_BLOCKED:
		case GG_STATUS_NOT_AVAIL_DESCR:
		case GG_STATUS_NOT_AVAIL:
		default:
			return Status::Offline;
	}

	return Status::Offline;
}

void GaduProtocol::userStatusChanged(const struct gg_event *e)
{
	kdebugf();

	int gadu_status_id;
	QString description;

	uint32_t uin;
	uint32_t remote_ip;
	uint16_t remote_port;
	uint8_t version;
	uint8_t image_size;

	if (e->type == GG_EVENT_STATUS60)
	{
		uin = e->event.status60.uin;
		gadu_status_id = e->event.status60.status;
		description = cp2unicode(e->event.status60.descr);
		remote_ip = e->event.status60.remote_ip;
		remote_port = e->event.status60.remote_port;
		version = e->event.status60.version;
		image_size = e->event.status60.image_size;
	}
	else
	{
		uin = e->event.status.uin;
		gadu_status_id = e->event.status.status;
		description = cp2unicode(e->event.status.descr);
		remote_ip = 0;
		remote_port = 0;
		version = 0;
		image_size = 0;
	}

	description.replace("\r\n", "\n");
	description.replace("\r", "\n");

	Status status(statusTypeFromIndex(gadu_status_id), description);
// TODO: 0.6.6
//	kdebugmf(KDEBUG_NETWORK|KDEBUG_INFO, "User %d went %d (%s)\n", uin,
//		status.type(), qPrintable(status.name()));

	Contact contact = account()->getContactById(QString::number(uin));

	if (contact.isAnonymous())
	{
		// ignore!
		kdebugmf(KDEBUG_INFO, "buddy %d not in list. Damned server!\n", uin);
		gg_remove_notify(Sess, uin);
		emit userStatusChangeIgnored(contact);
		return;
	}

	GaduContactAccountData *data = gaduContactAccountData(contact);

	if (status.isOffline())
	{
		remote_ip = 0;
		remote_port = 0;
		version = 0;
		image_size = 0;
	}

	data->setAddressAndPort(QHostAddress((quint32)(ntohl(remote_ip))), remote_port);
	data->setProtocolVersion(QString::number(version));
	data->setMaxImageSize(image_size);
// 	user.setProtocolData("Gadu", "Version", version);
// 	user.setProtocolData("Gadu", "dMaxImageSize", image_size);

// TODO: 0.6.5
// 	user.refreshDNSName("Gadu");

	Status oldStatus = data->status();
	data->setStatus(status);

	emit contactStatusChanged(account(), contact, oldStatus);
}

void GaduProtocol::dccRequest(UinType uin)
{
	gg_dcc_request(Sess, uin);
}

void GaduProtocol::setDccIpAndPort(unsigned long dcc_ip, int dcc_port)
{
	gg_dcc_ip = dcc_ip;
	gg_dcc_port = dcc_port;
}

AccountData * GaduProtocol::createAccountData()
{
	return new GaduAccountData();
}

unsigned int GaduProtocol::uin(Contact contact) const
{
	GaduContactAccountData *data = gaduContactAccountData(contact);
	return data
		? data->uin()
		: 0;
}

GaduContactAccountData * GaduProtocol::gaduContactAccountData(Contact contact) const
{
	return dynamic_cast<GaduContactAccountData *>(contact.accountData(account()));
}

QPixmap GaduProtocol::statusPixmap(Status status)
{
	QString description = status.description().isNull()
			? ""
			: "WithDescription";
	QString pixmapName;

	switch (status.type())
	{
		case Status::Online:
			pixmapName = QString("Online").append(description);
			break;
		case Status::Busy:
			pixmapName = QString("Busy").append(description);
			break;
		case Status::Invisible:
			pixmapName = QString("Invisible").append(description);
			break;
		default:
			pixmapName = QString("Offline").append(description);
			break;
	}

	return icons_manager->loadPixmap(pixmapName);
}
