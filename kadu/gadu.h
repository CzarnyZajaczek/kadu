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
		UinsList();
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
		SocketNotifiers(int);
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
		PubdirSocketNotifiers(struct gg_http *);
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
		TokenSocketNotifiers();
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
		GaduSocketNotifiers();
		virtual ~GaduSocketNotifiers();
		void setSession(gg_session *sess);
		void checkWrite();

	signals:
		void ackReceived(int);
		void connected();
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

		void connectedSlot();
		void disconnectedSlot();
		void connectionTimeoutTimerSlot();
		void errorSlot(GaduError);
		void imageReceived(UinType, uint32_t, uint32_t, const QString &, const char *data);
		void imageRequestReceived(UinType, uint32_t, uint32_t);
		void messageReceived(int, UinsList, QCString& msg, time_t, QByteArray &formats);
		void pingNetwork();
		void newResults(gg_pubdir50_t res);
		void systemMessageReceived(QString &, QDateTime &, int, void *);
		void userListReceived(const struct gg_event *);
		void userListReplyReceived(char, char *);
		void userStatusChanged(const struct gg_event *);

		void iWantGoOnline(const QString &);
		void iWantGoBusy(const QString &);
		void iWantGoInvisible(const QString &);
		void iWantGoOffline(const QString &);

	public:
		static void initModule();
		GaduProtocol(QObject *parent=NULL, const char *name=NULL);
		virtual ~GaduProtocol();

		UserStatus & status();
		const UserStatus & currentStatus();

		/**
			Zwraca serwer z kt�rym jeste�my po��czeni
			lub do kt�rego si� w�a�nie ��czymy.
			NULL = hub.
		**/
		QHostAddress* activeServer();

		/**
			Zamienia list� u�ytkownik�w na �a�cuch i na odwr�t
		**/
		QString userListToString(const UserList& userList) const;
		void stringToUserList(QString&, UserList& userList) const;
		void streamToUserList(QTextStream&, UserList& userList) const;

		void enableAutoConnection();
		void disableAutoConnection();

		bool userListSent();

	public slots:
		/**
			Wysyla wiadomosc. bez formatowania tekstu.
			Jesli adresatow jest wiecej niz
			jeden wysylana jest wiadomosc konferencyjna.
			Zwracany jest numer sekwencyjny wiadomosci, jesli
			przypadkiem mysli chcieli sledzic jej potwierdzenie.
		**/
		int sendMessage(const UinsList& uins,const char* msg);
		/**
			Wysyla wiadomosc z formatowaniem tekstu.
			Jesli adresatow jest wiecej niz
			jeden wysylana jest wiadomosc konferencyjna.
			Zwracany jest numer sekwencyjny wiadomosci, jesli
			przypadkiem mysli chcieli sledzic jej potwierdzenie.
		**/
		int sendMessageRichText(const UinsList& uins,const char* msg,unsigned char* myLastFormats,int myLastFormatsLength);
		/**
			Wysya prob�o przysanie obrazka.
		**/
		bool sendImageRequest(UinType uin,int size,uint32_t crc32);
		bool sendImage(UinType uin,const QString& file_name,uint32_t size,char* data);

		/**
			Zarz�dzanie kontem
		**/
		void registerAccount(const QString &mail, const QString &password);
		void unregisterAccount(UinType uin, const QString &password);
		void remindPassword(UinType uin);
		void changePassword(UinType uin, const QString &mail, const QString &password,
			const QString &newPassword);

		/**
			Wysya userlist na serwer
		**/
		bool doExportUserList(const UserList &);

		/**
			Czyscli userliste na serwerze
		**/
		bool doClearUserList();

		/**
			Importuje liste z serwera
		**/
		bool doImportUserList();

		void sendUserList();

		/**
		  	Szuka ludzi w katalogu publicznym
		**/
		void searchInPubdir(SearchRecord& searchRecord);
		void searchNextInPubdir(SearchRecord& searchRecord);

		/**
			Pobiera informacje o danych odobowych z katalogu publicznego
		**/
		void getPersonalInfo(SearchRecord& searchRecord);

		/**
			Ustawia informacje o danych osobowych z katalogu publicznego
		**/
		void setPersonalInfo(SearchRecord& searchRecord, SearchResult& newData);

		// to raczej chwilowo
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

		// userlist
		void userDataChanged(const UserListElement* const oldData, const UserListElement* const newData);

	signals:
		void ackReceived(int);
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
			Otrzymano wiadomo� kt� trzeba pokaza�(klasa chat lub msg,
			nadawca nie jest ignorowany, itp)
			Tre�zdeszyfrowana i zdekodowana do unicode.
			Jesli ktorys ze slotow sygnau chatMsgReceived1 ustawi zmienna
			grab na true to sygnal chatReceived2 nie zostanie wygenerowany.
			Jeli natomiast zmienna grab zostanie ustawiona przez slot
			chatMsgReceived0, to adna czynno�zwizana z obsug tego
			zdarzenia nie zostanie podj�a (tj. wywietlanie wiadomoci
			w oknie, dodanie jej do historii, etc.), poza przekonwertowaniem
			kodowania wiadomoci z CP1250 na Unicode.
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
