#ifndef KADU_USERLIST_H
#define KADU_USERLIST_H

#include <qstring.h>
#include "usergroup.h"

/**
	\class UserList
	\brief Klasa reprezentuj�ca list� kontakt�w.
**/
class UserList : public UserGroup
{
	Q_OBJECT
	public:
		/**
			\fn static void initModule()
			Inicjuje modu� listy kontakt�w.
		**/
		static void initModule();

		/**
			\fn static void closeModule()
			Sprz�ta po module listy kontakt�w.
		**/
		static void closeModule();

	public slots:
		/**
			\fn bool readFromFile()
			Wczytuje list� kontakt�w z plik�w "userlist"
			i "userattribs" z katalogu z ustawieniami.
			TODO: obsolete, do wywalenia w 0.6
			\return FALSE, gdy wyst�pi�y b��dy, lub TRUE w przeciwnym wypadku.
		**/
		bool readFromFile();

		/**
			\fn void merge(const QValueList<UserListElement> &list)
			Scala list� kontakt�w z podan� list� kontakt�w \a userlist.
			\param userlist lista kontakt�w z kt�r� nale�y scali�
		**/
		void merge(const QValueList<UserListElement> &list);

		/**
			\fn void readFromConfig()
			Wczytuje list� kontakt�w z drzewa xml z konfiguracj�
			programu. Poprzednie kontakty s� usuwane.
		**/
		void readFromConfig();

		/**
			\fn void writeToConfig()
			Dodaje list� kontakt�w do drzewa xml z konfiguracj�
			programu. Poprzednie kontakty s� usuwane.
		**/
		void writeToConfig();

		/**
			\fn void setAllOffline(const QString &protocolName)
			\param protocolName identyfikator protoko�u
			Przestawia status wszystkich kontakt�w
			dla wskazanego protoko�u na "niedost�pny".

		**/
		void setAllOffline(const QString &protocolName);

		/**
			\fn void clear()
			Usuwa wszystkie kontakty.
		**/
		void clear();

		void addPerContactNonProtocolConfigEntry(const QString &attribute_name, const QString &internal_key);
		void removePerContactNonProtocolConfigEntry(const QString &attribute_name);
		void addPerContactProtocolConfigEntry(const QString &protocolName, const QString &attribute_name, const QString &internal_key);
		void removePerContactProtocolConfigEntry(const QString &protocolName, const QString &attribute_name);


	private:
		/**
			\fn UserList()
			Konstruktor standardowy.
		**/
		UserList();

		/**
			\fn ~UserList()
		**/
		virtual ~UserList();

		QMap<QString, QString> nonProtoKeys;
		QMap<QString, QMap<QString, QString> > protoKeys;

		void initKeys();
};

extern UserList *userlist;

#endif
