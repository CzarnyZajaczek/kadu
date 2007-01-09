#ifndef HINT_H
#define HINT_H

#include <qobject.h>
#include <qwidget.h>
#include <qlabel.h>
#include <qcolor.h>
#include <qfont.h>
#include <qevent.h>
#include <qstring.h>
#include <qpixmap.h>
#include <qlayout.h>

#include "gadu.h"

class Hint : public QWidget
{
	Q_OBJECT

	private:
		QVBoxLayout *vbox;

		QHBoxLayout *labels;
		QHBoxLayout *callbacksBox;

		QLabel *icon;
		QLabel *label;
		QColor bcolor; //kolor t�a
		unsigned int secs;
		UserListElements users;

		bool hintClosing;

		void createLabels(const QString &text, const QPixmap &pixmap);

	private slots:
		void close();

	protected:
		virtual void mousePressEvent(QMouseEvent * event);
		virtual void enterEvent(QEvent *event);
		virtual void leaveEvent(QEvent *event);

	public:
		Hint(QWidget *parent, const QString &text, const QPixmap &pixmap, unsigned int timeout);
		/**
			zwraca list� uin�w dotycz�cych tego dymka
		**/
		UserListElements getUsers() const { return users; }

		void getData(QString &text, QPixmap &pixmap, unsigned int &timeout, QFont &font, QColor &fgcolor, QColor &bgcolor);
		bool isDeprecated();

	public slots:
		/**
			min�a sekunda, zmniejsza licznik pozosta�ych sekund,
			zwraca true je�eli jeszcze pozosta� czas
			false, gdy czas si� sko�czy�
		**/
		void nextSecond();
		/**
			gdy show==true pokazuje dymek
		**/
		void setShown(bool show);
		/**
			ustawia parametry dymka
			font - czcionka
			fgcolor - kolor czcionki
			bgcolor - kolor t�a
			id - identyfikator dymka
			show - czy pokaza� (teraz)
		**/
		void set(const QFont &font, const QColor &fgcolor, const QColor &bgcolor, bool doShow = true);
		/**
			ustawia list� uin�w dotycz�cych tego dymka
		**/
		void setUsers(const UserListElements &i) { users = i; };

	signals:
		void leftButtonClicked(Hint *hint);
		void rightButtonClicked(Hint *hint);
		void midButtonClicked(Hint *hint);
		void deleting(Hint *hint);
};

#endif
