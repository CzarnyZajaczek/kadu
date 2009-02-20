/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef GADU_PROTOCOL_H
#define GADU_PROTOCOL_H

#include <QtGui/QPixmap>
#include <QtNetwork/QHostAddress>

#include <libgadu.h>

#include "services/gadu-chat-image-service.h"
#include "services/gadu-chat-service.h"
#include "services/gadu-contact-list-service.h"
#include "services/gadu-file-transfer-service.h"
#include "services/gadu-personal-info-service.h"
#include "services/gadu-search-service.h"

#include "gadu_exports.h"
#include "gadu-search-record.h"
#include "gadu-search-result.h"

#include "protocols/protocol.h"

class DccManager;
class GaduAccountData;
class GaduContactAccountData;
class GaduProtocolSocketNotifiers;

class GADUAPI GaduProtocol : public Protocol
{
	Q_OBJECT
	Q_DISABLE_COPY(GaduProtocol);

public:
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

private:
	GaduChatImageService *CurrentChatImageService;
	GaduChatService *CurrentChatService;
	GaduContactListService *CurrentContactListService;
	GaduFileTransferService *CurrentFileTransferService;
	GaduPersonalInfoService *CurrentPersonalInfoService;
	GaduSearchService *CurrentSearchService;

	DccManager *Dcc;

	QHostAddress ActiveServer;

	struct gg_login_params GaduLoginParams;
	gg_session *GaduSession;

	unsigned int sendImageRequests;

	QHostAddress DccExternalIP;

	GaduProtocolSocketNotifiers *SocketNotifiers;

	QTimer *PingTimer;

	/**
		Ustawianie parametr�w po��czenia proxy. Metoda wywo�ywana podczas logowania.

		@see login
	**/
	void setupProxy();

	void networkConnected();
	void networkDisconnected();

	GaduAccountData * gaduAccountData() const;

	UinType uin(Contact contact) const;
	GaduContactAccountData * gaduContactAccountData(Contact contact) const;

	Status::StatusType statusTypeFromIndex(unsigned int index) const;

	GaduProtocolSocketNotifiers * socketNotifiers() { return SocketNotifiers; }

	friend class GaduChatImageService;
	friend class GaduChatService;
	friend class GaduContactListService;
	friend class GaduFileTransferService;
	friend class GaduPersonalInfoService;
	friend class GaduSearchService;

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
		Slot wywo�ywany po zerwaniu po��czenia przez serwer. Wywo�uje disconnectedSlot(),
		oraz dodatkowo zmienia NextStatus.
	**/
	void socketDisconnectedSlot();


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
		Wykonuje zadania co minut� - pinguje sie� i zeruje licznik
		odebranych obrazk�w (je�li jeste�my po��czeni).
	**/
	void everyMinuteActions();

	/**
		Pobrano list� u�ytkownik�w z serwera. Emituje userStatusChanged dla ka�dego
		otrzymanego kontaktu oraz userListChanged przed zako�czeniem dzia�ania.

		@see userStatusChanged
		@see userListChanged
	**/
	void userListReceived(const struct gg_event *);

	/**
		Informacja o zmianie statusu kontaktu. Emituje userStatusChanged oraz userListChanged.

		@see userStatusChanged
		@see userListChanged
	**/
	void userStatusChanged(const struct gg_event *);

protected:
	virtual void changeStatus(Status status);

public:
	static void initModule();

	static int statusToType(Status status);
	static Status typeToStatus(int type);

	GaduProtocol(Account *account, ProtocolFactory *factory);
	virtual ~GaduProtocol();

	virtual ChatImageService * chatImageService() { return CurrentChatImageService; }
	virtual ChatService * chatService() { return CurrentChatService; }
	virtual ContactListService * contactListService() { return CurrentContactListService; }
	virtual FileTransferService * fileTransferService() { return CurrentFileTransferService; }
	virtual PersonalInfoService * personalInfoService() { return CurrentPersonalInfoService; }
	virtual SearchService * searchService() { return CurrentSearchService; }

	virtual void setAccount(Account *account);

	unsigned int maxDescriptionLength();

	gg_session * gaduSession() { return GaduSession; }

	void changeID(const QString &id);
	/**
		Zwraca serwer z kt�rym jeste�my po��czeni lub do kt�rego si� w�a�nie ��czymy.
		isNull() = hub.
	**/
	QHostAddress activeServer();

	/**
		Po jedno sekundowym op��nieniu wykonuje pr�b� po��czenia.
	**/
	void connectAfterOneSecond();

	/**
		Ustawia adres IP kt�ry ma by� przekazany do serwera GG
		jako adres komputera, kt�ry przekierowuje do nas port DCC.
		Zmiana jest uwzgl�dniana dopiero przy ponownym po��czeniu
		z serwerem.
	**/
	void setDccExternalIP(const QHostAddress& ip);
	bool validateUserID(QString &uid);

	virtual QPixmap statusPixmap(Status status);

	// --------------------
	//  DCC
	// --------------------

	void dccRequest(UinType);
	void setDccIpAndPort(unsigned long dcc_ip, int dcc_port);

public slots:

	/**
		Wysy�a pro�b� o przys�anie obrazka z danymi parametrami.

		@param user u�ytkownik, od kt�rego chcemy obrazek
		@param size rozmiar obrazka w bajtach
		@param crc32 crc32 pliku
		@todo powinno by� sendImageRequest(uniqId uint32_t) - info o obrazku zapisywa� gdzie� w �rodku
	**/
	bool sendImageRequest(Contact contact, int size, uint32_t crc32);

	/**
		Wywy�a obrazek o podanych parametrach.

		@param user u�ytkownik, kt�remu wysy�amy obrazek
		@param file_name nazwa pliku obrazka
		@param size rozmiar obrazka w bajtach
		@param data zawarto�� pliku
		@todo usun�� parametry size i data - mo�emy to chyba sami wyznaczy�
	**/
	bool sendImage(Contact contact, const QString &file_name, uint32_t size, const char *data);

	/**
		Wysy�a nasz� list� u�ytkownik�w na serwer. Uwaga: nie ma to nic wsp�lnego z importem/eksportem.
	**/
	void sendUserList();
	void sendUserListLater();

	/**
		Pobiera informacje o danych osobowych z katalogu publicznego.

		@todo jak to w og�le dzia�a, bo zapomnia�em??
	**/
	void getPersonalInfo(SearchRecord &searchRecord);

	/**
		Ustawia informacje o danych osobowych w katalogu publicznym.

		@todo jak to w og�le dzia�a, bo zapomnia�em??
	**/
	void setPersonalInfo(SearchRecord &searchRecord, SearchResult &newData);

signals:
	/**
		wyst�pi� b��d po��czenia
		@param err przyczyna
		@see connectionError
	**/
	void error(GaduError err);

	/**
		Served sent information about status change for unknown user.
	**/
	void userStatusChangeIgnored(Contact);

	/**
		Otrzymano wiadomo�� CTCP.
		Kto� nas prosi o po��czenie dcc, poniewa�
		jeste�my za NAT-em
		@param user wywo�uj�cy
		@todo zmieni� nazw�
	**/
	void dccConnectionReceived(Contact contact);

	void dcc7New(struct gg_dcc7 *);
	void dcc7Accepted(struct gg_dcc7 *);
	void dcc7Rejected(struct gg_dcc7 *);

};

#endif // GADU_PROTOCOL_H
