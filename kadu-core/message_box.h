#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <qglobal.h>

#include <qdialog.h>
#include <qmap.h>
//Added by qt3to4:
#include <QCloseEvent>
#include <QLabel>

class Q3HBox;
class QLabel;

/**
	Odpowiednik klasy QMessageBox z QT. W zamierzeniu ma
	mie� wi�ksze mo�liwo�ci i wygodniejszy interface ni�
	orygina�.
**/
class MessageBox : public QDialog
{
	Q_OBJECT

	private:
		static QMap<QString,MessageBox*> Boxes;
		QLabel *icon;
		QString message;

		void addButton(QWidget *parent, const QString &caption, const char *slot);

	private slots:
		void okClicked();
		void cancelClicked();
		void yesClicked();
		void noClicked();

	protected:
		void closeEvent(QCloseEvent* e);

	public:
		// komponenty dialogu
		static const int OK;
		static const int CANCEL;
		static const int YES;
		static const int NO;
		//
		MessageBox(const QString& message, int components = 0, bool modal = false, const QString &iconName = QString::null, QWidget *parent = 0);
		~MessageBox();

		//

		/**
			Informuje u�ytkownika o wykonywanej przez
			program czynno�ci. Tworzy dialog zawieraj�cy
			tylko podany tekst. Nie blokuje wykonywania
			programu. Zamkni�cie dialogu nast�puje po
			wywo�aniu funkcji close z t� sam� wiadomo�ci�
			przekazan� jako argument.
			Wywo�uje QApplication::processEvents().
		**/
		static void status(const QString& message);

		/**
			Zadaje u�ytkownikowi pytanie. Tworzy dialog
			z dwoma przyciskami: "Tak" i "Nie". Blokuje
			wykonywanie programu i czeka na reakcj�
			u�ytkownika. Zwraca true je�li wybra� "Tak"
			lub false je�li wybra� "Nie".
		**/
		static bool ask(const QString& message, const QString& iconName = QString::null, QWidget *parent = 0);

		/**
			Przekazuje u�ytkownikowi informacj�. Tworzy
			dialog z przyciskiem: "OK" zamykaj�cy okno.
			Nie blokuje wykonywania programu.
		**/
		static void msg(const QString& message, bool modal=false, const QString& iconName = QString::null, QWidget *parent = 0);

		/**
			Zamyka dialog, kt�ry zosta� stworzony za
			pomoc� funkcji status z t� sam� wiadomo�ci�
			przekazan� jako argument.
		**/
		static void close(const QString& message);

	signals:
		void okPressed();
		void cancelPressed();
		void yesPressed();
		void noPressed();
};

#endif
