#ifndef KADU_USERLIST_H
#define KADU_USERLIST_H

#include <qdns.h>
#include <qhostaddress.h>
#include <qobject.h>

#include "gadu.h"

class UserList;
class UserListElement;

/**
	T�umaczy adres IP na nazw� domeny (DNS). Kiedy proces zostanie zako�czony,
	emituje odpowiedni sygna� zawieraj�cy informacj� o nazwie domeny.
	\class DnsHandler
	\brief Klasa t�umacz�ca adres IP na nazw� domeny.
**/
class DnsHandler : public QObject
{
	Q_OBJECT

	public:
		/**
			\fn DnsHandler(const QHostAddress &addr);
			Konstruktor wywo�uj�cy zapytanie o domen� dla danego adresu IP.
			\param addr adres IP
		**/
		DnsHandler(const QHostAddress &addr);
		~DnsHandler();

		static int counter; /*!< licznik obiekt�w tej klasy */

	private:
		QDns DnsResolver; /*!< obiekt zajmuj�cy si� translacj� adresu na nazw� domeny */

	private slots:
		/**
			\fn void resultsReady()
			Funkcja wywo�ywana, gdy proces t�umaczenia zosta� zako�czony.
		**/
		void resultsReady();

	signals:
		/**
			\fn void result(const QString &hostname)
			Sygna� emitowany, gdy proces t�umaczenia zosta� zako�czony.
			\param hostname nazwa domeny odpowiadaj�ca adresowi IP o kt�ry pytano
		**/
		void result(const QString &hostname);
};

/**
	Typ opisuj�cy stan powiadamiania.
	\enum NotifyType
	\brief Typ opisuj�cy stan powiadamiania.
**/
enum NotifyType {
	GLOBAL = 0, /*!< powiadomienia zale�ne od globalnych ustawie� */
	OFF = 1, /*!< powiadomienia wy��czone */
	OWN = 2 /*!< indywidualne ustawienie powiadamiania */
};

/**
	Klasa reprezentuj�ca kontakt (u�ytkownika na li�cie kontakt�w).
	\class UserListElement
	\brief Klasa reprezentuj�ca kontakt.
**/
class UserListElement : public QObject
{
	Q_OBJECT
	private:
		QString Group; /*!< Grupa do kt�rej przynale�y kontakt
						(je�li wi�cej ni� jedna, to oddzielone s� przecinkami). */
		QString FirstName; /*!< Imi� kontaktu. */
		QString LastName; /*!< Nazwisko kontaktu. */
		QString NickName; /*!< Pseudonim kontaktu. */
		QString AltNick; /*!< Pseudonim kontaktu, kt�ry jest wy�wietlany na li�cie. */
		QString Mobile; /*!< Numer telefonu kontaktu. */
		QString Email; /*!< E-Mail kontaktu. */
		UinType Uin; /*!< UIN kontaktu. */
		UserStatus *Stat; /*!< Aktualny status kontaktu. */
		int MaxImageSize; /*!< Maksymalny rozmiar obrazka, jak� mo�e przyj�� kontakt. */
		bool Anonymous; /*!< Informuje, czy kontakt jest anonimowy czy nie. */
		QHostAddress Ip; /*!< Adres IP kontaktu (je�li wykryty). */
		QString DnsName; /*!< Nazwa domenu kontaktu (je�li znaleziona). */
		short Port; /*!< Port kontaktu (je�li wykryty). */
		int Version; /*!< Wersja protoko�u u�ywanego przez kontakt. */
		bool Blocking; /*!< Informuje czy blokujemy kontakt, czy nie. */
		bool OfflineTo; /*!< Informuje czy mamy w��czony tryb "niedost�pny dla kontaktu" dla tego kontaktu. */
		bool Notify; /*!< Informuje czy mamy w��czone powiadamianie o kontakcie. */
		
		NotifyType AliveSound; /*!< Przechowuje informacj� o sposobie powiadamiania
								o zmianie statusu kontaku d�wi�kiem. */
		QString OwnAliveSound; /*!< Je�li spos�b powiadamiania o zmianie statusu kontaktu ma warto�� OWN,
								to ta zmienna przechowuje nazw� pliku d�wi�kowego do odtworzenia. */

		NotifyType MessageSound; /*!< Przechowuje informacj� o sposobie powiadamiania
								o nowej wiadomo�ci od kontaktu d�wi�kiem. */
		QString OwnMessageSound; /*!< Je�li spos�b powiadamiania o nowej wiadomo�ci od kontaktu ma warto�� OWN,
								to ta zmienna przechowuje nazw� pliku d�wi�kowego do odtworzenia. */
		
		QString HomePhone; /*!< Numer telefonu domowego kontaktu. */

		UserList *Parent; /*!< Wska�nik do listy kontakt�w, na kt�rej znajduje si� ten kontakt. */
		friend class UserList;
	public slots:
		/**
			\fn void setGroup(const QString& group)
			Ustawia grup� (lub grupy, oddzielone przecinkami) dla kontaktu.
			\param group grupa (grupy) do kt�rej zostanie przydzielony kontakt.
		**/
		void setGroup(const QString& group);
		
		/**
			\fn void setFirstName(const QString &firstName)
			Ustawia imi� dla kontaktu.
			\param firstName imi�, kt�re zostanie przydzielone kontaktowi.
		**/
		void setFirstName(const QString &firstName);
		
		/**
			\fn void setLastName(const QString &lastName)
			Ustawia nazwisko dla kontaktu.
			\param lastName nazwisko, kt�re zostanie przydzielone kontaktowi.
		**/
		void setLastName(const QString &lastName);
		
		/**
			\fn void setNickName(const QString &nickName)
			Ustawia pseudonim dla kontaktu.
			\param nickName pseudonim, kt�ry zostanie przydzielony kontaktowi.
		**/
		void setNickName(const QString &nickName);
		
		/**
			\fn void setAltNick(const QString &altNick)
			Ustawia wy�wietlany pseudonim dla kontaktu.
			\param altNick wy�wietlany pseudonim, kt�ry zostanie przydzielony kontaktowi.
		**/
		void setAltNick(const QString &altNick);
		
		/**
			\fn void setMobile(const QString &mobile)
			Ustawia numer telefonu dla kontaktu.
			\param mobile numer telefonu, kt�ry zostanie przydzielony kontaktowi.
		**/
		void setMobile(const QString &mobile);
		
		/**
			\fn void setEmail(const QString &email)
			Ustawia adres E-Mail dla kontaktu.
			\param email adres, kt�ry zostanie przydzielony kontaktowi.
		**/
		void setEmail(const QString &email);
		
		/**
			\fn void setUin(const UinType &uin)
			Ustawia UIN dla kontaktu.
			\param uin numer UIN, kt�ry zostanie przydzielony kontaktowi.
		**/
		void setUin(const UinType &uin);
		
		/**
			\fn void setMaxImageSize(const int maxImageSize)
			Ustawia maksymalny rozmiar obrazka dla kontaktu.
			\param maxImageSize rozmiar obrazka w bajtach, kt�ry zostanie przydzielony kontaktowi.
		**/
		void setMaxImageSize(const int maxImageSize);
		
		/**
			\fn void setAnonymous(const bool anonymous)
			Ustawia stan anonimowo�ci kontaktu.
			\param anonymous warto�� logiczna informuj�ca, �e kontakt jest anonimowy, lub nie.
		**/
		void setAnonymous(const bool anonymous);
		
		/**
			\fn void setDnsName(const QString &dnsName)
			Ustawia nazw� domeny dla kontaktu.
			\param dnsName nazwa domenu, kt�ra zostanie przydzielona kontaktowi.
		**/
		void setDnsName(const QString &dnsName);
		
		/**
			\fn void setPort(short port)
			Ustawia numer portu dla kontaktu.
			\param port numer portu, kt�ry zostanie przydzielony kontaktowi.
		**/
		void setPort(short port);
		
		/**
			\fn void setVersion(const int version)
			Ustawia wersj� protoko�u dla kontaktu.
			\param version wersja protoko�u, kt�ra zostanie przydzielona kontaktowi.
		**/
		void setVersion(const int version);
		
		/**
			\fn void setBlocking(const bool blocking)
			Ustawia stan blokowania kontaktu.
			\param blocking warto�� logiczna informuj�ca, czy kontakt ma by� blokowany.
		**/
		void setBlocking(const bool blocking);
		
		/**
			\fn void setOfflineTo(const bool offlineTo)
			Ustawia stan niedost�pno�ci dla kontaktu.
			\param offlineTo warto�� logiczna informuj�ca, czy u�ytkownik ma by� "niedost�pny dla kontaktu".
		**/
		void setOfflineTo(const bool offlineTo);
		
		/**
			\fn void setNotify(const bool notify)
			Ustawia stan powiadamiania o kontakcie.
			\param notify warto�� logiczna informuj�ca, czy u�ytkownik ma by� powiadamiany o kontakcie.
		**/
		void setNotify(const bool notify);

		/**
			\fn void setHomePhone(const QString &phone)
			Ustawia numer telefonu domowego dla kontaktu.
			\param phone numer telefonu, kt�ry zostanie przydzielony kontaktowi.
		**/
		void setHomePhone(const QString &phone);
		
		/**
			\fn void setAliveSound(NotifyType type, const QString &file="")
			Ustawia spos�b powiadamiania d�wi�kiem o zmianie statusu przez kontakt.
			\param type spos�b powiadamiania.
			\arg \c GLOBAL powiadomienia zale�ne od globalnych ustawie�.
			\arg \c OFF powiadomienia wy��czone.
			\arg \c OWN indywidualne ustawienie powiadamiania/

			\param file plik d�wi�kowy, wymagany dla indywidualnego ustawienia powiadomie�.
		**/
		void setAliveSound(NotifyType type, const QString &file="");
		
		/**
			\fn void setMessageSound(NotifyType type, const QString &file="")
			Ustawia spos�b powiadamiania d�wi�kiem o nowej wiadomo�ci od kontaktu.
			\param type spos�b powiadamiania.
			\arg \c GLOBAL powiadomienia zale�ne od globalnych ustawie�.
			\arg \c OFF powiadomienia wy��czone.
			\arg \c OWN indywidualne ustawienie powiadamiania/

			\param file plik d�wi�kowy, wymagany dla indywidualnego ustawienia powiadomie�.
		**/
		void setMessageSound(NotifyType type, const QString &file="");

	public:
		/**
			\fn UserListElement(UserList* parent)
			Konstruktor dodaj�cy kontakt do wskazanej listy kontakt�w.
			\param parent wska�nik listy kontakt�w, do kt�rej zostanie dodany kontakt.
		**/
		UserListElement(UserList* parent);

		/**
			\fn UserListElement(const UserListElement &copyMe)
			Konstruktor kopiuj�cy.
		**/
		UserListElement(const UserListElement &copyMe);

		/**
			\fn UserListElement()
			Domy�lny konstruktor tworz�cy kontakt o nie wype�nionych jeszcze danych.
		**/
		UserListElement();
		virtual ~UserListElement();

		/**
			\fn void refreshDnsName()
			Wywo�uje zapytanie o nazw� domeny dla kontaktu oraz wype�nia odpowiednie pole kontaktu,
			gdy domena zostanie odnaleziona.
		**/
		void refreshDnsName();

		/**
			\fn void operator = (const UserListElement &copyMe)
			Operator kopiuj�cy.
			\param copyMe obiekt klasy UserListElement kt�ry b�dzie skopiowany.
		**/
		void operator = (const UserListElement &copyMe);

		/**
			\fn const QString &group() const
			Zwraca grup� (lub wiele grup oddzielonych przecinkami) do kt�rej nale�y kontakt.
		**/
		const QString &group() const;
		
		/**
			\fn const QString &firstName() const
			Zwraca imi� kontaktu.
		**/
		const QString &firstName() const;
		
		/**
			\fn const QString &lastName() const
			Zwraca nazwisko kontaktu.
		**/
		const QString &lastName() const;
		
		/**
			\fn const QString &nickName() const
			Zwraca pseudonim kontaktu.
		**/
		const QString &nickName() const;
		
		/**
			\fn const QString &altNick() const
			Zwraca wy�wietlany pseudonim kontaktu.
		**/
		const QString &altNick() const;
		
		/**
			\fn const QString &mobile() const
			Zwraca numer telefonu kontaktu.
		**/
		const QString &mobile() const;
		
		/**
			\fn const QString &email() const
			Zwraca adres E-Mail kontaktu.
		**/
		const QString &email() const;
		
		/**
			\fn UinType uin() const
			Zwraca numer UIN kontaktu.
		**/
		UinType uin() const;
		
		/**
			\fn const QString &homePhone() const
			Zwraca numer telefonu domowego kontaktu.
		**/
		const QString &homePhone() const;
		
		/**
			\fn const QString &aliveSound(NotifyType &type) const
			Zwraca spos�b powiadamiania d�wi�kiem o zmianach statusu kontaktu.
		**/
		const QString &aliveSound(NotifyType &type) const;
		
		/**
			\fn const QString &messageSound(NotifyType &type) const
			Zwraca spos�b powiadamiania d�wi�kiem o wiadomo�ciach od kontaktu.
		**/
		const QString &messageSound(NotifyType &type) const;

		/**
			\fn const UserStatus & status() const
			Zwraca aktualny status kontaktu.
		**/
		const UserStatus & status() const;
		
		/**
			\overload UserStatus & status()
		**/
		UserStatus & status();

		/**
			\fn int maxImageSize() const
			Zwraca maksymalny rozmiar obrazka dla kontaktu.
		**/
		int maxImageSize() const;
		
		/**
			\fn bool isAnonymous() const
			Zwraca informacj�, czy kontakt jest anonimowy czy nie.
		**/
		bool isAnonymous() const;

		/**
			\fn const QHostAddress & ip() const
			Zwraca adres IP kontaktu (je�li wykryty).
		**/
		const QHostAddress & ip() const;
		
		/**
			\overload QHostAddress & ip()
		**/
		QHostAddress & ip();
		
		/**
			\fn const QString &dnsName() const
			Zwraca nazw� domeny kontaktu (je�li znaleziona).
		**/
		const QString &dnsName() const;
		
		/**
			\fn short port() const
			Zwraca numer portu kontaktu (je�li znaleziony).
		**/
		short port() const;
		
		/**
			\fn int version() const
			Zwraca wersj� protoko�u, jakiej u�ywa kontakt.
		**/
		int version() const;
		
		/**
			\fn bool blocking() const
			Zwraca informacj�, czy kontakt jest blokowany czy nie.
		**/
		bool blocking() const;
		
		/**
			\fn bool offlineTo() const
			Zwraca informacj�, czy u�ytkownik jest "niedost�pny dla kontaktu" czy nie.
		**/
		bool offlineTo() const;
		
		/**
			\fn bool notify() const
			Zwraca informacj�, czy u�ytkownik jest powiadamiany o kontakcie czy nie.
		**/
		bool notify() const;
};

/**
	Indeksami s� altnicki (wy�wietlane pseudonimy) konwertowane tak,
	aby wszystkie litery by�y ma�e.
	Dzi�ki temu nie mog� istnie� dwa kontakty
	o tych samych pseudonimach wy�wietlanych nawet, je�li
	r�ni� si� wielko�ciami liter.
	Pole Altnick w UserListElement zawiera natomiast
	oryginalny pseudonim wy�wietlany.
	
	\warning U�ywanie metod klasy QMap nie jest zalecane,
	trzeba wtedy pami�ta�, �e jako klucza u�ywamy pseudonim�w wy�wietlanych
	odpowiednio skonwertowanych, �e zmiany powinny generowa�
	odpowiednie sygna�y, itd. Du�o lepiej u�ywa� specjalnie
	przygotowanych metod klasy UserList.
	\class UserList
	\brief Klasa reprezentuj�ca list� kontakt�w.
	
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
		/**
			\fn UserList()
			Konstruktor standardowy.
		**/
		UserList();

		/**
			\fn UserList(const UserList& source)
			Konstruktor kopiuj�cy.
			\param source lista kontakt�w, kt�r� nale�y skopiowa�.
		**/
		UserList(const UserList& source);
		~UserList();

		/**
			\fn UserList& operator=(const UserList& userlist)
			Operator kopiuj�cy.
			\param userlist lista kontakt�w, kt�r� nale�y skopiowa�.
		**/
		UserList& operator=(const UserList& userlist);

		/**
			\fn UserListElement& byUin(UinType uin)
			Wyszukuje kontakt z numerem \a uin.
			\param uin numer UIN wg. kt�rego nast�puje wyszukiwanie.
			\return referencj� do obiektu reprezentuj�cego kontakt o podanym UIN'ie.
		**/
		UserListElement& byUin(UinType uin);
		
		/**
			\fn UserListElement& byNick(const QString& nickname);
			Wyszukuje kontakt po pseudonimie. Ma�e i
			du�e litery nie maj� znaczenia.
			\param nickname pseudonim wg. kt�rego nast�puje wyszukiwanie.
			\return referencj� do obiektu reprezentuj�cego kontakt o podanym pseudonimie.
		**/
		UserListElement& byNick(const QString& nickname);

		/**
			\fn UserListElement& byAltNick(const QString& altnick)
			Wyszukuje kontakt po wy�wietlanym pseudonimie. Ma�e i
			du�e litery nie maj� znaczenia.
			\param altnick wy�wietlany pseudonim wg. kt�rego nast�puje wyszukiwanie.
			\return referencj� do obiektu reprezentuj�cego kontakt o podanym pseudonimie wy�wietlanym.
		**/
		UserListElement& byAltNick(const QString& altnick);

		/**
			\fn UserListElement byUinValue(UinType uin)
			Wyszukuje kontakt po numerze \a uin.
			\param uin UIN wg. kt�rego nast�puje wyszukiwanie.
			\return obiekt reprezentuj�cy kontakt z podanym UIN'em. Jezeli nie znajdzie, zwracany jest UserListElement zawieraj�cy tylko UIN i pseudonim wy�wietlany (identyczny z numerem UIN).
		**/
		UserListElement byUinValue(UinType uin);

		/**
			\fn bool containsUin(UinType uin) const
			Sprawdza czy istnieje kontakt o danym numerze uin.
			\param uin sprawdzany UIN.
		**/
		bool containsUin(UinType uin) const;

		/**
			\fn bool containsAltNick(const QString& altnick) const;
			Sprawdza czy istnieje kontakt o danym pseudonimie wy�wietlanym.
			Ma�e i du�e litery nie maj� znaczenia.
			\param altnick Sprawdzany pseudonim wy�wietlany.
		**/
		bool containsAltNick(const QString& altnick) const;

	public slots:
		/**
			\fn void addUser(const UserListElement& ule)
			Dodaje do listy kontakt o podanych danych.
			Generuje nast�pnie sygna�y userDataChanged()
			i modified().
			\param ule obiekt reprezentuj�cy kontakt, kt�ry chcemy doda�.
		**/
		void addUser(const UserListElement& ule);

		/**
			\fn void addAnonymous(UinType uin)
			Dodaje do listy "anonimowy" kontakt o podanym
			numerze UIN. Numer ten wykorzystywany jest jako
			pseudonim wy�wietlany. Wywo�uje metod� addUser(), wi�c
			generowane s� sygna�y userDataChanged() i modified().
			\param uin UIN kontaktu, kt�ry chcemy doda�.
		**/
		void addAnonymous(UinType uin);

		/**
			\fn void removeUser(const QString& altnick)
			Usuwa kontakt o podanym pseudonimie wy�wietlanym. Ma�e i
			du�e litery nie maj� znaczenia.
			Tu� przed usuni�ciem kontaktu z listy generuje
			sygna� userDataChanged(). Po usuni�ciu generowany
			jest sygna� modified().
			\param altnick pseudonim wy�wietlany kontaktu, kt�ry chcemy usun��.
		**/
		void removeUser(const QString& altnick);

		/**
			\fn void changeUserInfo(const QString& old_altnick, const UserListElement& new_data)
			Zmienia dane kontaktu o podanym pseudonimie wy�wietlanym. Ma�e i
			du�e litery w zmiennej \a old_altnick nie maj� znaczenia.
			Po wykonaniu zmian wymusza ich uwzgl�dnienie przez
			wszystkie obiekty klasy UserBox wywo�uj�c metod�
			UserBox::all_renameUser() i UserBox::all_refresh().
			Na koniec generuje sygna�y userDataChanged() i modified().
			\param old_altnick dotychczasowy pseudonim wy�wietlany kontaktu.
			\param new_data obiekt reprezentuj�cy nowe dane kontaktu.
		**/
		void changeUserInfo(const QString& old_altnick, const UserListElement& new_data);

		/**
			\fn bool writeToFile(QString filename = "")
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
			\param filename plik do kt�rego zostanie zapisana lista.
			\return FALSE, gdy wyst�pi�y b��dy, lub TRUE w przeciwnym wypadku.
			
			TODO: To gdzie zapisujemy dodatkowe atrybutami te�
			powinno by� chyba podawane jako argument? Inaczej
			je�li chcemy zapisa� list� gdzie� indziej to nadpisz�
			nam si� atrybuty :/
		**/
		bool writeToFile(QString filename = "");

		/**
			\fn bool readFromFile()
			Wczytuje list� kontakt�w z plik�w "userlist"
			i "userattribs" z katalogu z ustawieniami.
			Metoda ta jest przeznaczona do przywracania
			danych zapisanych za pomoc� writeToFile().
			\return FALSE, gdy wyst�pi�y b��dy, lub TRUE w przeciwnym wypadku.
		**/
		bool readFromFile();

		/**
			\fn void merge(const UserList &userlist)
			Scala list� kontakt�w z podan� list� kontakt�w \a userlist.
			\param userlist lista kontakt�w z kt�r� nale�y scali�
		**/
		void merge(const UserList &userlist);

	signals:
		/**
			\fn void modified()
			Sygna� generowany po zako�czeniu zmian w li�cie
			kontakt�w.
		**/
		void modified();

		/**
			\fn void dnsNameReady(UinType)
			Sygna� generowany, gdy adres IP kontaktu o danym UIN'ie zostanie przet�umaczony
			na nazw� domeny.
			\param uin UIN kontaktu, dla kt�rego przygotowano nazw� domeny.
		**/
		void dnsNameReady(UinType uin);

		/**
			\fn void userDataChanged(const UserListElement* const oldData, const UserListElement* const newData, bool massively = false)
			Sygna� jest generowany je�li cokolwiek zmieni�o
			si� w danych kt�rego� z kontakt�w na li�cie
			(wtedy przekazywane s� argumenty \a oldData i \a newData).
			Zmiana mo�e nast�pi� przez bezpo�rednie u�ycie kt�rej�
			z metod klasy UserListElement.
			Sygna� przekazuje r�wnie� informacj� o tym, �e nowy
			kontakt pojawi� si� na li�cie,
			lub �e zosta� z listy usuni�ty.
			Je�eli ta zmiana jest cz�ci� czego� wi�kszego, to \a massively == true
			i emitowany jest sygna� UserList::allNewContacts.
			
			\param oldData obiekt reprezentuj�cy stare dane kontaktu. Je�li ten parametr ma warto�� NULL,
				to znaczy, �e kontakt w�a�nie zosta� dodany do listy.
			\param newData obiekt reprezentuj�cy nowe dane kontaktu. Je�li ten parametr ma warto�� NULL,
				to znaczy, �e kontakt zosta� w�a�nie usuni�ty z listy.
		**/
		void userDataChanged(const UserListElement* const oldData, const UserListElement* const newData, bool massively = false);

		/**
			\fn void allNewContacts(const UserList &ulist)
			Sygna� jest emitowany, kiedy powstaje zupe�nie nowa lista kontakt�w.
		**/
		void allNewContacts(const UserList &ulist);
};

extern UserList userlist;

#endif

