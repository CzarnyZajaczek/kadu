#ifndef KADU_DNS_HANDLER_H
#define KADU_DNS_HANDLER_H

#define QT3_SUPPORT
#include <qglobal.h>

#include <q3dns.h>
#include <qhostaddress.h>
#include <qobject.h>
#include <qstring.h>

/**
	T�umaczy adres IP na nazw� domeny (DNS). Kiedy proces zostanie zako�czony,
	emituje odpowiedni sygna� zawieraj�cy informacj� o nazwie domeny.
	\class DNSHandler
	\brief Klasa t�umacz�ca adres IP na nazw� domeny.
**/
class DNSHandler : public QObject
{
	Q_OBJECT

	public:
		/**
			\fn DNSHandler(const QString &marker, const QHostAddress &addr)
			Konstruktor wywo�uj�cy zapytanie o domen� dla danego adresu IP.
			\param marker znacznik (np. identyfikator protoko�u)
			\param addr adres IP
		**/
		DNSHandler(const QString &marker, const QHostAddress &addr);

		/**
			\fn ~DNSHandler()
			Destruktor klasy
		**/
		~DNSHandler();

		static int counter; /*!< licznik obiekt�w tej klasy */

	private:
		Q3Dns DNSResolver; /*!< obiekt zajmuj�cy si� translacj� adresu na nazw� domeny */
		QString marker; /*!< znacznik (np. identyfikator protoko�u) */

	private slots:
		/**
			\fn void resultsReady()
			Funkcja wywo�ywana, gdy proces t�umaczenia zosta� zako�czony.
		**/
		void resultsReady();

	signals:
		/**
			\fn void result(const QString &marker, const QString &hostname)
			Sygna� emitowany, gdy proces t�umaczenia zosta� zako�czony.
			\param marker znacznik (np. identyfikator protoko�u)
			\param hostname nazwa domeny odpowiadaj�ca adresowi IP o kt�ry pytano
		**/
		void result(const QString &marker, const QString &hostname);
};
#endif
