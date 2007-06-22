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

#include "main_configuration_window.h"
#include "notify_slots.h"
#include "notification.h"
#include "protocol.h"
#include "userlist.h"

class MessageNotification;

class QListBox;

/**
 * @defgroup notify Notify
 * @{
 */

/**
	@enum CallbackRequirement
	Okre�la, czy dane zdarzenie wymaga podj�cia od u�ytkownika akcji innej ni� domy�lne zaakceptowanie/odrzucenie.
 **/
enum CallbackRequirement {
	CallbackRequired,
	CallbackNotRequired
};

/**
	@enum CallbackCapacity
	Okre�la, czy dany notifikator potrafi obs�u�y� zdarzenia wymagajace od u�ytkownika akcji innej ni� domy�lne zaakceptowanie/odrzucenie.
 **/
enum CallbackCapacity {
	CallbackSupported,
	CallbackNotSupported
};

/**
	@class Notifier
	@brief Klasa abstrakcyjna opisuj�ca notifikator.

	Notifykatory zajmuj� si� wy�wietlaniem lub informowaniem w inny spos�b u�ytkownika o wyst�puj�cych
	w programie zdarzeniach (nowa rozmowa, nowy transfer pliku, b��d...).

	Notyfikatory mog� umo�liwia� u�ytkownikowi podj�cie akcji jak odebranie lub zignorownie rozmowy,
	odebranie pliku, kontynuacje odbierania pliku i inne. Niekt�ry notifikatory nie b�d�
	implementowa� akcji, dlatego te� niekt�re zdarzenia nie mog� by� przez nie obs�ugiwane.
 **/
class Notifier : public virtual QObject {
	public:
		Notifier(QObject *parent = 0, const char *name = 0) : QObject(parent, name) {};
		virtual ~Notifier() {};

		/**
			Okre�la, czy notifikator poradzi sobie ze zdarzeniami wymagajacymi podj�cia akcji.
		 **/
		virtual CallbackCapacity callbackCapacity() { return CallbackNotSupported; }

		/**
			Metoda informuj�ca notifikator o nowym zdarzeniu. Zdarzenie mo�e wywo�a�
			sygna� closed(), po kt�rym notyfikator musi przesta� informowa� u�ytkownika
			o danym zdarzeniu (na przyk�ad, musi zamkn�� skojarzone ze zdarzeniem okno).
		 **/
		virtual void notify(Notification *notification) = 0;

		/**
			Kopiuje konfiguracje jednego zdarzenia do drugiego.
			U�ywane przy przej�ciu z 0.5 na 0.6 - po 0.6 zostanie usuni�te.
		 **/
		virtual void copyConfiguration(const QString &fromEvent, const QString &toEvent) = 0;
};

class Notify : public ConfigurationUiHandler
{
	Q_OBJECT

	QListBox *allUsers;
	QListBox *notifiedUsers;

	QMap<QString, Notifier *> notifiers; //nazwa powiadamiacza("Hints") -> obiekt powiadomienia

	class NotifyEvent
	{
		public:
			QString name;
			CallbackRequirement callbackRequirement;
			const char *description;
			NotifyEvent() : name(), callbackRequirement(CallbackNotRequired), description(0){}
	};
	QValueList<NotifyEvent> NotifyEvents;

	void import_connection_from_0_5_0(const QString &notifierName, const QString &oldConnectionName, const QString &newConnectionName);

private slots:

	void messageReceived(Protocol *protocol, UserListElements senders, const QString &msg, time_t t);

	void connectionError(Protocol *protocol, const QString &message);
	void statusChanged(UserListElement elem, QString protocolName, const UserStatus &oldStatus, bool massively, bool last);

	void moveUp();
	void moveDown();

	void configurationWindowApplied();

public:
	Notify(QObject *parent=0, const char *name=0);
	virtual ~Notify();

	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

	void notify(Notification *notification);

	void registerNotifier(const QString &name, Notifier *notifier);
	void unregisterNotifier(const QString &name);

	void registerEvent(const QString &name, const char *description, CallbackRequirement callbackRequirement);
	void unregisterEvent(const QString &name);

	QStringList notifiersList() const;
	const QValueList<Notify::NotifyEvent> &notifyEvents();

};

extern Notify *notification_manager;
extern NotifySlots *notify_slots;

/** @} */

#endif
