#ifndef GADU_H
#define GADU_H

#include <qhostaddress.h>
#include <qobject.h>
#include <qstring.h>
#include <qtimer.h>
#include <qvaluelist.h>
#include <qdatetime.h>
#include <qsocketnotifier.h>
#include <qtextstream.h>
#include <qlistbox.h>

#include "libgadu.h"
#include "status.h"

typedef uin_t UinType;

class UserList;
class UserListElement;

extern QHostAddress config_extip;

// ------------------------------------
//              Timers
// ------------------------------------

class AutoConnectionTimer : private QTimer {
	Q_OBJECT

	public:
		static void on();
		static void off();

	public slots:
		void doConnect();

	private:
		AutoConnectionTimer(QObject *parent = 0, const char *name=0);

		static AutoConnectionTimer *autoconnection_object;
};

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
	Q_OBJECT

	public:
		GaduStatus();
		virtual ~GaduStatus();

		void operator = (const UserStatus &copyMe);

		virtual QPixmap pixmap(eUserStatus status, bool has_desc, bool mobile) const;

		int toStatusNumber() const;
		static int toStatusNumber(eUserStatus status, bool has_desc);

		void fromStatusNumber(int statusNumber, const QString &description);
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

// ------------------------------------
//             Notifiers
// ------------------------------------

class SocketNotifiers : public QObject
{
	Q_OBJECT

	protected:
		int Fd;
		QSocketNotifier *Snr;
		QSocketNotifier *Snw;

		void createSocketNotifiers();
		void deleteSocketNotifiers();
		void recreateSocketNotifiers();
		virtual void socketEvent() = 0;

	protected slots:
		virtual void dataReceived() = 0;
		virtual void dataSent() = 0;

	public:
		SocketNotifiers(int fd, QObject *parent=0, const char *name=0);
		virtual ~SocketNotifiers();
		virtual void start();
		virtual void stop();
};

class PubdirSocketNotifiers : public SocketNotifiers
{
	Q_OBJECT

	private:
		struct gg_http *H;

	protected:
		virtual void socketEvent();

	protected slots:
		virtual void dataReceived();
		virtual void dataSent();

	public:
		PubdirSocketNotifiers(struct gg_http *, QObject *parent=0, const char *name=0);
		virtual ~PubdirSocketNotifiers();

	signals:
		void done(bool ok, struct gg_http *);
};

class TokenSocketNotifiers : public SocketNotifiers
{
	Q_OBJECT

	private:
		struct gg_http *H;

	protected:
		virtual void socketEvent();

	protected slots:
		virtual void dataReceived();
		virtual void dataSent();

	public:
		TokenSocketNotifiers(QObject *parent=0, const char *name=0);
		virtual ~TokenSocketNotifiers();

		virtual void start();

	signals:
		void gotToken(QString, QPixmap);
		void tokenError();
};

typedef enum
{
	ConnectionServerNotFound,
	ConnectionCannotConnect,
	ConnectionNeedEmail,
	ConnectionInvalidData,
	ConnectionCannotRead,
	ConnectionCannotWrite,
	ConnectionIncorrectPassword,
	ConnectionTlsError,
	ConnectionUnknow,
	ConnectionTimeout,
	Disconnected
} GaduError;

class GaduSocketNotifiers : public SocketNotifiers
{
	Q_OBJECT

	private:
		gg_session *Sess;

		void connectionFailed(int);

	protected:
		virtual void socketEvent();

	public slots:
		virtual void dataReceived();
		virtual void dataSent();

	public:
		GaduSocketNotifiers(QObject *parent=0, const char *name=0);
		virtual ~GaduSocketNotifiers();
		void setSession(gg_session *sess);
		void checkWrite();

	signals:
		void ackReceived(int seq, uin_t uin, int status);
		void connected();
		/**
			Otrzymano wiadomo�� CTCP.
			Kto� nas prosi o po��czenie dcc, poniewa�
			jeste�my za NAT-em.
			TODO: zmieni� nazw�.
		**/
		void dccConnectionReceived(const UserListElement &);
		void disconnected();
		void error(GaduError);
		void imageReceived(UinType, uint32_t, uint32_t, const QString &filename, const char *data);
		void imageRequestReceived(UinType, uint32_t, uint32_t);
		void messageReceived(int, UinsList, QCString &, time_t, QByteArray &);
		void pubdirReplyReceived(gg_pubdir50_t);
		void systemMessageReceived(QString &, QDateTime &, int, void *);
		void userlistReceived(const struct gg_event *);
		void userlistReplyReceived(char, char *);
		void userStatusChanged(const struct gg_event *);
};

// ------------------------------------
//            GaduProtocol
// ------------------------------------

/**
	@class GaduProtocol
	@short Klasa do obs�ugi protoko�u Gadu-Gadu
 **/
class GaduProtocol : public QObject
{
	Q_OBJECT

	friend class AutoConnectionTimer;

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

			Kilka operacja w protokole Gadu-Gadu wymaga od klienta wys�ania do serwera, opr�cz
			standardowych pakiet�w, tak zwanego 'tokena'. Najpierw pobierany jest z serwera identyfikator
			tokena oraz obrazek. Nast�pnie z obrazka odczytywana jest warto�c tokena (w za�o�eniach
			mia�o to zapobiega� rejestrowaniu nowych u�ytkownik�w przez automaty.

			Sloty register, unregister, remindPassword i changePassword inicjuj� pobieranie tokena
			i ustalaj� warto�� pola Mode. Pobieranie obrazka realizowane jest przez klas�
			TokenSocketNotifiers. Po pobraniu wywo�ywany jest slot gotToken, kt�ry na podstawie warto�ci
			pola Mode wywo�uje jedn� z funkjci doRegister, doUnregister, doRemindPassword i doChangePassword.
		 **/
		enum
		{
			//! Rejestrowanie nowego u�ytkownika
			Register,
			//! Wyrejestrowywanie istniej�cego u�ytkownika
			Unregister,
			//! Przypominanie has�a
			RemindPassword,
			//! Zmienianie has�a
			ChangePassword
		} Mode;

		//! Identyfikator u�ytkownika
		UinType DataUin;
		//! e-mail u�ytkownika
		QString DataEmail;
		//! stare has�o u�ytkownika
		QString DataPassword;
		//! nowe has�o u�ytkownika
		QString DataNewPassword;
		//! identyfikator tokena
		QString TokenId;
		//! warto�� tokena
		QString TokenValue;

		//! Serwery, z kt�rymi �aczy si� obiekt.
		static QValueList<QHostAddress> ConfigServers;
		//! Numer serwera, do kt�rego obiekt ostatnio pr�bowa� si� pod��czy�.
		unsigned int ServerNr;
		//! Adres serwera, do kt�rego obiekt jest pod��czony.
		QHostAddress* ActiveServer;

		//! Parametry logowania - wymagane przez bibliotek� libgg
		struct gg_login_params LoginParams;
		//! Sesja po��czenia - wymagane przez bibliotek� libgg
		gg_session* Sess;

		/**
			Bie�acy status. Zmieniany po po��czeniu, oraz w przypadku zmiany statusu kiedy po��czenie
			jest ju� zainicjowane.

			@see login
			@see connected
			@see iWantGoOnline
			@see iWantGoBusy
			@see iWantGoInvisible
			@see iWantGoOffline
			@see NextStatus
			@see UserStatus
			@see status
		**/
		GaduStatus* CurrentStatus;

		/**
			Nast�pny status. Ustalany zewn�trznie przy wykorzystaniu metody status i odpowiednich
			slot�w klasy UserStatus. Zmiana wywo�uje jedn� z metod iWantGo... i w konsekwencji zmian�
			statusu (w razie konieczno�ci te� zalogowanie).

			@see login
			@see connected
			@see iWantGoOnline
			@see iWantGoBusy
			@see iWantGoInvisible
			@see iWantGoOffline
			@see CurrentStatus
			@see UserStatus
			@see status
		 **/
		GaduStatus* NextStatus;

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

		/**
			Okre�la, czy lista u�ytkownik�w zosta�a ju� wys�ana.

			@todo Wywali�, zamieni� na connected
		 **/
		bool UserListSent;
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
			Lista u�ytkownik�w pobrana z serwera w postaci �a�cucha. Warto�c ustalana w slocie
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
			Loguje si� do serwera Gadu-Gadu. Po uruchomieniu emituje sygna� connecting. Parametry
			logowania odczytuje z konfiguracji, status logowania pobiera z pola NextStatus.
			��cz�c si�, wybiera kolejne serwery (w przypadku nieudanego po��czenia) wykorzystuj�c
			pola ConfigServers i i ServerNr.

			Po poprawnym zalogowaniu wywo�ywany jest slot connectedSlot, kt�ry emituje sygna�
			GaduProtocol::connected

			Metod� mo�na uruchomi� po�rednio poprzez wywo�anie typu gadu->status().setOnline(),
			kt�re wywo�a slot iWantToGoOnline, kt�ry z kolei (gdy stwierdzi, �e nie jeste�my zalogowani)
			wywo�a procedur�.

			Metoda jest te� wywo�ywana przez obiekt statyczny klasy AutoConnectionTimer, kt�ra
			jest zaprzyja�niona z GaduProtocol.

			@see connecting
			@see connected
			@see connectedSlot
			@see NextStatus
			@see ConfigServers
			@see ServerNr
			@see AutoConnectionTimer
		 **/
		void login();
		/**
			Metoda wywo�ywana w razie roz��czenie si� z serwerem. Wywo�ywana przez iWantGoOffline
			albo przez connectionTimeoutTimerSlot. Wywo�uje disconnectedSlot, kt�ry z kolei
			emituje sygna� disconnected

			@see disconnected
			@see disconnectedSlot
			@see connectionTimeoutTimerSlot
			@see iWantGoOffline
			@todo Wywali� i zast�pi� wywo�aniami disconnectedSlot?
		 **/
		void logout();

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

	private slots:
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
		void imageRequestReceived(UinType, uint32_t, uint32_t);
		/**
			Slot wywo�ywany po otrzymaniu wiadomo�ci od serwera.
		 **/
		void messageReceived(int, UinsList, QCString& msg, time_t, QByteArray &formats);
		/**
			Co pewien czas pinguje serwer.
		 **/
		void pingNetwork();
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
		
	public:
		static void initModule();
		GaduProtocol(QObject *parent=NULL, const char *name=NULL);
		virtual ~GaduProtocol();

		/**
			Status u�ytkownika. Za pomoc� tej metody mo�emy go zmieni�, pobra� ikon� statusu i wykona�
			kilka innych ciekawych rzeczy.

			1. Zmiena statusu:
			<code>
				GaduProtocol gadu;

				...

				gadu.status().setOnline("Jestem zalogowany"); // zalogowanie i ustawienie opisu
				gadu.status().setFriendsOnly(true);           // tryb tylko dla przyjaci�
				...
				gadu.status().setOffline();                   // wylogowanie, usuni�cie opisu
			</code>

			2. Sprawdzenie statusu:
			<code>
				GaduProtocol gadu;

				if (gadu.status().isOnline())                 // jeste�my online
					...
				else if (gadu.status().isInvisible())         // jeste�my niewidzialni
					...

				// mo�na te�:
				switch (gadu.status().status())
				{
					case Online:
						break;
					case Busy:
						break;
					case Invisible:
						break;
					case Offline:
						break;
				}
			</code>

			3. Pobranie ikony i nazwy statusu
			<code>
				QPixmap pix;
				QString name;
				GaduProtocol gadu;

				...

				pix = gadu.status().pixmap();
				name = gadu.status().name();
			</code>

			@see currentStatus
		 **/
		UserStatus & status();

		/**
			Rzeczywisty aktualny status. Mo�na go wykorzysta� tylko w trybie do odczytu (pobranie
			ikony, nazwy, sprawdzenie rzeczywistego stanu po�aczenia).

			@see status
		 **/
		const UserStatus & currentStatus();

		/**
			Zwraca serwer z kt�rym jeste�my po��czeni lub do kt�rego si� w�a�nie ��czymy.
			NULL = hub.
		**/
		QHostAddress* activeServer();

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

			grup maj� posta�:
			<code>
				grupa_1,grupa_2,grupa_3
			</code>

			@param userList lista u�ytkownik�w, kt�ra zostanie skonwertowana
			@return �a�cuch reprezentuj�cy list� u�ytkownik�w
			@see stringToUserList
			@see streamToUserList
		 **/
		QString userListToString(const UserList &userList) const;
		/**
			Konwertuj� �a�cuch do listy u�ytkownik�w.

			Format �a�cucha jest anologiczny do tego z funkcji userListToString. Jedynym wyj�tkiem
			jest to, �e grupy oddzielone s� �rednikami, nie przecinkami.

			@param source �a�uch, b�d�cy reprezentacj� listy u�ytkownik�w
			@param userList lista u�ytkownik�w, do kt�rej zapisany zostanie wynik konwersji
			@see userListToString
			@see streamToUserList
		 **/
		void stringToUserList(QString &source, UserList &userList) const;
		/**
			Odczytuje ze strumienia �a�cuch reprezentuj�cy list� u�ytkownik�w i konwertuje
			go go postaci obiektu UserList.

			Format �a�cucha jest anologiczny do tego z funkcji userListToString. Jedynym wyj�tkiem
			jest to, �e grupy oddzielone s� �rednikami, nie przecinkami.

			@param source strumie�, z kt�rego odczytane zostan� dane
			@param userList lista u�ytkownik�w, do kt�rej zapisany zostanie wynik konwersji
			@see userListToString
			@see stringToUserList
		 **/
		void streamToUserList(QTextStream &source, UserList &userList) const;

		/**
			W��cza pr�by automatycznego ��czenia w razie niepowodzenia.
		 **/
		void enableAutoConnection();
		/**
			Wy��cza pr�by automatycznego ��czenia w razie niepowodzenia.
		 **/
		void disableAutoConnection();

		/**
			Zwraca true, je�eli jeste�my po��czenie z serwerem.

			@todo zmieni� nazw� na connected
		 **/
		bool userListSent();

	public slots:
		/**
			Wysy�a wiadomo�� bez formatowania tekstu. Je�li adresat�w jest wi�cej ni� jeden, to wysy�ana
			jest wiadomo�� konferencyjna. Zwracany jest numer sekwencyjny wiadomo�ci, je�li
			przypadkiem by�my chcieli �ledzi� jej potwierdzenie.

			@param uins lista u�ytkownik�w, do kt�rych wysy�amy wiadomo��
			@param msg wiadomo��, kt�r� wysy�amy - musi by� podana w postaci cp1250
			@toto zmieni� na sendMessage(const UinsList &, QString &) z wewn�trzn� konwersj� na cp1250
		**/
		int sendMessage(const UinsList &uins, const char *msg);

		/**
			Wysy�a wiadomo�� z formatowaniem tekstu. Je�li adresat�w jest wi�cej ni� jeden, to wysy�ana
			jest wiadomo�� konferencyjna. Zwracany jest numer sekwencyjny wiadomo�ci, je�li
			przypadkiem by�my chcieli �ledzi� jej potwierdzenie.

			@param uins lista u�ytkownik�w, do kt�rych wysy�amy wiadomo��
			@param msg wiadomo��, kt�r� wysy�amy - musi by� podana w postaci cp1250
			@param myLastFormats formatowanie tekstu
			@param myLastFormatsLength ilo�c znacznik�w formatuj�cych

			@toto zmieni� na sendMessageRichText(const UinsList &, QString &, ...)
				z wewn�trzn� konwersj� na cp1250 oraz z jakim� lepszym sposobem formatowania tekstu
		**/
		int sendMessageRichText(const UinsList &uins, const char *msg, unsigned char *myLastFormats,
		 	int myLastFormatsLength);

		/**
			Wysy�a pro�b� o przys�anie obrazka z danymi parametrami.

			@param uin u�ytkownik, od kt�rego chcemy obrazek
			@param size rozmiar obrazka w bajtach
			@param crc32 crc32 pliku
			@todo powinno by� sendImageRequest(uniqId uint32_t) - info o obrazku zapisywa� gdzie� w �rodku
		 **/
		bool sendImageRequest(UinType uin, int size, uint32_t crc32);
		/**
			Wywy�a obrazek o podanych parametrach.

			@param uin u�ytkownik, kt�remu wysy�amy obrazek
			@param file_name nazwa pliku obrazka
			@param size rozmiar obrazka w bajtach
			@param data zawarto�� pliku
			@toto usun�� parametry size i data - mo�emy to chyba sami wyznaczy�
		 **/
		bool sendImage(UinType uin, const QString &file_name, uint32_t size, char *data);

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
			@toto parametr uin naprawd� potrzebny?
		 **/
		void unregisterAccount(UinType uin, const QString &password);
		/**
			Wysy�a has�o na email. Odpowied� przychodzi poprzez sygna� reminded. Mo�e
			zosta� tak�e wywo�any sygna� needTokenValue.

			@param uin nasz uin
			@toto parametr uin naprawd� potrzebny?
		 **/
		void remindPassword(UinType uin);
		/**
			Zmienia nasze has�o. Odpowied� przychodzi poprzez sygna� passwordChanged. Mo�e
			zosta� tak�e wywo�any sygna� needTokenValue.

			@param uin nasz uin
			@param mail nasz email, jaki podali�my przy rejestracji
			@param password stare has�o
			@param newPassword nowe has�o
			@toto parametr uin naprawd� potrzebny?
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
			Pobiera informacje o danych odobowych z katalogu publicznego.

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
		struct gg_dcc* dccSocketCreate(UinType uin, uint16_t port);
		struct gg_dcc* dccSendFile(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin);
		struct gg_dcc* dccGetFile(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin);
		struct gg_dcc* dccVoiceChat(uint32_t ip, uint16_t port, UinType my_uin, UinType peer_uin);
		int dccVoiceSend(struct gg_dcc* d, char* buf, int length);
		void setDccIpAndPort(unsigned long dcc_ip, int dcc_port);

		// przeniesione z events.h
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
		void ifDefServerEnabled(bool value);
		void useTlsEnabled(bool value);

		/**
			Sygna� wywo�ywany, gdy zmieni�y si� dane kontaktu. Umo�liwia poinformowanie serwera
			o zmianie naszych zalece� co do obs�ugi wiadomo�ci od kontaktu.

			Przyk�ady:
			<code>
				GaduProtocol gadu;
				UserListElement old, new;

				...
				gadu.userDataChanged(NULL, &new);              // dodali�my nowy kontakt

				old = new;
				new.setBlockng(true);                          // blokujemy
				gadu.userDataChanged(&old, &new);              // informujemy o tym serwer

				gadu.userDataChanged(new, NULL);               // usun�li�my kontakt
				...
			</code>

			Powy�sze kontrukcje nie s� jednak na og� potrzebne, gdy� obiekty UserListElement
			wywo�uj� odpowieni sygna� swoich UserList (konkretnie userDataChanged), kt�ry mo�na
			podpi�� bezpo�rednio pod taki sam slot klasy GaduProtocol, dzi�ki czemu ka�da
			zmiana zostanie automatycznie zauwa�ona przez serwer.

			<code>
				GaduProtocol gadu;
				UserListElement *ule;
				UserList ul;

				...                                            // po�aczenie odpowiednich slot�w
				ule = &ul.byAltNick("AltNick");
				ule->setBlocking(true);                        // i wszystko robi si� samo
			</code>

			@param oldData wska�nik do starych danych kontatku
			@param newData wska�nik do nowych danych konaktu
		 **/
		void userDataChanged(const UserListElement* const oldData, const UserListElement* const newData);

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
			otrzymano informacj� o potwierdzeniu wiadomo�ci
			
			wkr�tce ten sygna� zostanie usuni�ty
			prosz� u�ywa� messageAccepted() i messageRejected() lub szczeg�owych odpowiednik�w
		**/
		void ackReceived(int seq);
		
		void connected();
		void connecting();
		void disconnected();
		void error(GaduError);
		/**
			Otrzymano dane obrazka i zapisano go do pliku.
		**/
		void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path);
		void userListChanged();
		void userStatusChanged(const UserListElement &, const UserStatus &oldStatus, bool onConnection = false);
		void systemMessageReceived(QString &);
		void dccConnectionReceived(const UserListElement&);
		void disconnectNetwork();
		void newSearchResults(SearchResults& searchResults, int seq, int lastUin);
		void registered(bool ok, UinType uin);
		void unregistered(bool ok);
		void reminded(bool ok);
		void passwordChanged(bool ok);
		void userListExported(bool ok);
		void userListCleared(bool ok);
		void userListImported(bool ok, UserList&);

		/**
			Sygnal daje mozliwosc operowania na wiadomoci
			ktora przyszla z serwera jeszcze w jej oryginalnej
			formie przed konwersja na unicode i innymi zabiegami.
			Tresc wiadomosci mozna zmienic grzebiac w buforze msg,
			ale uwaga: mona zepsu�formatowanie tekstu zapisane
			w formats. Oczywicie je r�nie mona zmienia� wedug
			opisu protokou GG ;)
			Mozna tez przerwac dalsza obrobke wiadomoci ustawiajac
			stop na true.
		**/
		void messageFiltering(const UinsList& senders,QCString& msg,
			QByteArray& formats,bool& stop);
		/**
			Otrzymano wiadomo�� kt�r� trzeba pokaza� (klasa chat lub msg,
			nadawca nie jest ignorowany, itp)
			Tre�� zdeszyfrowana i zdekodowana do unicode.
			Je�li kt�ry� ze slot�w sygna�u chatMsgReceived1 ustawi zmienn�
			grab na true to sygna� chatReceived2 nie zostanie wygenerowany.
			Je�li natomiast zmienna grab zostanie ustawiona przez slot
			chatMsgReceived0, to �adna czynno�� zwi�zana z obs�ug� tego
			zdarzenia nie zostanie podj�ta (tj. wy�wietlanie wiadomo�ci
			w oknie, dodanie jej do historii, etc.), poza przekonwertowaniem
			kodowania wiadomo�ci z CP1250 na Unicode.
		**/
		void chatMsgReceived0(UinsList senders,const QString& msg,time_t time,bool& grab);
		void chatMsgReceived1(UinsList senders,const QString& msg,time_t time,bool& grab);
		void chatMsgReceived2(UinsList senders,const QString& msg,time_t time);

		/**
			Wywo�ywane, gdy chcemy odczyta� token z obrazka
		**/
		void needTokenValue(QPixmap, QString &);

};

extern GaduProtocol* gadu;

#endif
