#ifndef NOTIFY_H
#define NOTIFY_H
#include <qobject.h>
#include <qstring.h>
#include <qmap.h>
#include <qpair.h>
#include <qvaluelist.h>

#include "gadu.h"
#include "notify_slots.h"

class Notify : QObject
{
	Q_OBJECT
	private:
	class Notifier
	{
		public:
		Notifier();
		Notifier(QObject *notifier, const QMap<QString, QString> &notifierSlots);
		
		QObject *notifier; //obiekt "nas�uchuj�cy" na zdarzenia
		QMap<QString, QPair<QString, bool> > notifierSlots; //nazwa sygna�u("NewMessage") -> para<slot (SLOT(m�jSlotMessage)), czy po��czony>
	};
	QMap<QString, Notifier> notifiers; //nazwa powiadamiacza("Hints") -> obiekt powiadomienia
	QMap<QString, QString> notifySignals; //nazwa sygna�u("NewMessage") -> sygna� (SIGNAL(newMessage(...)))
	
	/*
	 * dodaje kolumn� checkboks�w w konfiguracji,
	 * na podstawie notifierSlots decyduje czy doda� checkboksa aktywnego czy nie
	 */
	void addConfigColumn(const QString &name, const QMap<QString, QString> &notifierSlots);
	
	/* usuwa kolumn� checkboks�w z konfiguracji */
	void removeConfigColumn(const QString &name, const QMap<QString, QPair<QString, bool> > &notifierSlots);
	
	private slots:
	/* obs�uga zmian status�w */
	void changingStatus(const UinType, const unsigned int, const unsigned int);

	/* obs�uga zmian status�w */
	void changedStatus(UserListElement *);
	
	/*
	 * ��czy odpowiedni sygna� z notifierName (np.: Window, Hint, Sound)
	 * ze slotem slotName (np.: NewChat, ConnError)
	 */
	void connectSlot(const QString &notifierName, const QString &slotName);
	
	/*
	 * roz��cza odpowiedni sygna� z notifierName (np.: Window, Hint, Sound)
	 * od slotu slotName (np.: NewChat, ConnError)
	 */
	void disconnectSlot(const QString &notifierName, const QString &slotName);
	
	public:
	Notify();
	~Notify();
	
	/*
	 * rejestruje obiekt, kt�ry chce otrzymywa� informacje o zdarzeniach
	 * name b�dzie u�yte w oknie konfiguracji (nale�y podawa� angielsk� nazw�)
	 * slots to mapa odwzoruj�ca nazwy sygna��w na sloty w obiekcie notifier
	 * nazwy sygna��w:
		"NewChat" - nowa rozmowa
		"NewMessage" - nowa wiadomo��
		"ConnError" - b��d po��czenia
		"ChangingStatus" - zmiana statusu (dopiero si� odb�dzie)
		"toAvailable" - u�ytkownik zmieni� status na "Dost�pny"
		"toBusy" - u�ytkownik zmieni� status na "Zaraz wracam"
		"toNotAvailable" - u�ytkownik zmieni� status na "Niewidoczny lub Niedost�pny"
		"Message" - inna wiadomo��
	 * 
	 * nie trzeba definiowa� wszystkich slot�w
	 */
	void registerNotifier(const QString &name, QObject *notifier,
							const QMap<QString, QString> &notifierSlots);
	/*
	 * wyrejestrowuje obiekt
	 */
	void unregisterNotifier(const QString &name);
	
	public slots:
	/*
	 * aktualizuje wszystkie po��czenia
	 * wywo�ywany po zmianie konfiguracji dotycz�cej obs�ugi 
	 */
	void updateConnections();

	/* 
	 * emituje sygna� message
	 *  je�eli to==QString::null, to wysy�a do wszystkich, kt�rzy s� zaznaczeni w konfiguracji
	 *  je�eli to!=QString::null, to wysy�a tylko do jednego
	 * Notifier decyduje, kt�rych argument�w u�y�
	 */
	void emitMessage(const QString &from, const QString &to, const QString &type=QString(), const QString &message=QString(), const UserListElement *ule=NULL);
	
	signals:
	//UWAGA: razem ze zmianami nazw/parametr�w tych sygna��w nale�y aktualizowa� wpisy w konstruktorze Notify
	
	/* nowa rozmowa */
	void newChat(UinsList senders, const QString& msg, time_t time, bool &grab);
	
	/* nowa wiadomo�� w oknie chat */
	void newMessage(UinsList senders, const QString& msg, time_t time);
	
	/* b��d po��czenia */
	void connectionError(const QString &message);
	
	/* u�ytkownik ZMIENIA status (mo�na operowa� tylko na podanych parametrach, bo status _jeszcze_ si� nie zmieni�) */
	void userChangingStatus(const UinType uin, const unsigned int oldstatus, const unsigned int status);
	
	/* u�ytkownik ZMIENI� status na "Dost�pny" */
	void userChangedStatusToAvailable(const UserListElement &);
	
	/* u�ytkownik zmieni� status na "Zaraz wracam" */
	void userChangedStatusToBusy(const UserListElement &);
	
	/* u�ytkownik zmieni� status na "Niewidoczny" lub "Niedost�pny" */
	void userChangedStatusToNotAvailable(const UserListElement &);

	/* inna informacja do powiadomienia */
	void message(const QString &from, const QString &type, const QString &message, const UserListElement *ule);
	
	signals:
	/* do u�ytku wewn�trznego */
	void privateMessage(const QString &from, const QString &message, const QString &type, const UserListElement *ule);
};

extern Notify *notify;
extern NotifySlots *notify_slots;

#endif
