#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <qdialog.h>
#include <qmap.h>
#include <qprogressbar.h>

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
		QProgressBar* Progress;
		
	private slots:
		void okClicked();
		void cancelClicked();
		void yesClicked();
		void noClicked();
	
	public:
		// komponenty dialogu
		static const int OK;
		static const int CANCEL;
		static const int YES;
		static const int NO;
		static const int PROGRESS;
		//
		MessageBox(const QString& message,int components = 0
			,bool modal=false);
		void setTotalSteps(int s);
		void setProgress(int p);
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
		static bool ask(const QString& message);
		/**
			Informuje u�ytkownika o post�pie wykonywanej
			przez program czynno�ci. Tworzy dialog zawieraj�cy
			podany tekst, pasek post�pu o podanej
			maksymalnej warto�ci i opcjonalnie przycisk Anuluj
			pod��czony do podanego slot'u w podanym obiekcie.
			Nie blokuje wykonywania programu. Zamkni�cie dialogu
			nast�puje po wywo�aniu funkcji close z t� sam�
			wiadomo�ci� przekazan� jako argument.
			Wywo�uje QApplication::processEvents().
		**/				
		static void progress(const QString& message,
			const QObject* receiver=0,const char* slot=0,
			int total_steps=100);
		/**
			Ustawia podan� warto�� paska post�pu w dialogu
			stworzonym za pomoc� poprzedniej funkcji z t�
			sam� wiadomo�ci� przekazan� jako argument.
			Wywo�uje QApplication::processEvents().
		**/
		static void progress(const QString& message,int progress);
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
