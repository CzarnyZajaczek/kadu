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

#include "libgadu.h"
#include "misc.h"

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

	private:
		void ackHandler(int seq);
			
	private slots:
		void userStatusChangedSlot(struct gg_event*);
		void userlistReceivedSlot(struct gg_event *);
		void messageReceivedSlot(int, UinsList,unsigned char* msg,time_t,int,struct gg_msg_format*);
		
	public:
		EventManager();
		void eventHandler(gg_session* sess);

	signals:
		/**
			Kt�ry� z kontakt�w zmieni� sw�j status
		**/
		void userStatusChanged(struct gg_event*);
		/**
			Przysz�a z serwera lista kontakt�w
		**/
		void userlistReceived(struct gg_event *);
		/**
			Dosz�a jaka� wiadomo�� z serwera GG
		**/
		void messageReceived(int,UinsList,unsigned char* msg,time_t,int,struct gg_msg_format*);
		/**
			Dosz�a wiadomo��, kt�r� trzeba pokaza� (klasa chat lub msg,
			nadawca nie jest ignorowany, itp)
			Tre�� zdeszyfrowana i zdekodowana do unicode.
		**/
		void chatReceived(UinsList senders,const QString& msg,time_t time);
};

extern EventManager event_manager;

extern QTime lastsoundtime;

#endif
