#ifndef HINT_MANAGER_H
#define HINT_MANAGER_H

#include <qframe.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qstring.h>
#include <qfont.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <qptrlist.h>
#include <qpoint.h>

#include "gadu.h"
#include "hint.h"
#include "hint_manager_slots.h"

class HintManager : public QFrame
{
	Q_OBJECT

	private:
		HintManagerSlots *hint_manager_slots;
		QTimer *hint_timer;
		QGridLayout *grid;
		QPtrList<Hint> hints;

		/**
			ustala r�g, od kt�rego b�dzie liczona pozycja grupy dymk�w
		**/
		void setGridOrigin();

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
		void leftButtonSlot(unsigned int id);

		/**
			na dymku o numerze id klikni�to prawym przyciskiem myszy
			w zale�no�ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s� wszystkie
		**/
		void rightButtonSlot(unsigned int id);

		/**
			na dymku o numerze id klikni�to �rodkowym przyciskiem myszy
			w zale�no�ci od konfiguracji otwierana jest nowa rozmowa, dymek jest kasowany lub kasowane s� wszystkie
		**/
		void midButtonSlot(unsigned int id);

	public slots:
/*********** sloty dla notify ************/
		/* nowa rozmowa */
		void newChat(const UinsList &senders, const QString& msg, time_t time);

		/* nowa wiadomo�� w oknie chat */
		void newMessage(const UinsList &senders, const QString& msg, time_t time, bool &grab);

		/* b��d po��czenia */
		void connectionError(const QString &message);

		/* u�ytkownik zmieni� status */
		void userStatusChanged(const UserListElement &ule, const UserStatus &oldStatus);

		/* u�ytkownik zmieni� status na "Dost�pny" */
		void userChangedStatusToAvailable(const UserListElement &ule);

		/* u�ytkownik zmieni� status na "Zaraz wracam" */
		void userChangedStatusToBusy(const UserListElement &ule);

		/* u�ytkownik zmieni� status na "Niewidoczny" lub "Niedost�pny" */
		void userChangedStatusToNotAvailable(const UserListElement &ule);

		/* inna informacja do powiadomienia */
		/* je�eli parameters == NULL, to brane s� domy�lne ustawienia dymk�w typu message
		 * w przeciwnym razie z mapy brane s� nast�puj�ce warto�ci:
		 *		"Pixmap"           - QPixmap
		 *		"Font"             - QFont
		 *		"Foreground color" - QColor
		 *		"Background color" - QColor
		 *		"Timeout"          - unsigned int
		 * je�eli kt�ra� z warto�ci jest niekre�lona, to dla niej brana jest warto�� domy�lna
		 * gdy UserListElement != NULL, to jego uin brany jest do tworzenia dymka (LMB np otwiera okno rozmowy z danym uinem)
		 */
		void message(const QString &from, const QString &message, const QMap<QString, QVariant> *parameters, const UserListElement *ule);
/*********** koniec slot�w dla notify *************/

		/**
			pokazuje nowy dymek o tre�ci "text", obrazku "pixmap",
			czcionk� "font" o kolorze "fgcolor", tle "bgcolor",
			czasie zaniku "timeout" (sekund), dotycz�cy "senders"

			pozosta�e funkcje pokazuj�ce dymki s� tylko opakowaniami na t� funkcj�
		**/
		void addHint(const QString &text, const QPixmap &pixmap, const QFont &font, const QColor &fgcolor, const QColor &bgcolor, unsigned int timeout, const UinsList &senders=UinsList());

		/**
			usuwa dymek o numerze id
		**/
		void deleteHint(unsigned int id);

		/**
			je�eli dymek dotyczy� konkrentej osoby lub grupy os�b, to otwierane jest okno chatu
			dymek jest kasowany
		**/
		void openChat(unsigned int id);

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
		HintManager();
		~HintManager();
};

extern HintManager *hint_manager;

#endif
