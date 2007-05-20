#ifndef GADU_H
#define GADU_H

#include <qhostaddress.h>
#include <qvaluelist.h>

#include "libgadu.h"
#include "protocol.h"
#include "status.h"
#include "usergroup.h"
#include "userlist.h"

typedef uin_t UinType;

class QTimer;
class GaduSocketNotifiers;

// ------------------------------------
//            UinsList
// ------------------------------------

class UinsList : public QValueList<UinType>
{
	public:
		/**
			konstruuje obiekt UinsList
		**/
		UinsList();

		/**
			konstruuje obiekt UinsList, inicjuj�c go uinem
		**/
		UinsList(UinType uin);

		/**
			konstruuje obiekt UinsList na podstawie �a�cucha "uins" sk�adaj�cego
			si� z oddzielonych przecinkami Uin�w
		**/
		UinsList(const QString &uins);

		/**
			konstruuje obiekt UinsList na podstawie "list"
			przekszta�acaj�c ka�dy element do typy UinType
		**/
		UinsList(const QStringList &list);

		bool equals(const UinsList &uins) const;
		void sort();
		QStringList toStringList() const;
};

// ------------------------------------
//             UserStatus
// ------------------------------------

class GaduStatus : public UserStatus
{
//	Q_OBJECT

	public:
		GaduStatus();
		virtual ~GaduStatus();

		GaduStatus &operator = (const UserStatus &copyMe);

		virtual QPixmap pixmap(eUserStatus status, bool has_desc, bool mobile) const;
		virtual QString pixmapName(eUserStatus status, bool has_desc, bool mobile) const;

		int toStatusNumber() const;
		static int toStatusNumber(eUserStatus status, bool has_desc);

		void fromStatusNumber(int statusNumber, const QString &description);

		virtual UserStatus *copy() const;
		virtual QString protocolName() const;
};

// ------------------------------------
//              Search
// ------------------------------------

struct SearchResult
{
	QString Uin;
	QString First;
	QString Last;
	QString Nick;
	QString Born;
	QString City;
	QString FamilyName;
	QString FamilyCity;
	int Gender;
	GaduStatus Stat;

	SearchResult();
	SearchResult(const SearchResult &);
	void setData(const char *uin, const char *first, const char *last, const char *nick, const char *born,
		const char *city, const char *familyName, const char *familyCity, const char *gender, const char *status);
};

typedef QValueList<SearchResult> SearchResults;

struct SearchRecord
{
	int Seq;
	int FromUin;
	QString Uin;
	QString FirstName;
	QString LastName;
	QString NickName;
	QString City;
	QString BirthYearFrom;
	QString BirthYearTo;
	int Gender;
	bool Active;
	bool IgnoreResults;

	SearchRecord();
	virtual ~SearchRecord();

	void reqUin(const QString& uin);
	void reqFirstName(const QString& firstName);
	void reqLastName(const QString& lastName);
	void reqNickName(const QString& nickName);
	void reqCity(const QString& city);
	void reqBirthYear(const QString& birthYearFrom, const QString& birthYearTo);
	void reqGender(bool female);
	void reqActive();

	void clearData();
};


enum GaduError
{
	ConnectionServerNotFound,
	ConnectionCannotConnect,
	ConnectionNeedEmail,
	ConnectionInvalidData,
	ConnectionCannotRead,
	ConnectionCannotWrite,
	ConnectionIncorrectPassword,
	ConnectionTlsError,
	ConnectionIntruderError,
	ConnectionUnavailableError,
	ConnectionUnknow,
	ConnectionTimeout,
	Disconnected
};


// ------------------------------------
//            GaduProtocol
// ------------------------------------

/**
	Klasa do obs�ugi protoko�u Gadu-Gadu
**/
class GaduProtocol : public Protocol
{
	Q_OBJECT

	private:

		/**
			@enum Mode
			@short Tryb pracy obiektu
			@see TokenSocketNotifiers
			@see gotToken
			@see needTokenValue
			@see register
			@see doRegister
			@see unregister
			@see doUnregister
			@see remindPassword
			@see doRemindPassword
			@see changePassword
			@see doChangePassword

			Kilka operacji w protokole Gadu-Gadu wymaga od klienta wys�ania do serwera, opr�cz
			standardowych pakiet�w, tak zwanego 'tokena'. Najpierw pobierany jest z serwera identyfikator
			tokena oraz obrazek. Nast�pnie z obrazka odczytywana jest warto�c tokena (w za�o�eniach
			mia�o to zapobiega� rejestrowaniu nowych u�ytkownik�w przez automaty.

			Sloty register, unregister, remindPassword i changePassword inicjuj� pobieranie tokena
			i ustalaj� warto�� pola Mode. Pobieranie obrazka realizowane jest przez klas�
			TokenSocketNotifiers. Po pobraniu wywo�ywany jest slot gotToken, kt�ry na podstawie warto�ci
			pola Mode wywo�uje jedn� z funkcji doRegister, doUnregister, doRemindPassword i doChangePassword.
		**/
		enum
		{
			/** Rejestrowanie nowego u�ytkownika **/
			Register,
			/** Wyrejestrowywanie istniej�cego u�ytkownika **/
			Unregister,
			/** Przypominanie has�a **/
			RemindPassword,
			/** Zmienianie has�a **/
			ChangePassword
		} Mode;

		/** Identyfikator u�ytkownika **/
		UinType DataUin;
		/** e-mail u�ytkownika **/
		QString DataEmail;
		/** stare has�o u�ytkownika **/
		QString DataPassword;
		/** nowe has�o u�ytkownika **/
		QString DataNewPassword;
		/** identyfikator tokena **/
		QString TokenId;
		/** warto�� tokena **/
		QString TokenValue;

		/** Serwery, z kt�rymi �aczy si� obiekt. **/
		static QValueList<QHostAddress> ConfigServers;
		/** Numer serwera, do kt�rego obiekt ostatnio pr�bowa� si� pod��czy�. **/
		unsigned int ServerNr;
		/** Adres serwera, do kt�rego obiekt jest pod��czony. **/
		QHostAddress ActiveServer;

		/** IP serwera, do kt�rego uda�o si� pod��czy� ostatnim razem **/
		QHostAddress lastServerIP;

		/** port serwera, do kt�rego uda�o si� pod��czy� ostatnim razem **/
		int lastServerPort;

		/** czy w procedurze ��czenia mamy korzysta� z informacji o IP/portu ostatniego serwera? **/
		bool useLastServer;

		int lastTriedServerPort;

		/** Parametry logowania - wymagane przez bibliotek� libgadu **/
		struct gg_login_params LoginParams;
		/** Sesja po��czenia - wymagane przez bibliotek� libgadu **/
		gg_session* Sess;

		/** liczba ��da� obrazk�w wys�anych w ci�gu ostatniej minuty**/
		unsigned int sendImageRequests;

		/** czy jeste�my w trakcie ��czenia si� z serwerem **/
		bool whileConnecting;

		QHostAddress DccExternalIP;

		/**
			Klasa gniazdek ��cz�ca si� z serwerem. Wysy�a sygna�y po wyst�pieniu zdarzenia protoko�u
			(po��czenie, zerwanie po��czenia, nowa wiadomo��).

			@see GaduSocketNotifiers
		**/
		GaduSocketNotifiers *SocketNotifiers;

		/**
			Zegar pinguj�cy serwer.
		**/
		QTimer* PingTimer;

		QTimer* SendUserListTimer;

		/**
			Zmienna ustawiana w zale�no�ci od tego, czy wysy�amy list� kontakt�w na serwer
			czy te� usuwamy j� z tego serwera. Zako�czenie obydwu tych czynno�ci wywo�uje
			sygna� podpi�ty do slotu userListReplyReceived, kt�ry w zale�no�ci od warto�ci
			tego pola wywo�uje userListCleared albo userListExported.

			@see userListReplyReceived
			@see userListCleared
			@see userListExported
		**/
		bool UserListClear;

		/**
			Lista u�ytkownik�w pobrana z serwera w postaci �a�cucha. Warto�� ustalana w slocie
			userListReplyReceived.

			@see userListReplyReceived
		**/
		QString ImportReply;

		/**
			Ustawianie parametr�w po��czenia proxy. Metoda wywo�ywana podczas logowania.

			@see login
		**/
		void setupProxy();

		/**
			Za pomoc� klasy TokenSocketNotifiers metoda pobiera z serwera GaduGadu token wraz
			z identyfikatorem. Pobrany token jest obs�ugiwany za pomoc� slota gotToken,
			kt�ry pobiera warto�� tokena emituj�c sygna� needTokenValue i nast�pnie wywo�uj�c
			jedn� z metod  doRegisterAccount, doUnregisterAccount(), doRemindPassword(),
			doChangePassword() na podstawie warto�ci pola Mode.

			@see TokenSocketNotifiers
			@see gotToken
			@see doRegisterAccount
			@see doUnregisterAccount
			@see doRemindPassword
			@see doChangePassword
			@see Mode
		**/
		void getToken();

		/**
			Rejestruje nowe konto. Wywo�ywane przez gotToken (kt�re jest wywo�ane po�rednio przez
			registerAccount). Korzysta z pomocy PubdirSocketNotifiers oraz slotu registerDone,
			kt�ry emituje sygna� registered.

			@see registerAccount
			@see registered
			@see registerDone
			@see gotToken
		**/
		void doRegisterAccount();

		/**
			Wyrejestrowuje konto. Wywo�ywane przez gotToken (kt�re jest wywo�ane po�rednio przez
			unregisterAccount). Korzysta z pomocy PubdirSocketNotifiers oraz slotu unregisterDone,
			kt�ry emituje sygna� unregistered.

			@see unregisterAccount
			@see unregistered
			@see unregisterDone
			@see gotToken
		**/
		void doUnregisterAccount();

		/**
			Przypomina has�o. Wywo�ywane przez gotToken (kt�re jest wywo�ane po�rednio przez
			remindPassword). Korzysta z pomocy PubdirSocketNotifiers oraz slotu remindDone,
			kt�ry emituje sygna� reminded.

			@see remindPassword
			@see reminded
			@see remindDone
			@see gotToken
		**/
		void doRemindPassword();

		/**
			Zmienia has�o. Wywo�ywane przez gotToken (kt�re jest wywo�ane po�rednio przez
			changePassword). Korzysta z pomocy PubdirSocketNotifiers oraz slotu changePasswordDone,
			kt�ry emituje sygna� passwordChanged.

			@see changePassword
			@see passwordChanged
			@see changePasswordDone
			@see gotToken
		**/
		void doChangePassword();
		GaduProtocol(const GaduProtocol &) : Protocol(QString::null, QString::null) {}
		GaduProtocol &operator=(const GaduProtocol &){return *this;}

	private slots:

		/**
			Loguje si� do serwera Gadu-Gadu. Po uruchomieniu emituje sygna� connecting. Parametry
			logowania odczytuje z konfiguracji, status logowania pobiera z pola NextStatus.
			��cz�c si�, wybiera kolejne serwery (w przypadku nieudanego po��czenia) wykorzystuj�c
			pola ConfigServers i i ServerNr.

			Po poprawnym zalogowaniu wywo�ywany jest slot connectedSlot, kt�ry emituje sygna�
			GaduProtocol::connected

			Metod� mo�na uruchomi� po�rednio poprzez wywo�anie typu gadu->status().setOnline(),
			kt�re wywo�a slot iWantToGoOnline, kt�ry z kolei (gdy stwierdzi, �e nie jeste�my zalogowani)
			wywo�a procedur�.

			@see connecting
			@see connected
			@see connectedSlot
			@see NextStatus
			@see ConfigServers
			@see ServerNr
		**/
		void login();

		/**
			Wywo�ywany po zarejestrowaniu konta. Emituje registered/

			@see registerAccount
			@see doRegisterAccount
			@see registered
		**/
		void registerDone(bool ok, struct gg_http *);

		/**
			Wywo�ywany po wyrejestrowaniu konta. Emituje unregistered.

			@see unregisterAccount
			@see doUnregisterAccount
			@see unregistered
		**/
		void unregisterDone(bool ok, struct gg_http *);

		/**
			Wywo�ywany po przypomnieniu has�a. Emituje reminded.

			@see remindPassword
			@see doRemindPassword
			@see reminded
		**/
		void remindDone(bool ok, struct gg_http *);

		/**
			Wywo�ywany po zmianie has�a. Emituje passwordChanged.

			@see changePassword
			@see doChangePassword
			@see passwordChanged
		**/
		void changePasswordDone(bool ok, struct gg_http *);

		/**
			Slot wywo�ywany, gdy pobieranie tokena si� nie uda�o.

			@see getToken
		**/
		void tokenError();
		/**
			Slot wywo�ywany, gdy pobieranie tokena si� powiod�o. Emituje needTokenValue

			@see getToken
			@see needTokenValue
		**/
		void gotToken(QString, QPixmap);

		/**
			Slot wywo�ywany po po��czeniu z serwerem. Emituje connected i w��cza pingowanie
			serwera.

			@see connected
		**/
		void connectedSlot();

		/**
			Slot wywo�ywany po roz��czeniu z serwerem. Emituje disconnected i wy��cza pingowanie
			serwera.

			@see disconnected
		**/
		void disconnectedSlot();

		/**
			Slot wywo�ywany po przekroczeniu czasu po��czenia. Pr�buje po�aczy� ponownie.
		**/
		void connectionTimeoutTimerSlot();

		/**
			Slot wywo�ywane po wyst�pieniu b��du po��czenia. Emituje disconnected i error.

			@see error
			@see disconnected
		**/
		void errorSlot(GaduError);

		/**
			Slot wywo�ywany po otrzymaniu obrazka od serwera. Emituje imageReceivedAndSaved

			@see imageReceivedAndSaved
		**/
		void imageReceived(UinType sender, uint32_t size, uint32_t crc32,
			const QString &filename, const char *data);

		/**
			Slot wywo�ywany po otrzymaniu pro�by o obrazek od serwera. Wysy�a obrazek.
		**/
		void imageRequestReceivedSlot(UinType, uint32_t, uint32_t);

		/**
			Slot wywo�ywany po otrzymaniu wiadomo�ci od serwera.
		**/
		void messageReceived(int, UserListElements, QCString& msg, time_t, QByteArray &formats);

		/**
			Wykonuje zadania co minut� - pinguje sie� i zeruje licznik
			odebranych obrazk�w (je�li jeste�my po��czeni).
		**/
		void everyMinuteActions();

		/**
			Obs�uguje otrzymanie nowych wynik�w wyszukiwania z serwera. Emituje newSearchResults.

			@see newSearchResults
		**/
		void newResults(gg_pubdir50_t res);

		/**
			Nowa wiadomo�� od serwera. Emituje systemMessageReceived

			@see systemMessageReceived
		**/
		void systemMessageReceived(QString &, QDateTime &, int, void *);

		/**
			Pobrano list� u�ytkownik�w z serwera. Emituje userStatusChanged dla ka�dego
			otrzymanego kontaktu oraz userListChanged przed zako�czeniem dzia�ania.

			@see userStatusChanged
			@see userListChanged
		**/
		void userListReceived(const struct gg_event *);

		/**
			Odpowied� od serwera na temat operacji na li�cie u�ytkownik�w. Emituje, w zale�no�ci
			od trybu dzia�ania: userListCleared, userListExported, userListImported.

			@see userListCleared
			@see userListExported
			@see userListImported
		**/
		void userListReplyReceived(char, char *);

		/**
			Informacja o zmianie statusu kontaktu. Emituje userStatusChanged oraz userListChanged.

			@see userStatusChanged
			@see userListChanged
		**/
		void userStatusChanged(const struct gg_event *);

		/**
			Kto� wykona� gadu.status().setOnline(). ��czymy z serwerem, je�eli jeszcze tego nie
			zrobili�my, i zmieniamy status.

			@see CurrentStatus
			@see NextStatus
			@see login
		**/
		void iWantGoOnline(const QString &);

		/**
			Kto� wykona� gadu.status().setBusy(). ��czymy z serwerem, je�eli jeszcze tego nie
			zrobili�my, i zmieniamy status.

			@see CurrentStatus
			@see NextStatus
			@see login
		**/
		void iWantGoBusy(const QString &);

		/**
			Kto� wykona� gadu.status().setInvisible(). ��czymy z serwerem, je�eli jeszcze tego nie
			zrobili�my, i zmieniamy status.

			@see CurrentStatus
			@see NextStatus
			@see login
		**/
		void iWantGoInvisible(const QString &);

		/**
			Kto� wykona� gadu.status().setOffline(). Roz��czamy si� z serwerem i ustawiamy opis (je�eli
			byli�my po��czeni).

			@see CurrentStatus
			@see NextStatus
			@see logout
		**/
		void iWantGoOffline(const QString &);

		/**
			Przysz�a informacja o dostarczeniu (lub nie) wiadomo�ci.
			Na podstawie statusu emituje odpowiednie sygna�y message*

			@see messageBlocked
			@see messageDelivered
			@see messageQueued
			@see messageBoxFull
			@see messageNotDelivered
			@see messageAccepted
			@see messageRejected
		**/
		void ackReceived(int seq, uin_t uin, int status);

		void currentStatusChanged(const UserStatus &status, const UserStatus &oldStatus);

	public:
		static void initModule();
		static void closeModule();

		GaduProtocol(const QString &id, QObject *parent = NULL, const char *name = NULL);
		virtual ~GaduProtocol();

		void changeID(const QString &id);
		/**
			Zwraca serwer z kt�rym jeste�my po��czeni lub do kt�rego si� w�a�nie ��czymy.
			isNull() = hub.
		**/
		QHostAddress activeServer();

		/**
			Konwertuje list� u�ytkownik�w do postaci �a�cucha.

			�a�cuch wynikowy ma posta�:
			<code>
				opis_u�ytkownika<CR><LF>
				opis_u�ytkownika<CR><LF>
				...
				opis_u�ytkownika<CR><LF>
			</code>

			opis_u�ytkownika ma posta�:
			<code>
				firstName;lastName;nickName;altNick;mobile;grupy;uin;email;0;;0;
			</code>

			grupy maj� posta�:
			<code>
				grupa_1;grupa_2;grupa_3
			</code>

			@param userList lista u�ytkownik�w, kt�ra zostanie skonwertowana
			@return �a�cuch reprezentuj�cy list� u�ytkownik�w
			@see stringToUserList
			@see streamToUserList
		**/
		QString userListToString(const UserList &userList) const;

		/**
			Konwertuj� �a�cuch do listy u�ytkownik�w.

			Format �a�cucha jest anologiczny do tego z funkcji userListToString.

			@param source �a�uch, b�d�cy reprezentacj� listy u�ytkownik�w
			@see userListToString
			@see streamToUserList
		**/
		QValueList<UserListElement> stringToUserList(const QString &source) const;

		/**
			Odczytuje ze strumienia �a�cuch reprezentuj�cy list� u�ytkownik�w i konwertuje
			go go postaci obiektu UserList.

			Format �a�cucha jest anologiczny do tego z funkcji userListToString.

			@param source strumie�, z kt�rego odczytane zostan� dane
			@see userListToString
			@see stringToUserList
		**/
		QValueList<UserListElement> streamToUserList(QTextStream &source) const;

		/**
			Po jedno sekundowym op�nieniu wykonuje pr�b� po��czenia.
		**/
		void connectAfterOneSecond();

		/**
			Ustawia adres IP kt�ry ma by� przekazany do serwera GG
			jako adres komputera, kt�ry przekierowuje do nas port DCC.
			Zmiana jest uwzgl�dniana dopiero przy ponownym po��czeniu
			z serwerem.
		**/
		void setDccExternalIP(const QHostAddress& ip);

		virtual UserStatus *newStatus() const;
	public slots:
		/**
			Wysy�a wiadomo�� bez formatowania tekstu. Je�li adresat�w jest wi�cej ni� jeden, to wysy�ana
			jest wiadomo�� konferencyjna. Zwracany jest numer sekwencyjny wiadomo�ci, je�li
			przypadkiem by�my chcieli �ledzi� jej potwierdzenie.

			@param users lista u�ytkownik�w, do kt�rych wysy�amy wiadomo��
			@param msg wiadomo��, kt�r� wysy�amy - musi by� podana w postaci cp1250
			@todo zmieni� na sendMessage(const UinsList &, QString &) z wewn�trzn� konwersj� na cp1250
		**/
		int sendMessage(UserListElements users, const char *msg);
		/**
			Wysy�a wiadomo�� bez formatowania tekstu. Zwracany jest numer sekwencyjny wiadomo�ci, je�li
			przypadkiem by�my chcieli �ledzi� jej potwierdzenie.

			@param user u�ytkownik�, do kt�rygo wysy�amy wiadomo��
			@param msg wiadomo��, kt�r� wysy�amy - musi by� podana w postaci cp1250
			@todo zmieni� na sendMessage(const UinsList &, QString &) z wewn�trzn� konwersj� na cp1250
		**/
		int sendMessage(UserListElement user, const char *msg);

		/**
			Wysy�a wiadomo�� z formatowaniem tekstu. Je�li adresat�w jest wi�cej ni� jeden, to wysy�ana
			jest wiadomo�� konferencyjna. Zwracany jest numer sekwencyjny wiadomo�ci, je�li
			przypadkiem by�my chcieli �ledzi� jej potwierdzenie.

			@param users lista u�ytkownik�w, do kt�rych wysy�amy wiadomo��
			@param msg wiadomo��, kt�r� wysy�amy - musi by� podana w postaci cp1250
			@param myLastFormats formatowanie tekstu
			@param myLastFormatsLength ilo�c znacznik�w formatuj�cych

			@todo zmieni� na sendMessageRichText(const UinsList &, QString &, ...)
				z wewn�trzn� konwersj� na cp1250 oraz z jakim� lepszym sposobem formatowania tekstu
		**/
		int sendMessageRichText(UserListElements users, const char *msg, unsigned char *myLastFormats,
		 	unsigned int myLastFormatsLength);

		/**
			Wysy�a pro�b� o przys�anie obrazka z danymi parametrami.

			@param user u�ytkownik, od kt�rego chcemy obrazek
			@param size rozmiar obrazka w bajtach
			@param crc32 crc32 pliku
			@todo powinno by� sendImageRequest(uniqId uint32_t) - info o obrazku zapisywa� gdzie� w �rodku
		**/
		bool sendImageRequest(UserListElement user, int size, uint32_t crc32);

		/**
			Wywy�a obrazek o podanych parametrach.

			@param user u�ytkownik, kt�remu wysy�amy obrazek
			@param file_name nazwa pliku obrazka
			@param size rozmiar obrazka w bajtach
			@param data zawarto�� pliku
			@todo usun�� parametry size i data - mo�emy to chyba sami wyznaczy�
		**/
		bool sendImage(UserListElement user, const QString &file_name, uint32_t size, const char *data);

		/**
			Rejetrujemy nowe konto. Odpowied� przychodzi poprzez sygna� registered. Mo�e
			zosta� tak�e wywo�any sygna� needTokenValue.

			@param mail nasz email, pole nieobowi�zkowe
			@param password nasze has�o, pole obowi�zkowe
			@see registered
			@see needTokenValue
			@see unregisterAccount
		**/
		void registerAccount(const QString &mail, const QString &password);

		/**
			Wyrejestrowujemy stare konto. Odpowied� przychodzi poprzez sygna� unregistered. Mo�e
			zosta� tak�e wywo�any sygna� needTokenValue.

			@param uin nasz uin
			@param password nasze has�o
			@todo parametr uin naprawd� potrzebny?
		**/
		void unregisterAccount(UinType uin, const QString &password);

		/**
			Wysy�a has�o na email. Odpowied� przychodzi poprzez sygna� reminded. Mo�e
			zosta� tak�e wywo�any sygna� needTokenValue.

			@param uin nasz uin
			@param email nasz email (musi by� taki sam jak podczas rejestracji)
			@todo parametr uin naprawd� potrzebny?
		**/
		void remindPassword(UinType uin, const QString& mail);

		/**
			Zmienia nasze has�o. Odpowied� przychodzi poprzez sygna� passwordChanged. Mo�e
			zosta� tak�e wywo�any sygna� needTokenValue.

			@param uin nasz uin
			@param mail nasz email, jaki podali�my przy rejestracji
			@param password stare has�o
			@param newPassword nowe has�o
			@todo parametr uin naprawd� potrzebny?
		**/
		void changePassword(UinType uin, const QString &mail, const QString &password,
			const QString &newPassword);

		/**
			Wysy�a list� u�ytkownik�w na serwer. Odpowied� przychodzi przez sygna� userListExported.

			@return false, je�li operacja si� nie powiod�a
			@param userList lista do wys�ania
			@see userListExported
			@todo usun�� warto�� zwracan�
		**/
		bool doExportUserList(const UserList &userList);

		/**
			Usuwa list� u�ytkownik�w z serwera. Odpowied� przychodzi przez sygna� userListCleared.

			@return false, je�li operacja si� nie powiod�a
			@see userListCleared
			@todo usun�� warto�� zwracan�
		**/
		bool doClearUserList();

		/**
			Importuje list� u�ytkownik�w z serwera. Odpowied� przychodzi przez sygna� userListImported.

			@return false, je�li operacja si� nie powiod�a
			@see userListImported
			@todo usun�� warto�� zwracan�
		**/
		bool doImportUserList();

		/**
			Wysy�a nasz� list� u�ytkownik�w na serwer. Uwaga: nie ma to nic wsp�lnego z importem/eksportem.
		**/
		void sendUserList();

		void sendUserListLater();

		/**
			Szuka ludzi w katalogu publicznym. Wyniki przychodz� za pomoca sygna�u newSearchResults.

			@param searchRecord dane do szukania
			@see newSearchResults
			@see searchNextInPubdir
		**/
		void searchInPubdir(SearchRecord& searchRecord);

		/**
			Szuka ludzi w katalogu publicznym. Wyniki przychodz� za pomoca sygna�u newSearchResults.

			@param searchRecord dane do szukania
			@see newSearchResults
			@see searchInPubdir
		**/
		void searchNextInPubdir(SearchRecord& searchRecord);

		/**
			Zatrzymuje wyszukiwanie.

			@param searchRecord dane u�yte w wyszukiwaniu
			@see searchInPubdir
		**/
		void stopSearchInPubdir(SearchRecord& searchRecord);

		/**
			Pobiera informacje o danych osobowych z katalogu publicznego.

			@todo jak to w og�le dzia�a, bo zapomnia�em??
		**/
		void getPersonalInfo(SearchRecord& searchRecord);

		/**
			Ustawia informacje o danych osobowych w katalogu publicznym.

			@todo jak to w og�le dzia�a, bo zapomnia�em??
		**/
		void setPersonalInfo(SearchRecord& searchRecord, SearchResult& newData);

		/**
			 To raczej chwilowo.

			 @todo usun��
			 @todo niech Adrian zrobi porzadek z tym dcc...
		**/
		void freeEvent(struct gg_event* e);

		// --------------------
		//  DCC
		// --------------------

		void dccRequest(UinType);
		void dccFree(struct gg_dcc* d);
		struct gg_event* dccWatchFd(struct gg_dcc* d);
		void dccSetType(struct gg_dcc* d, int type);
		int dccFillFileInfo(struct gg_dcc* d, const QString& filename);
		void dccSocketCreate(UinType uin, uint16_t port, struct gg_dcc **out);
		void dccSendFile(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin, struct gg_dcc **out);
		void dccGetFile(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin, struct gg_dcc **out);
		void dccVoiceChat(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin, struct gg_dcc **out);
		int dccVoiceSend(struct gg_dcc* d, char* buf, int length);
		void setDccIpAndPort(unsigned long dcc_ip, int dcc_port);

	protected slots:
		/* sloty pod��czane do sygna��w z klasy UserList */
		void protocolUserDataChanged(QString protocolName, UserListElement elem,
							QString name, QVariant oldValue, QVariant currentValue,
							bool massively, bool last);
		void userDataChanged(UserListElement elem, QString name, QVariant oldValue,
							QVariant currentValue, bool massively, bool last);
		void userAdded(UserListElement elem, bool massively, bool last);

		void removingUser(UserListElement elem, bool massively, bool last);

		void protocolAdded(UserListElement elem, QString protocolName, bool massively, bool last);

		void removingProtocol(UserListElement elem, QString protocolName, bool massively, bool last);

	signals:
		/**
			wiadomo�� zosta�a zablokowana przez serwer
		**/
		void messageBlocked(int seq, UinType uin);

		/**
			wiadomo�� dostarczono
		**/
		void messageDelivered(int seq, UinType uin);

		/**
			wiadomo�� zakolejkowano
		**/
		void messageQueued(int seq, UinType uin);

		/**
			skrzynka odbiorcza na serwerze jest pe�na
		**/
		void messageBoxFull(int seq, UinType uin);

		/**
			wiadomo�� nie dostaczona (wyst�puje tylko przy CTCP)
		**/
		void messageNotDelivered(int seq, UinType uin);

		/**
			wiadomo�� zosta�a przyj�ta przez serwer
		**/
		void messageAccepted(int seq, UinType uin);
		/**
			wiadomo�� zosta�a odrzucona przez serwer
		**/
		void messageRejected(int seq, UinType uin);

		/**
			wyst�pi� b��d po��czenia
			@param err przyczyna
			@see connectionError
		**/
		void error(GaduError err);

		/**
			dostali�my pro�b� o przys�anie obrazka
			@param sender od kogo
			@param size rozmiar pliku
			@param crc32 jego suma kontrolna obliczana przy pomocy crc32
		**/
		void imageRequestReceived(UinType sender, uint32_t size, uint32_t crc32);

		/**
			Otrzymano dane obrazka i zapisano go do pliku.
			@param sender od kogo
			@param size rozmiar pliku
			@param crc32 jego suma kontrolna obliczana przy pomocy crc32
			@param path �cie�ka do zapisanego pliku
		**/
		void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path);

		/**
			dostali�my od serwera informacj� o zmianie statusu dla kontaktu,
			kt�rego nie mamy na li�cie
		**/
		void userStatusChangeIgnored(UinType uin);

		/**
			otrzymana wiadomo�� systemow�
			@param message tre�� wiadomo�ci wraz z dat�
		**/
		void systemMessageReceived(const QString &message);

		/**
			Otrzymano wiadomo�� CTCP.
			Kto� nas prosi o po��czenie dcc, poniewa�
			jeste�my za NAT-em
			@param user wywo�uj�cy
			@todo zmieni� nazw�
		**/
		void dccConnectionReceived(const UserListElement &user);

		/**
			otrzymano nowe wyniki wyszukiwania w katalogu publicznym
			@param searchResults wyniki
			@param seq numer sekwencyjny
			@param lastUin ?
		**/
		void newSearchResults(SearchResults& searchResults, int seq, int lastUin);

		/**
			operacja rejestracji nowego konta zosta�a zako�czona
			@param ok powodzenie operacji
			@param uin nowy numer
			@see doRegisterAccount
		**/
		void registered(bool ok, UinType uin);

		/**
			operacja wyrejestrowania konta zosta�a zako�czona
			@param ok powodzenie operacji
			@see doUnregisterAccount
		**/
		void unregistered(bool ok);

		/**
			operacja przypomnienia has�a zosta�a zako�czona
			@param ok powodzenie operacji
			@see doRemindPassword
		**/
		void reminded(bool ok);

		/**
			operacja zmiany has�a zosta�a zako�czona
			@param ok powodzenie operacji
			@see doChangePassword
		**/
		void passwordChanged(bool ok);

		/**
			operacja eksportu listy kontakt�w na serwer zosta�a zako�czona
			@param ok powodzenie operacji
			@see doExportUserList
		**/
		void userListExported(bool ok);

		/**
			operacja usuni�cia listy kontakt�w z serwera zosta�a zako�czona
			@param ok powodzenie operacji
			@see doClearUserList
		**/
		void userListCleared(bool ok);

		/**
			operacja importu listy kontakt�w z serwera zosta�a zako�czona
			@param ok powodzenie operacji
			@param list je�eli operacja si� powiod�a, to zaimportowana lista
			@see doImportUserList
		**/
		void userListImported(bool ok, QValueList<UserListElement> list);

		/**
			Sygna� daje mozliwo�� operowania na wiadomo�ci
			kt�ra przysz�a z serwera jeszcze w jej oryginalnej
			formie przed konwersj� na unicode i innymi zabiegami.
			Tre�� wiadomo�ci mo�na zmieni� grzebi�c w buforze msg,
			ale uwaga: mo�na zepsu� formatowanie tekstu zapisane
			w formats. Oczywi�cie je r�wnie� mo�na zmienia� wed�ug
			opisu protoko�u GG ;)
			Mo�na te� przerwa� dalsz� obr�bk� wiadomo�ci ustawiaj�c
			stop na true.
		**/
		void messageFiltering(Protocol *protocol, UserListElements senders,
								QCString &msg, QByteArray &formats, bool &stop);
		/**
			Otrzymano wiadomo�� kt�r� trzeba pokaza� (klasa chat lub msg,
			nadawca nie jest ignorowany, itp)
			Tre�� zdeszyfrowana i zdekodowana do unicode.
			Je�li natomiast zmienna grab zostanie ustawiona przez slot
			chatMsgReceived0, to �adna czynno�� zwi�zana z obs�ug� tego
			zdarzenia nie zostanie podj�ta (tj. wy�wietlanie wiadomo�ci
			w oknie, dodanie jej do historii, etc.), poza przekonwertowaniem
			kodowania wiadomo�ci z CP1250 na Unicode.
		**/
		void chatMsgReceived0(Protocol *protocol, UserListElements senders, const QString& msg, time_t time, bool &grab);
		void chatMsgReceived1(Protocol *protocol, UserListElements senders, const QString& msg, time_t time, bool &grab);
		void chatMsgReceived2(Protocol *protocol, UserListElements senders, const QString& msg, time_t time, bool grabbed);

		/**
			Wywo�ywane, gdy chcemy odczyta� token z obrazka
		**/
		void needTokenValue(QPixmap in, QString &out);

};

extern GaduProtocol* gadu;

#endif
