#ifndef HINT_MANAGER_H
#define HINT_MANAGER_H

#include <qcolor.h>
#include <qfont.h>
#include <qframe.h>
#include <qlayout.h>
#include <qmap.h>
#include <qpair.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qtimer.h>

#include "gadu.h"
#include "hint.h"
#include "hint_manager_slots.h"
#include "userbox.h"
#include "../notify/notify.h"

class HintManager : public Notifier, public ToolTipClass
{
	Q_OBJECT

	private:
		QFrame *frame;
		QVBoxLayout *layout;
		HintManagerSlots *hint_manager_slots;
		QTimer *hint_timer;
		QPtrList<Hint> hints;
		QFrame *tipFrame;

		QMap<QPair<UserListElements, QString>, Hint *> linkedHints;

		/**
			ustala r�g, od kt�rego b�dzie liczona pozycja grupy dymk�w
		**/
		void setLayoutDirection();

		void processButtonPress(const QString &buttonName, Hint *hint);

		void showNewMessage(const QString &configurationDirective, const QString &title, const QString &contentTitle, UserListElements senders, const QString &msg);

		// TODO: usun�� w 0.6
		void realCopyConfiguration(const QString &fromHint, const QString &toHint);
		void import_0_5_0_Configuration();
		void import_0_5_0_Configuration_fromTo(const QString &from, const QString &to,
			const QString &syntax = QString::null, const QString &detailSyntax = QString::null);

	private slots:
		/**
			min�a sekunda: aktualizuje czasy wszystkich dymk�w
			usuwa te, kt�rym min�� czas
		**/
		void oneSecond();

		/**
			przesuwa ca�� grup� hint�w zgodnie z preferowanym rozmiarem grupy
			i wykryt�/okre�lon� w konfiguracji pozycj� traya
		**/
		void setHint();

		/**
			na dymku o numerze id klikni�to lewym przyciskiem myszy
			w zale�no�ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s� wszystkie
		**/
		void leftButtonSlot(Hint *hint);

		/**
			na dymku o numerze id klikni�to prawym przyciskiem myszy
			w zale�no�ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s� wszystkie
		**/
		void rightButtonSlot(Hint *hint);

		/**
			na dymku o numerze id klikni�to �rodkowym przyciskiem myszy
			w zale�no�ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s� wszystkie
		**/
		void midButtonSlot(Hint *hint);

		void deleteHint(Hint *hint);
		void deleteHintAndUpdate(Hint *hint);

		void notificationClosed(Notification *notification);

	public slots:
/*********** sloty dla notify ************/

		/* inna informacja do powiadomienia */
		/* je�eli parameters == NULL, to brane s� domy�lne ustawienia dymk�w typu message
		 * w przeciwnym razie z mapy brane s� nast�puj�ce warto�ci:
		 *		"Pixmap"           - QPixmap
		 *		"Font"             - QFont
		 *		"Foreground color" - QColor
		 *		"Background color" - QColor
		 *		"Timeout"          - unsigned int
		 *		"ShowSource"       - bool
		 * je�eli kt�ra� z warto�ci jest nieokre�lona, to dla niej brana jest warto�� domy�lna
		 * gdy UserListElement != NULL, to jego uin brany jest do tworzenia dymka (LMB np otwiera okno rozmowy z danym uinem)
		 */
		void message(const QString &from, const QString &message, const QMap<QString, QVariant> *parameters, const UserListElement *ule);
		virtual void externalEvent(Notification *notification);
		virtual void copyConfiguration(const QString &fromEvent, const QString &toEvent);
/*********** koniec slot�w dla notify *************/

		/**
			pokazuje nowy dymek o tre�ci "text", obrazku "pixmap",
			czcionk� "font" o kolorze "fgcolor", tle "bgcolor",
			czasie zaniku "timeout" (sekund), dotycz�cy "senders"

			pozosta�e funkcje pokazuj�ce dymki s� tylko opakowaniami na t� funkcj�
		**/
		void addHint(const QString &text, const QPixmap &pixmap, const QFont &font, const QColor &fgcolor, const QColor &bgcolor, unsigned int timeout, const UserListElements &senders = UserListElements());
		void addHint(const QString &text, const QPixmap &pixmap, const QString &configurationDirective, const UserListElements &senders = UserListElements());
		Hint *addHint(Notification *notification);

		/**
			je�eli dymek dotyczy� konkrentej osoby lub grupy os�b, to otwierane jest okno chatu
			dymek jest kasowany
		**/
		void openChat(Hint *hint);

		/**
			usuwa wszystkie dymki
		**/
		void deleteAllHints();


	signals:
		/**
			HintManager szuka pozycji traya, aby wy�wietli� w jego okolicy dymki
		**/
		void searchingForTrayPosition(QPoint& pos);

	public:
		HintManager(QWidget *parent=0, const char *name=0);
		~HintManager();

		virtual CallbackCapacity callbackCapacity() { return CallbackSupported; }

		virtual void showToolTip(const QPoint &, const UserListElement &);
		virtual void hideToolTip();

};

extern HintManager *hint_manager;

#endif
