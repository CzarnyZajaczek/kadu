/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qfile.h>
#include <qtextstream.h>
#include <qtextcodec.h>
#include <qdns.h>

#include <sys/stat.h>

#include "userlist.h"
#include "misc.h"
#include "userbox.h"
#include "debug.h"

DnsHandler::DnsHandler(uin_t uin) : uin(uin) {
	UserListElement &ule = userlist.byUin(uin);
	completed = false;
	connect(&dnsresolver, SIGNAL(resultsReady()), this, SLOT(resultsReady()));
	dnsresolver.setRecordType(QDns::Ptr);
	dnsresolver.setLabel(ule.ip);
	counter++;
	kdebug("DnsHandler::DnsHandler(): counter = %d\n", counter);
}

DnsHandler::~DnsHandler() {
	counter--;
/* patrz ~Userlist()
	kdebug("DnsHandler::~DnsHandler(): counter = %d\n", counter);
*/
}

void DnsHandler::resultsReady() {
	if (dnsresolver.hostNames().count())
		userlist.setDnsName(uin, dnsresolver.hostNames()[0]);
	else
		userlist.setDnsName(uin, QString::null);
	completed = true;
}

int DnsHandler::counter = 0;

bool DnsHandler::isCompleted() {
	return completed;
}

UserListElement::UserListElement(UserList* parent)
{
	Parent = parent;
	version = 0;
	status = GG_STATUS_NOT_AVAIL;
	image_size = 0;
	port = 0;
	blocking = false;
	offline_to_user = false;
	notify = true;
	anonymous = false;
}

UserListElement::UserListElement()
{
	Parent = NULL;
	version = 0;
	status = GG_STATUS_NOT_AVAIL;
	image_size = 0;
	port = 0;
	QHostAddress ipaddress;
	ipaddress.setAddress("0.0.0.0");
	ip = ipaddress;
	blocking = false;
	offline_to_user = false;
	notify = true;
	anonymous = false;
}

QString UserListElement::group()
{
	return Group;
};

void UserListElement::setGroup(const QString& group)
{
	if (group == QT_TR_NOOP("All"))
		Group="";
	else
		Group=group;
	if (Parent)
		emit Parent->modified();
};

UserList::UserList(const UserList& source)
{
	for(const_iterator i=source.begin(); i!=source.end(); i++)
		append(*i);
};

UserList::UserList() : QObject(), QValueList<UserListElement>()
{
	dnslookups.setAutoDelete(true);
};

UserList::~UserList()
{
	/* Ciekawa zagadka... ten kdebug przy zamykaniu powodowa� naruszenie
		pami�ci oboj�tnie, co mia� w parametrach, debugowanie tego nie jest
		konieczne wi�c zakomentowa�em.
		kdebug("UserList::~UserList(): dnslookups.count() = %d\n", dnslookups.count());
	*/
	/* chilek: Mysle, ze nie jest to takie mocno ciekawe, zwazywszy na to, ze pare linii
		wyzej mamy w konstruktorze dnslookups ustawiony na autoDelete ;)
		Po prostu tu w destruktorze klasy UserList pewnie juz obiekt dnslookups zostal
		zniszczony i stad problem z wyswietleniem pewnych jego atrybutow.
	*/
}

void UserList::addDnsLookup(uin_t uin, const QHostAddress &ip) {
	DnsHandler *dnshandler;
	dnshandler = dnslookups.first();
	while (dnshandler) {
		if (dnshandler->isCompleted()) {
			dnslookups.remove();
			dnshandler = dnslookups.current();
			}
		else
			dnshandler = dnslookups.next();
		}
	if (!containsUin(uin))
		return;
	UserListElement &ule = byUin(uin);
	dnshandler = new DnsHandler(uin);
	dnslookups.append(dnshandler);
}

void UserList::setDnsName(uin_t uin, const QString &name) {
	if (!containsUin(uin))
		return;
	UserListElement &ule = byUin(uin);
	if (ule.dnsname != name) {
		ule.dnsname = name;
		kdebug("UserList::setDnsName(): dnsname for uin %d: %s\n", uin, name.latin1());
		emit dnsNameReady(uin);
		}
}

UserListElement& UserList::byUin(uin_t uin)
{
	for(iterator i=begin(); i!=end(); i++)
		if((*i).uin==uin)
			return (*i);
	kdebug("UserList::byUin(): Panic!\n");
};

UserListElement& UserList::byNick(const QString& nickname)
{
	for (iterator i = begin(); i != end(); i++)
		if ((*i).nickname.lower() == nickname.lower())
			return (*i);
	kdebug("UserList::byNick(): Panic! %s not exists\n",
		(const char*)nickname.lower().local8Bit());
};

UserListElement& UserList::byAltNick(const QString& altnick)
{
	for (iterator i = begin(); i != end(); i++)
		if ((*i).altnick.lower() == altnick.lower())
			return (*i);
	kdebug("UserList::byAltNick(): Panic! %s not exists\n",
		(const char*)altnick.lower().local8Bit());
};

//Zwraca elementy userlisty, jezeli nie mamy danego
//uin na liscie, zwracany jest UserListElement tylko z uin i altnick == uin
UserListElement UserList::byUinValue(uin_t uin)
{
	for (iterator i = begin(); i != end(); i++)
		if ((*i).uin == uin)
			return (*i);
	UserListElement ule;
	ule.uin = uin;
	ule.altnick = QString::number(uin);
	ule.anonymous = true;
	return ule;
};

bool UserList::containsUin(uin_t uin) {
	for (iterator i = begin(); i != end(); i++)
		if ((*i).uin == uin)
			return true;
	kdebug("UserList::containsUin(): userlist doesnt contain %d\n", uin);
	return false;
}

bool UserList::containsAltNick(const QString &altnick) {
	for (iterator i = begin(); i != end(); i++)
		if ((*i).altnick.lower() == altnick.lower())
			return true;
	kdebug("UserList::containsAltNick(): userlist doesnt contain %s\n",
		(const char *)altnick.lower().local8Bit());
	return false;
}

void UserList::addUser(UserListElement& ule)
{
	UserListElement e(this);
	e.first_name = ule.first_name;
	e.last_name = ule.last_name;
	e.nickname = ule.nickname;
	e.altnick = ule.altnick;
	e.mobile = ule.mobile;
	e.uin = ule.uin;
	e.status = ule.status;
	e.image_size = ule.image_size;
	e.blocking = ule.blocking;
	e.offline_to_user = ule.offline_to_user;
	e.notify = ule.notify;
	if (e.group() != tr("All"))
		e.setGroup(ule.Group);
	else
		e.setGroup("");
	e.description = ule.description;
	e.email = ule.email;
	e.anonymous = ule.anonymous;
	e.ip = ule.ip;
	e.port = ule.port;
	append(e);
	emit userAdded(e);
	emit modified();
}

void UserList::addAnonymous(uin_t uin)
{
	QString tmp = QString::number(uin);
	UserListElement e;
	e.first_name = "";
	e.last_name = "";
	e.nickname = tmp;
	e.altnick = tmp;
	e.mobile = "";
	e.uin = uin;
	e.setGroup("");
	e.description = "";
	e.email = "";
	e.anonymous = true;
	addUser(e);
}

void UserList::changeUserInfo(const QString &oldaltnick, UserListElement &ule)
{
	UserListElement &e = byAltNick(oldaltnick);
	e.first_name = ule.first_name;
	e.last_name = ule.last_name;
	e.nickname = ule.nickname;
	e.altnick = ule.altnick;
	e.mobile = ule.mobile;
	e.email = ule.email;
	e.uin = ule.uin;
	e.anonymous = false;
	e.status = ule.status;
	e.image_size = ule.image_size;
	e.blocking = ule.blocking;
	e.offline_to_user = ule.offline_to_user;
	e.notify = ule.notify;
	if (e.group() != tr("All"))
		e.setGroup(ule.Group);
	else
		e.setGroup("");
	if (ule.altnick != oldaltnick) {
		UserBox::all_renameUser(oldaltnick, ule.altnick);
		UserBox::all_refresh();			
		}
	emit modified();
}

void UserList::changeUserStatus(const uin_t uin, const unsigned int status)
{
	UserListElement &e = byUin(uin);
	if (status != e.status) {
		emit changingStatus(uin, e.status, status);
		e.status = status;
//		UserBox::all_refresh();			
		emit statusModified(&e);
		}
}

void UserList::removeUser(const QString &altnick)
{
	for (Iterator i = begin(); i != end(); i++)
		if((*i).altnick == altnick)
		{ 
			remove(i);
			emit modified();
			break;
		};
};

bool UserList::writeToFile(QString filename)
{
	QString faname;
	QString tmp;

	tmp = ggPath("");
	mkdir(tmp.local8Bit(), 0700);

	if (!filename.length()) {
		filename = ggPath("userlist");
		}

	faname = ggPath("userattribs");

	kdebug("UserList::writeToFile(): %s\n", (const char *)filename.local8Bit());

	QFile f(filename);

	if (!f.open(IO_WriteOnly)) {
		kdebug("UserList::writeToFile(): Error opening file :(\n");
		return false;
		}

	QString s;
	QCString str;
	for (Iterator i = begin(); i != end(); i++) {
		s.truncate(0);
		s.append((*i).first_name);
		s.append(QString(";"));
		s.append((*i).last_name);
		s.append(QString(";"));
		s.append((*i).nickname);
		s.append(QString(";"));
		s.append((*i).altnick);
		s.append(QString(";"));
		s.append((*i).mobile);
		s.append(QString(";"));
		s.append((*i).group());
		s.append(QString(";"));
		if ((*i).uin)
			s.append(QString::number((*i).uin));
		s.append(QString(";"));
		s.append((*i).email);
		s.append(QString("\r\n"));
		
		if (!(*i).anonymous) {
			kdebug(s.local8Bit());
			str = QTextCodec::codecForName("ISO 8859-2")->fromUnicode(s);
			f.writeBlock(str, str.length());
			}
		}
	f.close();

	QFile fa(faname);

	if (!fa.open(IO_WriteOnly)) {
		kdebug("UserList::writeToFile(): Error opening file :(\n");
		return false;
		}

	for (Iterator i = begin(); i != end(); i++) {
		s.truncate(0);
		s.append(QString::number((*i).uin));
		s.append(QString(";"));
		s.append((*i).blocking ? QString("true") : QString("false"));
		s.append(QString(";"));
		s.append((*i).offline_to_user ? QString("true") : QString("false"));
		s.append(QString(";"));
		s.append((*i).notify ? QString("true") : QString("false"));
		s.append(QString("\r\n"));
		
		if (!(*i).anonymous && (*i).uin) {
			kdebug(s.local8Bit());
			str = QTextCodec::codecForName("ISO 8859-2")->fromUnicode(s);
			fa.writeBlock(str, str.length());
			}
		}
	fa.close();
		
	return true;
}

bool UserList::readFromFile()
{
	QString path;
	QValueList<QStringList> ualist;
	QString line;
	UserListElement e;
	bool ok;

	path = ggPath("userattribs");
	kdebug("UserList::readFromFile(): Opening userattribs file: %s\n",
		(const char *)path.local8Bit());
	QFile fa(path);
	if (!fa.open(IO_ReadOnly)) {
		kdebug("UserList::readFromFile(): Error opening userattribs file\n");
		}
	else {
		QTextStream s(&fa);
		while ((line = s.readLine()).length()) {
			QStringList slist;
			slist = QStringList::split(';', line);
			if (slist.count() == 4)
				ualist.append(slist);
			}
		fa.close();
		}

	path = ggPath("userlist");
	kdebug("UserList::readFromFile(): Opening userlist file: %s\n",
		(const char *)path.local8Bit());
	QFile f(path);
	if (!f.open(IO_ReadOnly)) {
		kdebug("UserList::readFromFile(): Error opening userlist file");
		return false;
		}

	kdebug("UserList::readFromFile(): File opened successfuly\n");

	clear();

	QTextStream t(&f);
	t.setCodec(QTextCodec::codecForName("ISO 8859-2"));
	while ((line = t.readLine()).length()) {
		if (line[0] == '#')
			continue;

		if (line.find(';') < 0) {
			QString nickname = line.section(' ',0,0);
			QString uin = line.section(' ',1,1);
			if (uin == "")
				continue;
			e.first_name = "";
			e.last_name = "";
			e.nickname = nickname;
			e.altnick = nickname;
			e.mobile = "";
			e.uin = uin.toUInt(&ok);
			if (!ok)
				e.uin = 0;
			e.setGroup("");
			e.description = "";
			e.email = "";
			e.blocking = false;
			e.offline_to_user = false;
			e.notify = true;
			addUser(e);
			}
		else {	    
			e.first_name = line.section(';', 0, 0);
			e.last_name = line.section(';', 1, 1);
			e.nickname = line.section(';', 2, 2);
			e.altnick = line.section(';', 3, 3);
			e.mobile = line.section(';', 4, 4);
			e.setGroup(line.section(';', 5, 5));
			e.uin = line.section(';', 6, 6).toUInt(&ok);
			if (!ok)
				e.uin = 0;
			e.email = line.section(';', 7, 7);

			if (e.altnick == "") {
				if (e.nickname == "")
					e.altnick = e.first_name;
				else
					e.altnick = e.nickname;
				}

			QValueList<QStringList>::Iterator i = ualist.begin();
			while ((*i)[0].toUInt() != e.uin && i != ualist.end())
				i++;
			if (i != ualist.end()) {
				e.blocking = ((*i)[1] == "true");
				e.offline_to_user = ((*i)[2] == "true");
				e.notify = ((*i)[3] == "true");
				}
			else {
				e.blocking = false;
				e.offline_to_user = false;
				e.notify = true;
				}

			e.description = "";
			addUser(e);
			}
		}

	f.close();
	emit modified();
    	return true;
}

UserList& UserList::operator=(const UserList& userlist)
{
	QValueList<UserListElement>::operator=(userlist);
	for (Iterator i = begin(); i != end(); i++)
		(*i).Parent = this;
	emit modified();
	return *this;
}

void UserList::merge(UserList &userlist) {
	UserListElement e(this);

	for (Iterator i = userlist.begin(); i != userlist.end(); i++) {
		Iterator j;
		if ((*i).uin) {
			j = begin();
			while (j != end() && (*j).uin != (*i).uin)
				j++;
			}
		else {
			j = begin();
			while (j != end() && (*j).mobile != (*i).mobile)
				j++;
			}
		if (j != end()) {
			(*j).first_name = (*i).first_name;
			(*j).last_name = (*i).last_name;
			(*j).nickname = (*i).nickname;
			(*j).altnick = (*i).altnick;
			if ((*i).uin)
				(*j).mobile = (*i).mobile;
			else
				(*j).uin = (*i).uin;
//			(*j).status = (*i).status;
//			(*j).image_size = (*i).image_size;
			(*j).setGroup((*i).group());
//			(*j).description = (*i).description;
			(*j).email = (*i).email;
			}
		else {
			e.first_name = (*i).first_name;
			e.last_name = (*i).last_name;
			e.nickname = (*i).nickname;
			e.altnick = (*i).altnick;
			e.mobile = (*i).mobile;
			e.uin = (*i).uin;
			e.status = (*i).status;
			e.image_size = (*i).image_size;
			e.blocking = (*i).blocking;
			e.offline_to_user = (*i).offline_to_user;
			e.notify = (*i).notify;
			e.setGroup((*i).group());
			e.description = (*i).description;
			e.email = (*i).email;
			e.anonymous = (*i).anonymous;
			e.port = (*i).port;
			append(e);
			}
		}
	emit modified();
}

UserList userlist;
