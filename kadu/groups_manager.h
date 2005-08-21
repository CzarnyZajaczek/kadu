#ifndef KADU_GROUPS_MANAGER_H
#define KADU_GROUPS_MANAGER_H

#include <qmap.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qtimer.h>
#include <qvariant.h>

#include "usergroup.h"
#include "userlistelement.h"

class KaduTabBar;

/**
	Klasa s�u��ca przede wszystkim zarz�dzaniu nazwanymi grupami kontakt�w,
	ale tak�e zarz�dza grupami typu: blokowani, blokuj�cy, tylko z opisami, tylko dost�pni, anonimowi
	\class GroupsManager
	\brief
**/
class GroupsManager : public QObject
{
	Q_OBJECT
	public:
		/**
			\fn static void initModule()
			inicjuje modu� zarz�dcy grup
		**/
		static void initModule();

		/**
			\fn static void closeModule()
			sprz�ta po module zarz�dcy grup
		**/
		static void closeModule();

		/**
			\fn UserGroup *group(const QString &name) const
			zwraca grup� kontakt�w o nazwie name
			\param name nazwa grupy
			\return grupa kontakt�w
		**/
		UserGroup *group(const QString &name) const;

		/**
			\fn UserGroup *addGroup(const QString &name)
			dodaje now� grup� kontakt�w o nazwie name
			\param name nazwa grupy
			\return grupa kontakt�w
		**/
		UserGroup *addGroup(const QString &name);

		/**
			\fn void removeGroup(const QString &name)
			usuwa grup� kontakt�w o nazwie name
			\param name nazwa grupy
		**/
		void removeGroup(const QString &name);

		/**
			\fn void setTabBar(KaduTabBar *bar)
			ustawia pas zak�adek, na kt�rym klasa b�dzie operowa� oraz inicjuje
			wewn�trzne dane klasy
			\param bar pas zak�adek, w kt�rym b�d� umieszczane zak�adki grup
		**/
		void setTabBar(KaduTabBar *bar);

		/**
			\fn QStringList groups() const
			\return lista grup
		**/
		QStringList groups() const;

		/**
			\fn bool groupExists(const QString &name)
			\param name nazwa grupy
			\return informacja czy grupa istnieje
		**/
		bool groupExists(const QString &name);

		/**
			\fn QString currentGroupName() const
			\return nazwa aktualnie wybranej grupy
		**/
		QString currentGroupName() const;

	public slots:

		/**
			\fn void setActiveGroup(const QString &name)
			ustawia aktywn� grup� na name
			\param name nazwa grupy
		**/
		void setActiveGroup(const QString& group);

		/**
			\fn void refreshTabBar()
			od�wie�a pasek zak�adek grup
		**/
		void refreshTabBar();

		/**
			\fn void refreshTabBarLater()
			od�wie�a pasek zak�adek grup, ale po zako�czeniu bie�acych operacji
		**/
		inline void refreshTabBarLater() { refreshTimer.start(0, true); }

		/**
			\fn void changeDisplayingBlocking()
			w��cza lub wy��cza wy�wietlanie kontakt�w blokuj�cych
			w g��wnej li�cie kontakt�w w zale�no�ci od poprzedniego stanu
		**/
		void changeDisplayingBlocking();

		/**
			\fn void changeDisplayingBlocked()
			w��cza lub wy��cza wy�wietlanie kontakt�w blokowanych
			w g��wnej li�cie kontakt�w w zale�no�ci od poprzedniego stanu
		**/
		void changeDisplayingBlocked();

		/**
			\fn void changeDisplayingOffline()
			w��cza lub wy��cza wy�wietlanie kontakt�w niedost�pnych
			w g��wnej li�cie kontakt�w w zale�no�ci od poprzedniego stanu
		**/
		void changeDisplayingOffline();

		/**
			\fn void changeDisplayingWithoutDescription()
			w��cza lub wy��cza wy�wietlanie kontakt�w o statusach bez opisu
			w g��wnej li�cie kontakt�w w zale�no�ci od poprzedniego stanu
		**/
		void changeDisplayingWithoutDescription();

	private slots:
		void userDataChanged(UserListElement elem, QString name, QVariant oldValue,
							QVariant currentValue, bool massively, bool last);
		void userAdded(UserListElement elem, bool massively, bool last);
		void userRemoved(UserListElement elem, bool massively, bool last);

		void tabSelected(int id);

		void onApplyConfigDialog();

	private:
		GroupsManager();
		~GroupsManager();
		QMap<QString, UserGroup *> Groups;
		KaduTabBar *GroupBar;
		int lastId;
		QString currentGroup;
		bool showBlocked;
		bool showBlocking;
		bool showOffline;
		bool showWithoutDescription;
		QTimer refreshTimer;
};
extern GroupsManager *groups_manager;

/**
	\class UsersWithDescription
	Klasa grupuj�ca kontakty o statusie z opisem,
	automatycznie synchronizuj�ca si� z g��wn� list� kontakt�w
**/
class UsersWithDescription : public UserGroup
{
	Q_OBJECT
	public:
		UsersWithDescription();
		virtual ~UsersWithDescription();
	private slots:
		void statusChangedSlot(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last);
};
extern UsersWithDescription *usersWithDescription;

/**
	\class OnlineUsers
	Klasa grupuj�ca kontakty o statusie "dost�pny" lub "zaj�ty",
	automatycznie synchronizuj�ca si� z g��wn� list� kontakt�w
**/
class OnlineUsers : public UserGroup
{
	Q_OBJECT
	public:
		OnlineUsers();
		virtual ~OnlineUsers();
	private slots:
		void statusChangedSlot(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last);
};
extern OnlineUsers *onlineUsers;

/**
	\class BlockedUsers
	Klasa grupuj�ca kontakty, kt�re s� blokowane,
	automatycznie synchronizuj�ca si� z g��wn� list� kontakt�w
**/
class BlockedUsers : public UserGroup
{
	Q_OBJECT
	public:
		BlockedUsers();
		virtual ~BlockedUsers();
	private slots:
		void protocolUserDataChangedSlot(QString protocolName, UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last);
};
extern BlockedUsers *blockedUsers;

/**
	\class BlockingUsers
	Klasa grupuj�ca kontakty o statusie blokuj�cym,
	automatycznie synchronizuj�ca si� z g��wn� list� kontakt�w
**/
class BlockingUsers : public UserGroup
{
	Q_OBJECT
	public:
		BlockingUsers();
		virtual ~BlockingUsers();
	private slots:
		void statusChangedSlot(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last);
};
extern BlockingUsers *blockingUsers;

/**
	\class AnonymousUsers
	Klasa grupuj�ca kontakty anonimowe,
	automatycznie synchronizuj�ca si� z g��wn� list� kontakt�w
**/
class AnonymousUsers : public UserGroup
{
	Q_OBJECT
	public:
		AnonymousUsers();
		virtual ~AnonymousUsers();
	private slots:
		void userDataChangedSlot(UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last);
		void userAdded(UserListElement elem, bool massively, bool last);
		void removingUser(UserListElement elem, bool massively, bool last);
};
extern AnonymousUsers *anonymousUsers;

#endif
