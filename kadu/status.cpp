/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <qobject.h>

#include "debug.h"
#include "gadu.h"
#include "status.h"

UserStatus::UserStatus() : QObject(NULL, 0)
{
	Stat = Offline;
	Description = "";
	FriendsOnly = false;
}

UserStatus::UserStatus(const UserStatus &copyMe) : QObject(NULL, 0)
{
	Stat = copyMe.Stat;
	Description = copyMe.Description;
	FriendsOnly = copyMe.FriendsOnly;
}

UserStatus::~UserStatus()
{
}

void UserStatus::operator = (const UserStatus &copyMe)
{
	Stat = copyMe.Stat;
	Description = copyMe.Description;
	FriendsOnly = copyMe.FriendsOnly;
}

bool UserStatus::operator == (const UserStatus &compare) const
{
	return (Stat == compare.Stat) && (Description == compare.Description);
}

bool UserStatus::operator != (const UserStatus &compare) const
{
	return (Stat != compare.Stat) || (Description != compare.Description);
}

QPixmap UserStatus::pixmap(bool mobile) const
{
	return pixmap(Stat, hasDescription(), mobile);
}

QPixmap UserStatus::pixmap(const UserStatus &stat, bool mobile) const
{
	return pixmap(stat.status(), stat.hasDescription(), mobile);
}

QPixmap UserStatus::pixmap(eUserStatus /*stat*/, bool /*desc*/, bool /*mobile*/) const
{
	static QPixmap result;
	return result;
}

eUserStatus UserStatus::status() const
{
	return Stat;
}

bool UserStatus::isOnline() const
{
	return Stat == Online;
}

bool UserStatus::isBusy() const
{
	return Stat == Busy;
}

bool UserStatus::isInvisible() const
{
	return Stat == Invisible;
}

bool UserStatus::isOffline() const
{
	return Stat == Offline;
}

bool UserStatus::isBlocking() const
{
	return Stat == Blocking;
}

bool UserStatus::isOffline(int index)
{
	return (index == 6) || (index == 7);
}

bool UserStatus::hasDescription() const
{
	return !Description.isEmpty();
}

bool UserStatus::isFriendsOnly() const
{
	return FriendsOnly;
}

QString UserStatus::description() const
{
	return Description;
}

int UserStatus::index(eUserStatus stat, bool desc)
{
	return (static_cast<int>(stat) << 1) + (desc ? 1 : 0);
}

int UserStatus::index() const
{
	return (static_cast<int>(Stat) << 1) + (Description.isEmpty() ? 0 : 1);
}

void UserStatus::setOnline(const QString& desc)
{
	if (Stat == Online && Description == desc && !Changed)
		return;

	Stat = Online;
	Description = desc;
	Changed = false;

	emit goOnline(Description);
	emit changed(*this);
}

void UserStatus::setBusy(const QString& desc)
{
	if (Stat == Busy && Description == desc && !Changed)
		return;

	Stat = Busy;
	Description = desc;
	Changed = false;

	emit goBusy(Description);
	emit changed(*this);
}

void UserStatus::setInvisible(const QString& desc)
{
	if (Stat == Invisible && Description == desc && !Changed)
		return;

	Stat = Invisible;
	Description = desc;
	Changed = false;

	emit goInvisible(Description);
	emit changed(*this);
}

void UserStatus::setOffline(const QString& desc)
{
	if (Stat == Offline && Description == desc)
		return;

	Stat = Offline;
	Description = desc;
	Changed = false;

	emit goOffline(Description);
	emit changed(*this);
}

void UserStatus::setBlocking()
{
	if (Stat == Blocking)
		return;

	Stat = Blocking;
	Description = "";
	Changed = false;

	emit goBlocking();
	emit changed(*this);
}

void UserStatus::setDescription(const QString& desc)
{
	if (Description == desc)
		return;

	Description = desc;

	switch (Stat)
	{
		case Online:
			emit goOnline(Description);
			emit changed(*this);
			break;

		case Busy:
			emit goBusy(Description);
			emit changed(*this);
			break;

		case Invisible:
			emit goInvisible(Description);
			emit changed(*this);
			break;

		case Blocking:
			emit goBlocking();
			emit changed(*this);
			break;

		case Offline:
		default:
			emit goOffline(Description);
			emit changed(*this);
			break;
	}
}

void UserStatus::setFriendsOnly(bool f)
{
	if (FriendsOnly == f)
		return;

	FriendsOnly = f;
	Changed = true;

	switch (Stat)
	{
		case Online: setOnline(Description); break;
		case Busy: setBusy(Description); break;
		case Invisible: setInvisible(Description); break;
		case Blocking: setBlocking(); break;
		case Offline:
		default:
			break;
	}
}

void UserStatus::setStatus(const UserStatus& stat)
{
	FriendsOnly = stat.FriendsOnly;
	Changed = true;

	switch (stat.Stat)
	{
		case Online: setOnline(stat.Description); break;
		case Busy: setBusy(stat.Description); break;
		case Invisible: setInvisible(stat.Description); break;
		case Blocking: setBlocking(); break;
		case Offline:
		default:
			setOffline(stat.Description);
			break;
	}
}

void UserStatus::setIndex(int index, const QString& desc)
{
	if (index % 2 == 0)
		setStatus(static_cast<eUserStatus>(index >> 1), "");
	else
		setStatus(static_cast<eUserStatus>(index >> 1), desc);
}

void UserStatus::setStatus(eUserStatus stat, const QString& desc)
{
	switch (stat)
	{
		case Online: setOnline(desc); break;
		case Busy: setBusy(desc); break;
		case Invisible: setInvisible(desc); break;
		case Blocking: setBlocking(); break;
		case Offline:
		default:
			setOffline(desc); break;
	}
}

void UserStatus::refresh()
{
	Changed = true;
	setStatus(*this);
}

eUserStatus UserStatus::fromString(const QString& stat)
{
	if (stat.contains("Online"))
		return Online;
	if (stat.contains("Busy"))
		return Busy;
	if (stat.contains("Invisible"))
		return Invisible;
	if (stat.contains("Blocking"))
		return Blocking;
	return Offline;
}

QString UserStatus::toString(eUserStatus stat, bool desc)
{
	QString res;
	switch (stat)
	{
		case Online: res.append("Online"); break;
		case Busy: res.append("Busy"); break;
		case Invisible: res.append("Invisible"); break;
		case Blocking: res.append("Blocking"); break;
		case Offline:
		default:
			res.append("Offline");
			break;
	}
	if (desc && stat!=Blocking)
		res.append("WithDescription");
	return res;
}

int UserStatus::count()
{
	return 9;
}

int UserStatus::initCount()
{
	return 7;
}

QString UserStatus::name(int nr)
{
	static const char * names[] = {
		QT_TR_NOOP("Online"),
		QT_TR_NOOP("Online (d.)"),
		QT_TR_NOOP("Busy"),
		QT_TR_NOOP("Busy (d.)"),
		QT_TR_NOOP("Invisible"),
		QT_TR_NOOP("Invisible (d.)"),
		QT_TR_NOOP("Offline"),
		QT_TR_NOOP("Offline (d.)"),
		QT_TR_NOOP("Blocking")
	};

	return names[nr];
}

QString UserStatus::name() const
{
	return name(index());
}

/* our own description container */
QStringList defaultdescriptions;
