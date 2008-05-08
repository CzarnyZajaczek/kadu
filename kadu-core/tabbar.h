#ifndef KADU_TABBAR_H
#define KADU_TABBAR_H

#include <QTabBar>

/**
	Klasa ta jest odpowiedzialna za obs�ug� zak�adek grup kontakt�w.
	\class KaduTabBar
	\brief Zak�adki grup.
**/
class KaduTabBar : public QTabBar {
	Q_OBJECT
	public:
		/**
			\fn KaduTabBar(QWidget *parent = 0, const char *name = 0)
			Standardowy konstruktor.
			\param parent rodzic kontrolki.
			\param name nazwa kontrolki.
		**/
		KaduTabBar(QWidget *parent = 0);

		/**
			\fn void dragEnterEvent(QDragEnterEvent* e)
			Metoda obs�uguj�ca zdarzenie "przeci�gni�cia" nad list� zak�adek "podniesionego" wcze�niej elementu.
			\param e wska�nik obiektu opisuj�cego to zdarzenie.
		**/
		void dragEnterEvent(QDragEnterEvent* e);

		/**
			\fn void dropEvent(QDropEvent* e)
			Metoda obs�uguj�ca zdarzenie "upuszczenia" nad list� zak�adek "podniesionego" wcze�niej elementu.
			\param e wska�nik obiektu opisuj�cego to zdarzenie.
		**/
		void dropEvent(QDropEvent* e);

};

#endif
