#ifndef EVENTS_H
#define EVENTS_H

#include <qdialog.h>
#include <qstring.h>
#include <qtimer.h>
#include <qlistbox.h>

#include "libgadu.h"
#include "misc.h"

class AutoConnectionTimer : private QTimer {
	Q_OBJECT

	public:
		static void on();
		static void off();

	public slots:
		void doConnect();
	
	private:
		AutoConnectionTimer(QObject *parent = 0);

		static AutoConnectionTimer *autoconnection_object;
};

class ConnectionTimeoutTimer : public QTimer {
	Q_OBJECT

	public:
		static void on();
		static void off();
		static bool connectTimeoutRoutine(const QObject *receiver, const char *member);

	private:
		ConnectionTimeoutTimer(QObject *parent = 0);

		static ConnectionTimeoutTimer *connectiontimeout_object;
};

class SavePublicKey : public QDialog {
	Q_OBJECT
	public:
		SavePublicKey(uin_t uin, QString keyData, QWidget *parent = 0, const char *name = 0);

	private:
		uin_t uin;
		QString keyData;

	private slots:
		void yesClicked();
};

/**
	Menad�er zdarze�. Udost�pnia sygna�y reprezentuj�ce zdarzenia
	otrzymywane z serwera GG.
**/
class EventManager : public QObject
{
	Q_OBJECT

	private slots:
		void connectedSlot();
		void connectionFailedSlot(int);
		void connectionBrokenSlot();
		void connectionTimeoutSlot();
		void connectionTimeoutTimerSlot();
		void disconnectedSlot();
		void userStatusChangedSlot(struct gg_event*);
		void userlistReceivedSlot(struct gg_event *);
		void messageReceivedSlot(int, UinsList,unsigned char *msg,time_t,
			int formats_length, void *formats);
		void systemMessageReceivedSlot(QString &msg, time_t,
			int formats_length, void *formats);
		void chatMsgReceived2Slot(UinsList senders,const QString& msg,time_t time);
		void ackReceivedSlot(int seq);
		void dccConnectionReceivedSlot(const UserListElement& sender);
		void pubdirReplyReceivedSlot(gg_pubdir50_t res);
		void userlistReplyReceivedSlot(char type, char *reply);

	public:
		EventManager();
		void eventHandler(gg_session* sess);

	signals:
		/**
			Nawi�zano po��czenie z serwerem
		**/
		void connected();
		/**
			B��d po��czenia z serwerem
		**/
		void connectionFailed(int);
		/**
			Po��czenie z serwerem zosta�o przerwane
		**/
		void connectionBroken();
		/**
			Po��czenie z serwerem zosta�o przerwane z powodu zbyt d�ugiego
			czasu nieaktywno�ci w trakcie ��czenia
		**/
		void connectionTimeout();
		/**
			Otrzymano polecenie roz��czenia
		**/
		void disconnected();		
		/**
			Kt�ry� z kontakt�w zmieni� sw�j status
		**/
		void userStatusChanged(struct gg_event*);
		/**
			Przysz�a z serwera lista kontakt�w
		**/
		void userlistReceived(struct gg_event *);
		/**
			Otrzymano jak�� wiadomo�� od serwera GG
		**/
		void systemMessageReceived(QString &msg, time_t, int formats_length, void *formats);
		/**
			Otrzymano wiadomo�� systemowa od serwera GG
		**/
		void messageReceived(int,UinsList,unsigned char* msg,time_t,
			int formats_length, void *formats);
		
		/**
			Otrzymano wiadomo��, kt�r� trzeba pokaza� (klasa chat lub msg,
			nadawca nie jest ignorowany, itp)
			Tre�� zdeszyfrowana i zdekodowana do unicode.
			Jesli ktorys ze slotow ustawi zmienna grab na true
			to sygnal chatReceived2 nie zostanie wygenerowany.
		**/
		void chatMsgReceived1(UinsList senders,const QString& msg,time_t time,bool& grab);
		void chatMsgReceived2(UinsList senders,const QString& msg,time_t time);
		/**
			Otrzymano potwierdzenie wiadomo�ci
		**/
		void ackReceived(int seq);
		/**
			Otrzymano DCC CTCP
		**/
		void dccConnectionReceived(const UserListElement& sender);
		/**
			Otrzymano z serwera rezultat operacji wyszukiwania
			w katalogu publicznym
		**/		
		void pubdirReplyReceived(gg_pubdir50_t res);
		/**
			Otrzymano z serwera rezultat operacji na li�cie
			u�ytkownik�w
		**/		
		void userlistReplyReceived(char type, char *reply);
};

extern EventManager event_manager;

extern QTime lastsoundtime;

class EventConfigSlots : public QObject
{
	Q_OBJECT
	public:
	    static void initModule();
	    
	public slots:
	    void onCreateConfigDialog();
	    void onDestroyConfigDialog();
	    void ifDccEnabled(bool value);
	    void ifDccIpEnabled(bool value);
	    void ifDefServerEnabled(bool value);
	    void useTlsEnabled(bool value);
	    void _Left();
	    void _Right();
	    void _Left2(QListBoxItem *item);
	    void _Right2(QListBoxItem *item);

	    void ifNotifyGlobal(bool toggled);
	    void ifNotifyAll(bool toggled);
};

#endif
