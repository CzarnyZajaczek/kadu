#ifndef ABOUT_H
#define ABOUT_H

#include <qhbox.h>

/**
	\class About 
	\brief Okno z informacjami o programie.
**/
class About : public QHBox {
	Q_OBJECT

	public:
	
	/**
		\fn About(QWidget *parent=NULL, const char *name=NULL)
		Konstruktor tworzy okno, ustawia zak�adki z informacjami o autorach, podzi�kowaniach, licencji oraz list� zmian w programie. \n
		Domy�lnie okno ma rozmiar 640x420.
		\param parent rodzic okna
		\param name nazwa obiektu
	**/
		About(QWidget *parent=NULL, const char *name=NULL);
	
	/**
		\fn ~About()
		Destruktor zapisuje rozmiar okna.
	**/
		~About();

	private:
	/**
		\fn QString loadFile(const QString &name)
		Funkcja zwracaj�ca zawarto�� pliku.
		\param name nazwa pliku do przeczytania
	**/
		QString loadFile(const QString &name);
	
	/**
		\fn void keyPressEvent(QKeyEvent *)
		Funkcja obs�uguj�ca zdarzenie naci�ni�cia przycisku 
		Esc do zamkni�cia okna.
	**/
		void keyPressEvent(QKeyEvent *);
};

#endif
