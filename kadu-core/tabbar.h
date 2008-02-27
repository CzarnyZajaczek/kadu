#ifndef KADU_TABBAR_H
#define KADU_TABBAR_H

#include <qglobal.h>

#include <qtabbar.h>
//Added by qt3to4:
#include <QResizeEvent>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <Q3PtrList>

class QPainter;
class QToolButton;

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
		KaduTabBar(QWidget *parent = 0, const char *name = 0);
		~KaduTabBar();

		/**
			\fn void layoutTabs()
			Ustawia zak�adki w odpowiedniej pozycji.
		**/
		void layoutTabs();

		/**
			\fn QSize sizeHint() const
			Sugeruje odpowiedni rozmiar zak�adek.
			\return sugerowany rozmiar.
		**/
		QSize sizeHint() const;

		/**
			\fn QSize minimumSizeHint() const
			Sugeruje minimalny rozmiar zak�adek.
			\return sugerowany rozmiar.
		**/
		QSize minimumSizeHint() const;

		/**
			\fn int insertTab(QTab *newTab, int index = -1)
			Dodaje now� zak�adk�.
			\param newTab wska�nik do obiektu zak�adki, kt�r� dodajemy.
			\param index pozycja, w kt�r� ma by� wstawiona nowa zak�adka. Domy�lnie -1 (na ko�cu).
		**/
// 		int insertTab(QString *newTab, int index = -1);

		/**
			\fn void removeTab(QTab *t)
			Usuwa zak�adk�.
			\param t wska�nik do obiektu zak�adki, kt�r� usuwamy.
		**/
// 		void removeTab(QString *t);

	public slots:

		/**
			\fn void setCurrentTab(QTab *tab)
			Ustawia dan� zak�adk� jako bierz�c�.
			\param tab Wska�nik do obiektu zak�adki, kt�r� ustawiamy.
		**/
// 		void setCurrentTab(QString tab);

	private slots:
		void scrollTabsVert();

	protected:
		/**
			\fn void paint(QPainter *p, QTab *t, bool selected) const
			Rysuje dan� zak�adk�.
			\param p urz�dzenie rysuj�ce.
			\param t wska�nik do obiektu zak�adki, kt�r� chcemy rysowa�.
			\param selected informuje o tym, czy zak�adka jest wybran� (bierz�c�),
				czy jedn� z zak�adek zas�oni�tych.
		**/
		void paint(QPainter *p, int index, bool selected) const;

		/**
			\fn void updateArrowButtonsVert()
			Od�wierza przyciski s�u��ce do przewijania listy zak�adek (gdy jest d�uga).
		**/
		void updateArrowButtonsVert();

		/**
			\fn void makeVisibleVert(QTab *t)
			W��cza przyciski s�u��ce do przewijania listy zak�adekna danej zak�adce.
			\param t wska�nik do obiektu zak�adki, na kt�rej w��czamy przyciski.
			TODO: Nie jestem pewien czy ta metoda dok�adnie to robi.
				Niech to poprawi kto�, kto t� metod� pisa� :)
		**/
// 		void makeVisibleVert(QString t);

		/**
			\fn void resizeEvent(QResizeEvent *)
			Metoda obs�uguj�ca zdarzenie zmiany rozmiaru paska zak�adek.
			\param e wska�nik obiektu opisuj�cego to zdarzenie.
		**/
		void resizeEvent(QResizeEvent *e);

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

		/**
			\var QPtrList<QTab> *lstatic2
			Lista wska�nik�w do obiekt�w zak�adek.
		**/
		Q3PtrList<QString> *lstatic2;

		/**
			\var bool vertscrolls
			Informuje o tym, czy pasek zak�adek jest ju� na tyle d�ugi, ze trzeba go przewija�.
		**/
		bool vertscrolls;

		/**
			\var QToolButton *upB
			Wska�nik do obiektu przycisku przewijaj�cego zak�adki w g�r�.
		**/
		QToolButton *upB;

		/**
			\var QToolButton *downB
			Wska�nik do obiektu przycisku przewijaj�cego zak�adki w d�.
		**/
		QToolButton *downB;
};

#endif
