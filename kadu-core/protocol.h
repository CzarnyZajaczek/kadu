#ifndef KADU_PROTOCOL_H
#define KADU_PROTOCOL_H

#include <qobject.h>
#include <qstring.h>
#include <qdatetime.h>

#include "status.h"
#include "usergroup.h"

typedef uint32_t UinType;

class Protocol : public QObject
{
	Q_OBJECT

	private:
		UserStatus &writeableStatus() { return *NextStatus; }
		friend class Kadu;

	protected:
		QDateTime ConnectionTime;

		QString ProtocolID;
		QString id;

		/**
			Bie��cy status. Zmieniany po po��czeniu, oraz w przypadku zmiany statusu kiedy po��czenie
			jest ju� zainicjowane.

			@see login
			@see connected
			@see NextStatus
			@see UserStatus
			@see status
		**/
		UserStatus *CurrentStatus;

		/**
			Nast�pny status. Ustalany zewn�trznie przy wykorzystaniu metody status i odpowiednich
			slot�w klasy UserStatus. Zmiana wywo�uje jedn� z metod iWantGo... i w konsekwencji zmian�
			statusu (w razie konieczno�ci te� zalogowanie).

			@see login
			@see connected
			@see CurrentStatus
			@see UserStatus
			@see status
		**/
		UserStatus *NextStatus;

	public:
		Protocol(const QString &proto, const QString &id, QObject *parent = 0, const char *name = 0);
		virtual ~Protocol();

		/**
			Status u�ytkownika. Za pomoc� tej metody mo�emy go zmieni�, pobra� ikon� statusu i wykona�
			kilka innych ciekawych rzeczy.

			1. Zmiana statusu:
			<code>
				Protocol *proto;

				...

				proto->status().setOnline("Jestem zalogowany"); // zalogowanie i ustawienie opisu
				proto->status().setFriendsOnly(true);           // tryb tylko dla przyjaci�
				...
				proto->status().setOffline();                   // wylogowanie, usuni�cie opisu
			</code>

			2. Sprawdzenie statusu:
			<code>
				Protocol *proto;

				if (proto->status().isOnline())                 // jeste�my online
					...
				else if (proto->status().isInvisible())         // jeste�my niewidzialni
					...

				// mo�na te�:
				switch (proto->status().status())
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
				Protocol *proto;

				...

				pix = proto->status().pixmap();
				name = proto->status().name();
			</code>

			@see currentStatus
		**/
		const UserStatus &status() { return *NextStatus; }

		/**
			Rzeczywisty aktualny status. Mo�na go wykorzysta� tylko w trybie do odczytu (pobranie
			ikony, nazwy, sprawdzenie rzeczywistego stanu po��czenia).

			@see status
		**/
		const UserStatus &currentStatus() const { return *CurrentStatus; }

		QString protocolID() const { return ProtocolID; }

		QString ID() const { return id; }

		virtual bool validateUserID(QString& uid) = 0;

		virtual UserStatus *newStatus() const = 0;

		const QDateTime &connectionTime() const;
	public slots:
		/**
			Wysy�a wiadomo�� bez formatowania tekstu. Je�li adresat�w jest wi�cej ni� jeden, to wysy�ana
			jest wiadomo�� konferencyjna. Zwracany jest numer sekwencyjny wiadomo�ci, je�li
			przypadkiem by�my chcieli �ledzi� jej potwierdzenie.
			@param users lista u�ytkownik�w, do kt�rych wysy�amy wiadomo��
			@param mesg wiadomo��, kt�r� wysy�amy - kodowanie zmieniane wewn�trz
		**/
		virtual /*int*/void sendMessage(UserListElements users, const QString &mesg) = 0;
		/**
			Wysy�a wiadomo�� bez formatowania tekstu. Zwracany jest numer sekwencyjny wiadomo�ci, je�li
			przypadkiem by�my chcieli �ledzi� jej potwierdzenie.
			@param users lista u�ytkownik�w, do kt�rych wysy�amy wiadomo��
			@param mesg wiadomo��, kt�r� wysy�amy - kodowanie zmieniane wewn�trz
		**/
		/*int*/void sendMessage(UserListElement user, const QString &mesg);


	signals:

		/**
			uda�o si� zalogowa�
		**/
		void connected();

		/**
			rozpoczynamy procedur� logowania si�
		**/
		void connecting();

		/**
			roz��czyli�my si� z serwerem
		**/
		void disconnected();

		/**
			wyst�pi� b��d po��czenia
			@param protocol protok�
			@param reason napis do wy�wietlenia dla u�ytkownika
		**/
		void connectionError(Protocol *protocol, const QString &reason);

		/**
			\fn void messageFiltering(const UserGroup *users, QCString& msg, bool& stop)
			Sygnal daje mozliwosc operowania na wiadomosci
			ktora ma byc wyslana do serwera juz w jej docelowej
			formie po konwersji z unicode i innymi zabiegami.
			Tresc wiadomosci mozna zmienic podmieniajac wskaznik
			msg na nowy bufor i zwalniajac stary (za pomoca free).
			Mozna tez przerwac dalsza jej obrobke ustawiajac
			wskaznik stop na true.
			\param users lista u�ytkownik�w
			\param msg wiadomo��
			\param stop zako�czenie dalszej obr�bki sygna�u
		**/
		void sendMessageFiltering(const UserListElements users, QString &msg, bool &stop);
		/**
			wiadomo�� nie zosta�a dostaczona
		**/
		void messageNotDelivered(const QString &message);
		/**
			wiadomo�� zosta�a przyj�ta przez serwer
			TODO: WTF??
		**/
		void messageAccepted();

		/**
			\fn receivedMessageFilter(Protocol *protocol, UserListElements senders, const QString &msg, time_t time, bool &ignore);
			Filtrujemy wiadomo��. Mo�na j� odrzuci� albo i nie.
			\param protocol protok� na kt�rym otrzymali�my wiadomo��
			\param senders lista nadawc�w
			\param message komunikat w postaci Unicode HTML
			\param time czas nadania wiadomo�ci
			\param ignore po ustawieniu na true wiadomo�� jest ignorowana
		**/
		void receivedMessageFilter(Protocol *protocol, UserListElements senders, const QString &message, time_t time, bool &ignore);
		/**
			\fn messageReceived(Protocol *protocol, UserListElements senders, const QString &msg, time_t time);
			Otrzymali�my wiadomo��.
			\param protocol protok� na kt�rym otrzymali�my wiadomo��
			\param senders lista nadawc�w
			\param message komunikat w postaci Unicode HTML
			\param time czas nadania wiadomo�ci
		**/
		void messageReceived(Protocol *protocol, UserListElements senders, const QString &message, time_t time);

	private:
		Protocol(const Protocol &) {}
		virtual Protocol &operator=(const Protocol &){return *this;}
};

#endif
