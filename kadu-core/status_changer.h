#ifndef STATUS_CHANGER
#define STATUS_CHANGER

#include <qglobal.h>

#include <qobject.h>
//Added by qt3to4:
#include <Q3ValueList>

#include "status.h"

class QTimer;

/**
	@class StatusChanger
	@brief Podstawowa klasa s�u��ca do zmiany statusu u�ytkownika.

	Implementacje klasy dokonuj� zmiany statusu u�ytkownik�w na podstawie dowolnych, zale�nych od
	klasy parametr�w. Zmianie mo�e ulega� zar�wno opis jak i stan dost�pno�ci.
 **/
class StatusChanger : public QObject
{
	Q_OBJECT

	int Priority;

public:
	/**
		Podstawowy kontruktor. Tworzy modyfikator statusu z zadanym priorytetem.
		Modyfikatory aplikowane s� w kolejno�ci rosn�cych priorytet�w.

		@arg priority priorytet modyfikatora
	 **/
	StatusChanger(int priority);
	virtual ~StatusChanger();

	int priority();

	/**
		Metoda zmieniaj�ca status.

		@arg status aktualny status u�ytkownika, metoda mo�e zmieni� warto�� argumentu
	 **/
	virtual void changeStatus(UserStatus &status) = 0;

signals:
	void statusChanged();

};

/**
	@class UserStatusChanger
	@brief Klasa umo�liwiaj�ca u�ytkownikowi ustawienie w�asnego statusu.

	Klasa posiada pole userStatus okre�laj�ce status, jaki u�ytkownik sam sobie ustawi�.
	Klasa modyfikuje aktualny status u�ytkownika ustawiaj�c go na warto�� pola userStatus.
	Priorytet modyfikatora: 0.
 **/
class UserStatusChanger : public StatusChanger
{
	Q_OBJECT

	UserStatus userStatus;

public:
	UserStatusChanger();
	virtual ~UserStatusChanger();

	virtual void changeStatus(UserStatus &status);
	const UserStatus & status() { return userStatus; }

public slots:
	/**
		Zapisuje status, jaki u�ytkownik sobie ustawi�.

		@param status status u�ytkownika
	 **/
	void userStatusSet(UserStatus &status);

};

/**
	@class SplitStatusChanger
	@brief Klasa dziel�ca d�ugie opisy na kr�tsze.

	Klasa dzieli d�ugi opis na kilka kr�tszych, kt�re wy�wietlane s� w odst�pie 30 sekund od siebie.
	Dla przyk�adu, opis "ten opis ma wi�cej ni� dwadzie�cia znak�w" przy maksymalnej d�ugo�ci opisu
	wynosz�cej dwadzie�cia znak�w zostanie podzielony na nast�puj�ce pod-opisy:

		"ten opis ma wi� --->"
		"<--- cej ni� dw --->"
		"<--- adzie�cia  --->"
		"<--- znak�w"

	W Gadu-Gadu limit d�ugo�ci wynosi 70 znak�w.

	Priorytet: 1000.
 **/
class SplitStatusChanger : public StatusChanger
{
	Q_OBJECT

	unsigned int splitSize;

	unsigned int descriptionSplitBegin;
	unsigned int descriptionSplitLength;

	QTimer *splitTimer;
	QString lastDescription;

	bool enabled;

private slots:
	void timerInvoked();

public:
	/**
		Tworzy nowy obiekt dziel�cy opisy na pod-opisy o maksymalnej d�ugo�ci splitSize znak�w.

		@arg splitSize maksymalna d�ugo�� pod-opisu
	 **/
	SplitStatusChanger(unsigned int splitSize);
	virtual ~SplitStatusChanger();

	virtual void changeStatus(UserStatus &status);

	void enable();
	void disable();

};

/**
	@class StatusChangerManager
	@brief Manager modyfikator�w statusu.

	Klasa zarz�dza modyfikatorami statusu. Do��cza i od��cza na ��danie nowy modyfikatory.
	Wywo�uje poszczeg�lne modyfikatory w kolejno�ci ich rosn�cych priorytet�w.

	Po stworzeniu i dodaniu pierwszych modyfikator�w wymaga uruchomienia za pomoc� metody enable.
 **/
class StatusChangerManager : public QObject
{
	Q_OBJECT

	Q3ValueList<StatusChanger *> statusChangers;
	bool enabled;

public:
	static void initModule();
	static void closeModule();

	StatusChangerManager();
	virtual ~StatusChangerManager();

	/**
		Aktywuje modyfikatory.
	 **/
	void enable();

	void registerStatusChanger(StatusChanger *statusChanger);
	void unregisterStatusChanger(StatusChanger *statusChanger);

public slots:
	/**
		Slot wywo�ywany, gdy jeden z modyfikator�w dokona zmiany opisu.
		Wywo�anie powoduje ponowne przetworzenie statusu przez wszystkie modyfikatory
		i wys�anie sygna�u statusChanged(UserStatus).
	 **/
	void statusChanged();

signals:
	/**
		Sygna� wysy�any, gdy zostanie ustalony ostateczny status u�ytkownika.
	 **/
	void statusChanged(UserStatus);

};

extern StatusChangerManager *status_changer_manager;

#endif // STATUS_CHANGER
