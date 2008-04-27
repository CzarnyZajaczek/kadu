#ifndef ABOUT_H
#define ABOUT_H

#include <QWidget>

/**
	\class About
	\brief Okno z informacjami o programie.
**/
class About : public QWidget {
	Q_OBJECT

	QString loadFile(const QString &name);
	void keyPressEvent(QKeyEvent *);

public:
	/**
		\fn About(QWidget *parent = 0)
		Konstruktor tworzy okno, ustawia zakładki z informacjami o autorach, podziękowaniach, licencji oraz listę zmian w programie. \n
		Domyślnie okno ma rozmiar 640x420.
		\param parent rodzic okna
		\param name nazwa obiektu
	**/
	About(QWidget *parent = 0);

	/**
		\fn ~About()
		Destruktor zapisuje rozmiar okna.
	**/
	~About();

};

#endif
