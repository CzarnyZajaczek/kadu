/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef EVENTS_H
#define EVENTS_H

#include <qdialog.h>
#include <qstring.h>
#include <qdatetime.h>
#include <qtimer.h>

#include "libgadu.h"
#include "misc.h"

class AutoConnectionTimer : private QTimer {
	Q_OBJECT

	public:
		static void on();
		static void off();

	private:
		AutoConnectionTimer(QObject *parent = 0);
		void doConnect();

		static AutoConnectionTimer *autoconnection_object;
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

class EventManager : public QObject
{
	Q_OBJECT

	private slots:
		void connectedSlot();
		void connectionFailedSlot();
		void connectionBrokenSlot();
		void disconnectedSlot();
		void userStatusChangedSlot(struct gg_event*);
		void userlistReceivedSlot(struct gg_event *);
		void messageReceivedSlot(int, UinsList,unsigned char *msg,time_t,
			int formats_length, void *formats);
		void chatReceivedSlot(UinsList senders,const QString& msg,time_t time);
		void ackReceivedSlot(int seq);
		void dccConnectionReceivedSlot(const UserListElement& sender);
		void pubdirReplyReceivedSlot(gg_pubdir50_t res);
		
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
		void connectionFailed();
		/**
			Po��czenia z serwerem zosta�o przerwane
		**/
		void connectionBroken();
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
			Otrzymano jak�� wiadomo�� z serwera GG
		**/
		void messageReceived(int,UinsList,unsigned char* msg,time_t,
			int formats_length, void *formats);
		/**
			Otrzymano wiadomo��, kt�r� trzeba pokaza� (klasa chat lub msg,
			nadawca nie jest ignorowany, itp)
			Tre�� zdeszyfrowana i zdekodowana do unicode.
		**/
		void chatReceived(UinsList senders,const QString& msg,time_t time);
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
};

extern EventManager event_manager;

extern QTime lastsoundtime;

#endif
