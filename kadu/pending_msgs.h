#ifndef PENDING_MSGS_H
#define PENDING_MSGS_H

#include <qvaluelist.h>
#include "gadu.h"
#include "misc.h"

/**
	Klasa ta reprezentuje kolejk� wiadomo�ci oczekuj�cych na odczytanie.
	Wiadomo�ci takie mog� by� przechowywane w tej klasie nawet pomi�dzy kolejnymi
	uruchomieniami Kadu.
	\class PendingMsgs
	\brief Kolejka oczekuj�cych wiadomo�ci.
**/
class PendingMsgs : public QObject
{
	Q_OBJECT

	public:
		/**
			Struktura reprezentuj�ca wiadomo�� oczekuj�c� w kolejce.
			\struct Element
			\brief Wiadomo�� oczekuj�c� w kolejce.
		**/
		struct Element
		{
			UinsList uins; /*!< Lista numer�w UIN, kt�re nades�a�y t� wiadomo��. */
			QString msg; /*!< Tre�� wiadomo�ci. */
			int msgclass; /*!< Klasa wiadomo�ci. */
			time_t time; /*!< Czas, w kt�rym nadano wiadomo��. */
		};

	private:
		typedef QValueList<Element> PendingMsgsList;
		PendingMsgsList msgs;
		
	public:
		/**
			\fn PendingMsgs(QObject *parent=0, const char *name=0)
			Standardowy konstruktor.
			\param parent rodzic kontrolki. Domy�lnie 0.
			\param name nazwa kontrolki. Domy�lnie 0.
		**/
		PendingMsgs(QObject *parent=0, const char *name=0);

		/**
			\fn bool pendingMsgs(UinType uin) const
			Sprawdza, czy w kolejce s� jakie� oczekuj�ce wiadomo�ci dla podanego numeru UIN.
			\param uin numer UIN dla kt�rego nale�y sprawdzi� kolejk�.
			\return warto�� logiczn� informuj�c� czy w kolejce jest jaka� wiadomo�� czy nie.
		**/
		bool pendingMsgs(UinType uin) const;

		/**
			\fn bool pendingMsgs() const
			Sprawdza, czy w jakiejkolwiek kolejce s� oczekuj�ce wiadomo�ci.
			\return warto�� logiczn� informuj�c� czy s� jaka� wiadomo�ci czy nie.
		**/
		bool pendingMsgs() const;

		/**
			\fn int count() const
			Liczy wszystkie wiadomo�ci oczekuj�ce w kolejkach.
			\return liczba oczekuj�cych wiadomo�ci.
		**/
		int count() const;

		/**
			\fn Element &operator[](int index)
			\return wiadomo�� z kolejki, o podanym indeksie.
		**/
		Element &operator[](int index);

	public slots:
		/**
			\fn void openMessages()
			Wywo�uje metod� ChatManager::openPendingMsgs.
		**/
		void openMessages();

		/**
			\fn void deleteMsg(int index)
			Usuwa z kolejki wiadomo�� o danym indeksie.
			\param index indeks wiadomo�ci do usuni�cia.
		**/
		void deleteMsg(int index);

		/**
			void addMsg(UinsList uins, QString msg, int msgclass, time_t time)
			Dodaje now� wiadomo�� do kolejki.
			\param uins lista numer�w UIN, kt�re przys�a�y wiadomo��.
			\param msg tre�� wiadomo�ci.
			\param msgclass klasa wiadomo�ci.
			\param time czas nades�ania wiadomo�ci.
		**/
		void addMsg(UinsList uins, QString msg, int msgclass, time_t time);

		/**
			\fn bool loadFromFile()
			Wczytuje kolejk� wiadomo�ci z pliku. Metoda ta wywo�ywana jest przy starcie
			Kadu.
		**/
		bool loadFromFile();

		/**
			\fn bool loadFromFile()
			Zapisuje kolejk� wiadomo�ci do pliku. Metoda ta wywo�ywana jest przy zamykaniu
			Kadu.
		**/
		void writeToFile();
		
	signals:
		/**
			\fn void messageAdded()
			Sygna� ten emitowany jest, gdy nowa wiadomo�� zosta�a dodana do kolejki.
		**/
		void messageAdded();

		/**
			\fn void messageDeleted()
			Sygna� ten emitowany jest, gdy dowolna wiadomo�� zosta�a usuni�ta z kolejki.
		**/
		void messageDeleted();
};

extern PendingMsgs pending;

#endif
