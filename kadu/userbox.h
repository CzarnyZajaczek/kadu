#ifndef KADU_USERBOX_H
#define KADU_USERBOX_H

#include <qlistbox.h>
#include <qpainter.h>
#include <qpopupmenu.h>
#include <qstringlist.h>
#include <qtooltip.h>

//#include "misc.h"
#include "userlist.h"

class QFontMetrics;
class UserBoxSlots;
/**
	Klasa reprezentuj�ca kontakt wizualnie na li�cie kontakt�w. Opisuje ona ikon� kontaktu,
	jego wy�witlan� nazw� oraz ewentualny opis.
	\class KaduListBoxPixmap
	\brief Klasa reprezentuj�ca kontakt wizualnie.
**/
class KaduListBoxPixmap : public QListBoxItem
{
	public:
		/**
			\fn KaduListBoxPixmap(const QPixmap &pix, const QString &text)
			Konstruktor dodaj�cy kontakt do listy z podan� ikon� oraz nazw�.
			\param pix ikona statusu.
			\param test wy�wietlana nazwa kontaktu.
		**/
		KaduListBoxPixmap(const QPixmap &pix, const QString &text);

		/**
			\fn KaduListBoxPixmap(const QPixmap &pix, const QString &text, const QString &descr, bool bold)
			Konstruktor dodaj�cy kontakt do listy z podan� ikon�, nazw�, oraz opisem.
			\param pix ikona statusu.
			\param test wy�wietlana nazwa kontaktu.
			\param descr wy�wietlany opis pod nazw� kontaktu.
			\param bold warto�� logiczna informuj�ca o tym, czy nazwa kontaktu
				ma by� wy�wietlana pogrubion� czcionk�.
		**/
		KaduListBoxPixmap(const QPixmap &pix, const QString &text, const QString &descr, bool bold);

		/**
			\fn const QString &description() const
			Zwraca wy�wietlany opis kontaktu.
		**/
		const QString &description() const { return descr; }

		/**
			\fn bool isBold() const
			Zwraca informacj�, czy nazwa kontaktu jest wy�wietlana pogrubion� czcionk�, czy nie.
		**/
		bool isBold() const { return bold; }

		/**
			\fn int height(const QListBox *lb) const
			Zwraca wysoko�� elementu reprezentuj�cego wy�wietlany kontakt w pikselach.
			\param lb obiekt reprezentuj�cy wy�wietlany kontakt.
		**/
		int height(const QListBox *lb) const;

		/**
			\fn int width(const QListBox *lb) const
			Zwraca szeroko�� elementu reprezentuj�cego wy�wietlany kontakt w pikselach.
			\param lb obiekt reprezentuj�cy wy�wietlany kontakt.
		**/
		int width(const QListBox *lb) const;

	protected:
		/**
			\fn int lineHeight(const QListBox *lb) const
			Zwraca wysoko�� samej czcionki dla elementu reprezentuj�cego wy�wietlany kontakt w pikselach.
			\param lb obiekt reprezentuj�cy wy�wietlany kontakt.
		**/
		int lineHeight(const QListBox *lb) const;

		/**
			\fn void paint(QPainter *painter)
			Rysuje wy�wietlany kontakt na li�cie.
			\param painter wska�nik do obiektu rysuj�cego.
		**/
		void paint(QPainter *painter);

		/**
			\fn void setDescription(const QString &d)
			Ustawia wywietlany opis na podany.
			\param d opis do wy�wietlenia.
		**/
		void setDescription(const QString &d) { descr = d; }

		/**
			\fn void setBold(bool b)
			Ustawia stan pogrubienia czcionki wy�wietlanego kontaktu.
			\param b warto�� logiczna okre�laj�ca, czy czcionka ma by� pogrubiona czy nie.
		**/
		void setBold(bool b) { bold = b; }

		/**
			\fn void calculateSize(const QString &text, int width, QStringList &out, int &height) const
			Oblicza odpowiedni� wysoko�� elementu listy kontakt�w tak, aby pomie�ci�
			opis podzielony na niezb�dn� liczb� linii.
			\param[in] text wy�wietlany opis.
			\param[in] width szeroko�� kolumny listy kontakt�w.
			\param[out] out lista �a�cuch�w znak�w, zawieraj�ca kolejne linie opisu powsta�e
				w wyniku podzielenia opisu tak, aby zmie�ci� si� w danej szeroko�ci kolumny.
			\param[out] height wysoko�� elementu listy kontakt�w potrzebna aby pomie�ci� opis
				podzielony w liniach.
		**/
		void calculateSize(const QString &text, int width, QStringList &out, int &height) const;

		/**
			\fn void changeText(const QString &text)
			Zmienia nazw� wy�wietlan� dla kontaktu na li�cie na podan�.
			\param text nowa nazwa do wy�wietlenia.
		**/
		void changeText(const QString &text);
		friend class UserBox;
		friend class UserBoxSlots;

		/*funkcje wprowadzone �eby zaoszcz�dzi� na odwo�aniach do pliku konfiguracyjnego*/
		/**
			\fn static void setFont(const QFont &f)
			Ustawia czcionk� dla tego elementu.
			\param f czcionka
		**/
		static void setFont(const QFont &f);

		/**
			\fn static void setMyUIN(UinType u)
			Ustawia UIN lokalnego u�ytkownika Kadu dla tego elementu, aby by�o wiadomo,
			�e ten element jego reprezentuje (poniewa� do pobierania informacji o statusie i opisie
			u�ywane s� r�ne metody dla kontaktu i lokalnego u�ytkownika).
			\param u numer UIN.
		**/
		static void setMyUIN(UinType u);

		/**
			\fn static void setShowDesc(bool sd)
			Ustawia stan wy�wietlania opis�w na li�cie kontakt�w.
			\param sd warto�� logiczna informuj�ca o tym, czy opisy maj� by� wy�wietlane na li�cie kontat�w.
		**/
		static void setShowDesc(bool sd);

		/**
			\fn static void setAlignTop(bool at)
			Ustawia stan wyr�wnywania do g�rnego brzegu elementu.
			\param at warto�� logiczna informuj�ca o tym, czy wy�wietlana nazwa kontaktu ma by� wyr�wnywana
				do g�rnej kraw�dzi elementu.
		**/
		static void setAlignTop(bool at);

		/**
			\fn static void setShowMultilineDesc(bool m)
			Ustawia stan wy�wietlania opis�w wieloliniowych.
			\param m warto�� logiczna informuj�ca o tym, czy opis tego elementu mo�e by� wy�wietlany
				w wielu liniach, je�li zawiera znaki nowej linii.
		**/
		static void setShowMultilineDesc(bool m);

		/**
			\fn static void setMultiColumn(bool m)
			Ustawia stan wy�wietlania listy kontakt�w w wielu kolumnach.
			\param m warto�� logiczna informuj�ca o tym, czy lista ma by� wy�wietlana w wielu kolumnach.
		**/
		static void setMultiColumn(bool m);

		/**
			\fn static void setMultiColumnWidth(int w)
			Ustawia szeroko�� jednej kolumny dla wy�wietlania listy w trybie wielu kolumn.
			\param w szeroko�� kolumny w pikselach.
		**/
		static void setMultiColumnWidth(int w);

		/**
			\fn static void setDescriptionColor(const QColor &col)
			Ustawia kolor wy�wietlanego opisu.
			\param col kolor
		**/
		static void setDescriptionColor(const QColor &col);

	private:
		QPixmap pm;
		QString descr;
		bool bold;
		static QFontMetrics *descriptionFontMetrics;

		static UinType myUIN;
		static bool ShowDesc;
		static bool AlignUserboxIconsTop;
		static bool ShowMultilineDesc;
		static bool MultiColumn;
		static int  MultiColumnWidth;
		static QColor descColor;
		
		mutable QString buf_text;
		mutable int buf_width;
		mutable QStringList buf_out;
		mutable int buf_height;
};

/**
	Klasa reprezentuj�ca menu podr�czne listy kontakt�w - czyli to, kt�re dostajemy klikaj�c
	na kt�rymkolwiek kontakcie prawym przyciskiem myszy.
	\class UserBoxMenu
	\brief Menu podr�czne listy kontakt�w.
**/
class UserBoxMenu : public QPopupMenu
{
	Q_OBJECT

	private:
		QValueList<QPair<QString, QString> > iconNames;
	private slots:
		void restoreLook();

	public:
		/**
			\fn UserBoxMenu(QWidget *parent=0, const char* name=0)
			Standardowy konstruktor.
			\param parent wska�nik na obiekt kontrolki-rodzica.
			\param name nazwa kontrolki.
		**/
		UserBoxMenu(QWidget *parent=0, const char* name=0);

		/**
			\fn int getItem(const QString &caption) const
			Zwraca unikaln� liczb� identyfikuj�c� element menu z podanym napisem.
			\param caption napis elementu.
		**/
		int getItem(const QString &caption) const;
	
	public slots:
		/**
			\fn void show(QListBoxItem *item)
			Pokazuje menu dla podanego kontaktu.
			\param item element wy�wietlanej listy kontakt�w reprezentuj�cy rz�dany kontakt.
		**/
		void show(QListBoxItem *item);

		/**
			\fn int addItem(const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1)
			Dodaje nowy element do menu.
			\param text napis dla nowego elementu.
			\param receiver obiekt odbieraj�cy sygna� wybrania elementu z menu.
			\param member SLOT obiektu \a receiver kt�ry zostanie wykonany po wybraniu elementu z menu.
			\param accel skr�t klawiaturowy dla tego elementu. Domy�lnie brak.
			\param id Unikatowa liczba identyfikuj�ca nowy element. Domy�lnie pierwsza wolna.
		**/
		int addItem(const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);

		/**
			\fn int addItem(const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1)
			Dodaje nowy element do menu.
			\param iconname nazwa ikony z zestawu lub sciezka do pliku
			\param text napis dla nowego elementu.
			\param receiver obiekt odbieraj�cy sygna� wybrania elementu z menu.
			\param member SLOT obiektu \a receiver kt�ry zostanie wykonany po wybraniu elementu z menu.
			\param accel skr�t klawiaturowy dla tego elementu. Domy�lnie brak.
			\param id Unikatowa liczba identyfikuj�ca nowy element. Domy�lnie pierwsza wolna.
		**/
		int addItem(const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);

		/**
			\fn int addItemAtPos(int index,const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1)
			Dodaje nowy element do menu.
			\param index pozycja (licz�c od 0) na kt�rej znale�� ma si� nowy element.
			\param iconname nazwa ikony z zestawu lub sciezka do pliku
			\param text napis dla nowego elementu.
			\param receiver obiekt odbieraj�cy sygna� wybrania elementu z menu.
			\param member SLOT obiektu \a receiver kt�ry zostanie wykonany po wybraniu elementu z menu.
			\param accel skr�t klawiaturowy dla tego elementu. Domy�lnie brak.
			\param id Unikatowa liczba identyfikuj�ca nowy element. Domy�lnie pierwsza wolna.
		**/
		int addItemAtPos(int index,const QString &iconname, const QString &text, const QObject* receiver, const char* member, const QKeySequence accel= 0, int id= -1);
		
		/**
			\fn void refreshIcons()
			Prze�adowuje wszystkie ikony w tym menu dodane przy pomocy powy�szych funkcji 
		**/
		void refreshIcons();
	signals:
		/**
			\fn void popup()
			Sygna� emitowany, gdy menu jest wywo�ywane.
		**/
		void popup();
};


/**
	Klasa reprezentuj�ca list� kontakt�w wraz z ikonkami stan�w.
	\class UserBox
	\brief Wy�wietlana lista kontakt�w.
**/
class UserBox : public QListBox , QToolTip
{
	Q_OBJECT

	private:
		static QValueList<UserBox*> UserBoxes;
		static UserBoxSlots *userboxslots;
		// Pixmaps
		static QPixmap* OnlineMobilePixmap;
		static QPixmap* OnlineDescMobilePixmap;
		static QPixmap* BusyMobilePixmap;
		static QPixmap* BusyDescMobilePixmap;
		static QPixmap* OfflineMobilePixmap;
		static QPixmap* OfflineDescMobilePixmap;		
		//
		QStringList Users;
		void sortUsersByAltNick(QStringList &);
			
	protected:
		/**
			\fn virtual void maybeTip(const QPoint& c)
			Wy�wietla dymek w danym punkcie z informacj� o kontakcie znajduj�cym si� w tym punkcie
			wzgl�dem lewego g�rnego rogu listy kontakt�w.
			\param c wsp�rz�dne punktu
		**/
		virtual void maybeTip(const QPoint&);

		/**
			\fn virtual void mousePressEvent(QMouseEvent *e)
			Wci�ni�to kt�ry� z przycisk�w myszki na li�cie kontakt�w.
			\param e wska�nik obiektu opisuj�cego to zdarzenie.
		**/
		virtual void mousePressEvent(QMouseEvent *e);

		/**
			\fn virtual void mouseMoveEvent(QMouseEvent* e)
			Poruszono myszk� nad list� kontakt�w.
			\param e wska�nik obiektu opisuj�cego to zdarzenie.
		**/
		virtual void mouseMoveEvent(QMouseEvent* e);

		/**
			\fn virtual void keyPressEvent(QKeyEvent *e)
			Wci�ni�to kt�ry� z klawisz�w w aktywnej li�cie kontakt�w.
			\param e wska�nik obiektu opisuj�cego to zdarzenie.
		**/
		virtual void keyPressEvent(QKeyEvent *e);

		/**
			\fn virtual void resizeEvent(QResizeEvent *)
			Lista kontakt�w zmieni�a sw�j rozmiar.
			\param e wska�nik obiektu opisuj�cego to zdarzenie.
		**/
		virtual void resizeEvent(QResizeEvent *);

	public:
		/**
			\fn UserBox(QWidget* parent=0, const char* name=0, WFlags f=0)
			Standardowy konstruktor tworz�cy list� kontakt�w.
			\param parent rodzic kontrolki. Domy�lnie 0.
			\param name nazwa kontrolki. Domy�lnie 0.
			\param f flagi kontrolki. Domy�lnie 0.
		**/
		UserBox(QWidget* parent=0,const char* name=0,WFlags f=0);
		~UserBox();

		/**
			\var static UserBoxMenu *userboxmenu
			Wska�nik do menu kontekstowego listy kontakt�w.
		**/
		static UserBoxMenu *userboxmenu;

		/**
			\fn bool containsAltNick(const QString &altnick) const
			Funkcja sprawdza czy dany altnick, znajduje si� na liscie w UserBox.
		**/
		bool containsAltNick(const QString &altnick) const;

		/**
			\fn static void initModule()
			Inicjalizuje zmienne niezb�dne do dzia�ania UserBox. Funkcja ta jest
			wywo�ywana przy starcie Kadu przez rdze� Kadu.
		**/
		static void initModule();
		static void closeModule();
		/**
			\fn UinsList getSelectedUins() const
			Funkcja zwraca list� zaznaczonych u�ytkownik�w.
			\return UinsList z zaznaczonymi u�ytkownikami posiadaj�cymi numer GG
		**/
		UinsList getSelectedUins() const;

		/**
			\fn UserList getSelectedUsers() const
			Funkcja zwraca list� zaznaczonych uzytkownik�w.
			\return UserList z zaznaczonymi u�ytkownikami.
		**/
		UserList getSelectedUsers() const;

		/**
			\fn QStringList getSelectedAltNicks() const
			Funkcja zwraca list� zaznaczonych uzytkownik�w.
			\return QStringList z pseudonimami wy�wietlanymi (altNick'ami).
		**/
		QStringList getSelectedAltNicks() const;

		/**
			\fn static UserBox* getActiveUserBox()
			Funkcja znajdujaca aktywny UserBox.
			\return wska�nik do aktywnego UserBox'a, je�li taki nie istnieje zwracana jest warto�� NULL.
		**/
		static UserBox* getActiveUserBox();

	public slots:
		/**
			\fn void refresh()
			Od�wierza list� (rysuje od nowa).
		**/
		void refresh();

		/**
			\fn void showHideInactive()
			Prze��cza tryb pokazywania kontakt�w niedost�pnych.
		**/
		void showHideInactive();

		/**
			\fn void showHideDescriptions()
			Prze��cza tryb pokazywania opis�w kontakt�w.
		**/
		void showHideDescriptions();

		/**
			\fn virtual void clear()
			Czy�li list�.
		**/
		virtual void clear() { QListBox::clear(); };

		/**
			\fn void clearUsers()
			Funkcja czy�ci ca�� list� u�ytkownik�w w UserBox.
		**/
		void clearUsers() { Users.clear(); };

		/**
			\fn void addUser(const QString &altnick)
			Funkcja dodaje u�ytkownika o podanym pseudonimie wy�wietlanym (altNicku) do listy w UserBox.
		**/
		void addUser(const QString &altnick);

		/**
			\fn void removeUser(const QString &altnick)
			Funkcja usuwa u�ytkownika o podanym pseudonimie wy�wietlanym (altNick) z listy w UserBox.
		**/
		void removeUser(const QString &altnick);

		/**
			\fn void renameUser(const QString &oldaltnick, const QString &newaltnick)
			Funkcja zmienia pseudonim wy�wietlany z podanego na inny w li�cie UserBox.
		**/
		void renameUser(const QString &oldaltnick, const QString &newaltnick);

		/**
			\fn void changeAllToInactive()
			Ustawia statusy dla wszystkich kontakt�w na li�cie na niedost�pny.
			\note Chodzi tu tylko o te widoczne statusy, rzeczywiste statusy kontakt�w nadal
			b�d� znanie poprawnie w instancjach UserListElement.
		**/
		void changeAllToInactive();

		// Functions below works on all created userboxes
		/**
			\fn static void all_refresh()
			Wykonuje metod� UserBox::refresh() na wszystkich istniej�cych instancjach UserBox.
		**/
		static void all_refresh();

		/**
			\fn static void all_removeUser(const QString &altnick)
			Funkcja usuwa u�ytkownika o podanym pseudonimie wy�wietlanym (altNick) z wszystkich
			instancji UserBox.
		**/
		static void all_removeUser(const QString &altnick);

		/**
			\fn static void all_changeAllToInactive()
			Wykonuje metod� UserBox::changeAllToInactive() na wszystkich istniej�cych instancjach
			UserBox.
		**/
		static void all_changeAllToInactive();

		/**
			\fn static void all_renameUser(const QString &oldaltnick, const QString &newaltnick)
			Funkcja zmienia pseudonim wy�wietlany z podanego na inny we wszystkich instancjach UserBox.
		**/
		static void all_renameUser(const QString &oldaltnick, const QString &newaltnick);

	signals:
		/**
			\fn void doubleClicked(const QString &text)
			Sygna� jest emitowany, gdy na kt�rym� z kontakt�w klikni�to dwukrotnie.
			\param text pseudonim wy�wietlany (altNick) kontaktu na kt�rym dwu-kilkni�to.
			\warning U�ywaj tego sygna�u zamiast QListBox::doubleClicked(QListBoxItem *) !!!
			Tamten ze wzgl�du na od�wie�anie listy w jednym ze slot�w pod��czonych
			do tego sygna�u czasami przekazuje wska�nik do elementu, kt�ry ju� NIE ISTNIEJE.
		**/
		void doubleClicked(const QString &text);

		/**
			\fn void returnPressed(const QString &text)
			Sygna� jest emitowany, gdy wci�ni�to klawisz enter na wybranym kontakcie.
			\param text pseudonim wy�wietlany (altNick) kontaktu na kt�rym wci�ni�to enter.
			\warning U�ywaj tego sygna�u zamiast QListBox::returnPressed(QListBoxItem *) !!!
			Tamten ze wzgl�du na od�wie�anie listy w jednym ze slot�w pod��czonych
			do tego sygna�u czasami przekazuje wska�nik do elementu, kt�ry ju� NIE ISTNIEJE.
		**/
		void returnPressed(const QString &text);
	private slots:
		void doubleClickedSlot(QListBoxItem *item);
		void returnPressedSlot(QListBoxItem *item);
};

/**
	Klasa ta pozwala na podgl�d wprowadzanych zmian konfiguracyjnych wobec UserBox
	w oknie konfiguracji, zanim zaaplikuje si� je do Kadu.
	\class UserBoxSlots
	\brief Obs�uga UserBox w konfiguracji.
**/
class UserBoxSlots : public QObject
{
	Q_OBJECT
	public slots:
		/**
			\fn void onCreateConfigDialog()
			Obs�uguje sekcj� UserBox podczas otwierania okna konfiguracji.
		**/
		void onCreateConfigDialog();

		/**
			\fn void onDestroyConfigDialog()
			Obs�uguje sekcj� UserBox podczas zamykania okna konfiguracji.
		**/
		void onDestroyConfigDialog();

		/**
			\fn void chooseColor(const char *name, const QColor& color)
			Od�wierza podgl�d wybranego koloru.
			\param name nazwa elementu, dla kt�rego wybrano kolor.
				\arg \c userbox_bg_color oznacza kolor t�a.
				\arg \c userbox_font_color oznacz kolor czcionki.
			\param color wybrany kolor.
		**/
		void chooseColor(const char *name, const QColor& color);

		/**
			\fn void chooseFont(const char *name, const QFont& font)
			Od�wierza podgl�d wybranej czcionki.
			\param name nazwa elementu, dla kt�rego wybrano czcionk�.
				\arg \c userbox_font_box oznacza og�ln� czcionk� listy kontakt�w.
			\param font wybrana czcionka.
		**/
		void chooseFont(const char *name, const QFont& font);

		/**
			\fn void onMultiColumnUserbox(bool toggled)
			W��cza b�d� wy��cza kontrolk� szeroko�ci kolumny listy kontakt�w, w zale�no�ci od podanego argumentu.
			\param togglet warto�� logiczna informuj�ca, czy kontrolka ma by� w��czona czy wy��czona.
		**/
		void onMultiColumnUserbox(bool toggled);

		/**
			\fn void updatePreview()
			Od�wierza podgl�d wszystkich element�w UserBox'a.
		**/
		void updatePreview();
};

#endif
