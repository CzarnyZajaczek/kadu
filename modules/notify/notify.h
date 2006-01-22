#ifndef NOTIFY_H
#define NOTIFY_H
#include <qobject.h>
#include <qstring.h>
#include <qmap.h>
#include <qvariant.h>
#include <qpair.h>
#include <qvaluelist.h>
#include <qstringlist.h>

#include <time.h>

#include "notify_slots.h"
#include "protocol.h"
#include "userlist.h"

class Notifier : public QObject {
	public:
		Notifier(QObject *parent = 0, const char *name = 0) : QObject(parent, name) {};
		virtual ~Notifier() {};
		virtual void externalEvent(const QString &notifyType, const QString &msg, const UserListElements &ules) = 0;
};

class Notify : public QObject
{
	Q_OBJECT
	private:
		class NotifierSlots
		{
			public:
			NotifierSlots();
			NotifierSlots(Notifier *notifier, const QMap<QString, QString> &notifierSlots);

			Notifier *notifier; //obiekt "nas�uchuj�cy" na zdarzenia
			QMap<QString, QPair<QString, bool> > notifierSlots; //nazwa sygna�u("NewMessage") -> para<slot (SLOT(m�jSlotMessage)), czy po��czony>
		};
		QMap<QString, NotifierSlots> notifiers; //nazwa powiadamiacza("Hints") -> obiekt powiadomienia
		QMap<QString, QString> notifySignals; //nazwa sygna�u("NewMessage") -> sygna� (SIGNAL(newMessage(...)))
		QStringList eventNames;
		QValueList<QPair<QString, QString> > notifyEventsNames;

		/*
		 * dodaje kolumn� checkboks�w w konfiguracji,
		 * na podstawie notifierSlots decyduje czy doda� checkboksa aktywnego czy nie
		 */
		void addConfigColumn(const QString &name, const QMap<QString, QString> &notifierSlots);

		/* usuwa kolumn� checkboks�w z konfiguracji */
		void removeConfigColumn(const QString &name, const QMap<QString, QPair<QString, bool> > &notifierSlots);

		void addConfigRow(const QString &name, const QString &description);
		void removeConfigRow(const QString &name, const QString &description);

	private slots:
		/* pomocniczy slot */
		void newChatSlot(Protocol *protocol, UserListElements senders, const QString &msg, time_t t);
		/* pomocniczy slot */
		void probablyNewMessage(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool &grab);

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
		Notify(QObject *parent=0, const char *name=0);
		~Notify();

		/*
		 * rejestruje obiekt, kt�ry chce otrzymywa� informacje o zdarzeniach
		 * name b�dzie u�yte w oknie konfiguracji (nale�y podawa� angielsk� nazw�)
		 * slots to mapa odwzoruj�ca nazwy sygna��w na sloty w obiekcie notifier
		 * nazwy sygna��w:
			"NewChat" - nowa rozmowa
			"NewMessage" - nowa wiadomo��
			"ConnError" - b��d po��czenia
			"StatusChanged" - zmiana statusu og�lna
			"toAvailable" - u�ytkownik zmieni� status na "Dost�pny"
			"toBusy" - u�ytkownik zmieni� status na "Zaraz wracam"
			"toInvisible" - u�ytkownik zmieni� status na "Ukryty"
			"toNotAvailable" - u�ytkownik zmieni� status na "Niewidoczny lub Niedost�pny"
			"Message" - inna wiadomo��
		 *
		 * nie trzeba definiowa� wszystkich slot�w
		 */
		void registerNotifier(const QString &name, Notifier *notifier,
							const QMap<QString, QString> &notifierSlots);
		/*
		 * wyrejestrowuje obiekt
		 */
		void unregisterNotifier(const QString &name);

		void registerEvent(const QString &name, const QString &description);
		void unregisterEvent(const QString &name, const QString &description);

		QStringList notifiersList() const;
		const QValueList<QPair<QString, QString> > &externalNotifyTypes();

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
		void emitMessage(const QString &from, const QString &to, const QString &message=QString(), const QMap<QString, QVariant> *parameters=NULL, const UserListElement *ule=NULL);

		/* obs�uga zmian status�w */
		void statusChanged(UserListElement elem, QString protocolName,
							const UserStatus &oldStatus, bool massively, bool last);

		void notify(const QString &notifyType, const QString &msg, const UserListElements &ules = UserListElements());

	signals:
	//UWAGA: razem ze zmianami nazw/parametr�w tych sygna��w nale�y aktualizowa� wpisy w konstruktorze Notify

		/* nowa rozmowa */
		void newChat(Protocol *protocol, UserListElements senders, const QString &msg, time_t t);

		/* nowa wiadomo�� w oknie chat */
		void newMessage(Protocol *protocol, UserListElements senders, const QString &msg, time_t t, bool &grab);

		/* b��d po��czenia */
		void connectionError(Protocol *protocol, const QString &message);

		/* u�ytkownik zmieni� status */
		void userStatusChanged(UserListElement elem, QString protocolName, const UserStatus &oldStatus);

		/* u�ytkownik zmieni� status na "Dost�pny" */
		void userChangedStatusToAvailable(const QString &protocolName, UserListElement);

		/* u�ytkownik zmieni� status na "Zaraz wracam" */
		void userChangedStatusToBusy(const QString &protocolName, UserListElement);

		/* u�ytkownik zmieni� status na "Niewidoczny" */
		void userChangedStatusToInvisible(const QString &protocolName, UserListElement);

		/* u�ytkownik zmieni� status na "Niewidoczny" lub "Niedost�pny" */
		void userChangedStatusToNotAvailable(const QString &protocolName, UserListElement);

		/* inna informacja do powiadomienia */
		void message(const QString &from, const QString &msg, const QMap<QString, QVariant> *parameters, const UserListElement *ule);

	signals:
		/* do u�ytku wewn�trznego */
		void privateMessage(const QString &from, const QString &message, const QMap<QString, QVariant> *parameters, const UserListElement *ule);
};

extern Notify *notify;
extern NotifySlots *notify_slots;

#endif
