#ifndef USERLIST_H
#define USERLIST_H

#include <qobject.h>
#include <qvaluelist.h>
#include <qtimer.h>
#include <qhostaddress.h>
#include <qdns.h>
#include <qptrlist.h>

#include "gadu.h"

class UserList;
class UserListElement;

class DnsHandler : public QObject
{
	Q_OBJECT

	UserListElement &Ule;

	public:
		DnsHandler(UserListElement &ule);
		~DnsHandler();

		static int counter;

	private:
		QDns DnsResolver;

	private slots:
		void resultsReady();
};

struct UserListElement
{
	private:
		QString Group;
		QString FirstName;
		QString LastName;
		QString NickName;
		QString AltNick;
		QString Mobile;
		QString Email;
		UinType Uin;
		UserStatus *Stat;
		int MaxImageSize;
		bool Anonymous;
		QHostAddress Ip;
		QString DnsName;
		short Port;
		int Version;
		bool Blocking;
		bool OfflineTo;
		bool Notify;

		UserList *Parent;
		friend class UserList;

	public:
		UserListElement(UserList* parent);
		UserListElement(const UserListElement &copyMe);
		UserListElement();
		virtual ~UserListElement();

		void refreshDnsName();

		void operator = (const UserListElement &copyMe);

		QString group() const;
		void setGroup(const QString& group);

		QString firstName() const;
		void setFirstName(const QString &firstName);

		QString lastName() const;
		void setLastName(const QString &lastName);

		QString nickName() const;
		void setNickName(const QString &nickName);

		QString altNick() const;
		void setAltNick(const QString &altNick);

		QString mobile() const;
		void setMobile(const QString &mobile);

		QString email() const;
		void setEmail(const QString &email);

		UinType uin() const;
		void setUin(const UinType &uin);

		const UserStatus & status() const;
		UserStatus & status();

		int maxImageSize() const;
		void setMaxImageSize(const int maxImageSize);

		bool isAnonymous() const;
		void setAnonymous(const bool anonymous);

		const QHostAddress & ip() const;
		QHostAddress & ip();

		QString dnsName() const;
		void setDnsName(const QString &dnsName);

		short port() const;
		void setPort(short port);

		int version() const;
		void setVersion(const int version);

		bool blocking() const;
		void setBlocking(const bool blocking);

		bool offlineTo() const;
		void setOfflineTo(const bool offlineTo);

		bool notify() const;
		void setNotify(const bool notify);
};

/**
	Klasa reprezentuj�ca list� kontakt�w.
	indexami s� altnicki konwertowane tak,
	aby wszystkie litery by�y ma�e.
	Dzi�ki temu nie mog� istnie� dwa kontakty
	o tych samych altnickach nawet, je�li
	r�ni� si� wielko�ciami liter.
	Pole Altnick w UserListElement zawiera natomiast
	oryginalny altnick.
	
	UWAGA! U�ywanie metod klasy QMap nie jest zalecane,
	trzeba wtedy pami�ta�, �e jako klucza u�ywamy altnick�w
	odpowiednio skonwertowanych, �e zmiany powinny generowa�
	odpowiednie sygna�y, itd. Du�o lepiej u�ywa� specjalnie
	przygotowanych metod klasy UserList.
	
	TODO: Zgodnie z tym co powy�ej z QMap powinni�my dziedziczy�
	prywatnie albo niech to b�dzie pole wewn�trz klasy. W tej
	chwili nie chc� �ama� api freeze w 0.4.0.
**/
class UserList : public QObject, public QMap<QString,UserListElement>
{
	Q_OBJECT

	protected:
		friend class UserListElement;

	public:
		UserList();
		UserList(const UserList& source);
		~UserList();
		UserList& operator=(const UserList& userlist);

		UserListElement& byUin(UinType uin);
		UserListElement& byNick(const QString& nickname);
		/**
			Wyszukuje kontakt po altnicku. Ma�e i
			du�e litery nie maj� znaczenia.
		**/
		UserListElement& byAltNick(const QString& altnick);
		/**
			Wyszukuje kontakt po numerze uin.
			Jezeli nie znajdzie, zwracany jest
			UserListElement tylko z uin i altnick == uin
		**/
		UserListElement byUinValue(UinType uin);
		/**
			Sprawdza czy istnieje kontakt o danym numerze uin.
		**/
		bool containsUin(UinType uin) const;
		/**
			Sprawdza czy istnieje kontakt o danym altnicku.
			Ma�e i du�e litery nie maj� znaczenia.
		**/
		bool containsAltNick(const QString& altnick) const;

	public slots:
		/**
			Dodaje do listy kontakt o podanych danych.
			Generuje nast�pnie sygna�y userDataChanged()
			i modified().
		**/
		void addUser(const UserListElement& ule);
		/**
			Dodaje do listy "anonimowy" kontakt o podanym
			numerze uin. Numer ten wykorzystywany jest jako
			altnick. Wywo�uje metod� addUser(), wi�c
			generowane s� sygna�y userDataChanged() i modified().
		**/
		void addAnonymous(UinType uin);
		/**
			Usuwa kontakt o podanym altnicku. Ma�e i
			du�e litery nie maj� znaczenia.
			Tu� przed usuni�ciem kontaktu z listy generuje
			sygna� userDataChanged(). Po usuni�ciu generowany
			jest sygna� modified().
		**/
		void removeUser(const QString& altnick);
		/**
			Zmienia dane kontaktu o podanym altnicku. Ma�e i
			du�e litery w zmiennej old_altnick nie maj� znaczenia.
			Po wykonaniu zmian wymusza ich uwzgl�dnienie przez
			wszystkie obiekty klasy UserBox wywo�uj�c metod�
			UserBox::all_renameUser() i UserBox::all_refresh().
			Na koniec generuje sygna�y userDataChanged() i modified().
		**/		
		void changeUserInfo(const QString& old_altnick, const UserListElement& new_data);
		/**
			Zapisuje list� kontakt�w do pliku o podanej nazwie
			(wraz ze �cie�k�). Domy�lnie jest to plik "userlist"
			w katalogu z ustawieniami.
			Utrzymywana jest kompatybilno�� z programem EKG,
			z tej przyczyny dodatkowe informacje jak to czy
			blokujemy u�ytkownika, czy chcemy by� powiadamiani
			o zmianach jego statusu itp. s� zapisywane do
			oddzielnego pliku "userattribs" w katalogu z
			ustawieniami.
			Kontakty "anonimowe" s� pomijane.
			
			TODO: To gdzie zapisujemy dodatkowe atrybutami te�
			powinno by� chyba podawane jako argument? Inaczej
			je�li chcemy zapisa� list� gdzie� indziej to nadpisz�
			nam si� atrybuty :/
		**/
		bool writeToFile(QString filename = "");
		/**
			Wczytuje list� kontakt�w z plik�w "userlist"
			i "userattribs" z katalogu z ustawieniami.
			Metoda ta jest przeznaczona do przywracania
			danych zapisanych za pomoc� writeToFile().
		**/		
		bool readFromFile();
		void merge(const UserList &userlist);

	signals:
		/**
			Sygna� generowany po zako�czeniu zmian w li�cie
			kontakt�w.
		**/
		void modified();
		void dnsNameReady(UinType);
		/**
			Sygna� jest generowany je�li cokolwiek zmieni�o
			si� w danych kt�rego� z kontakt�w na li�cie
			(wtedy przekazywane s� argumenty oldData i newData).
			Zmiana mo�e nast�pi� przez bezpo�rednie u�ycie kt�rej�
			z metod klasy UserListElement.
			Sygna� przekazuje r�wnie� informacj� o tym, �e nowy
			kontakt pojawi� si� na li�cie (wtedy oldData == NULL)
			lub �e zosta� z listy usuni�ty (wtedy newData = NULL).
		**/
		void userDataChanged(const UserListElement* const oldData, const UserListElement* const newData);
};

extern UserList userlist;

#endif

