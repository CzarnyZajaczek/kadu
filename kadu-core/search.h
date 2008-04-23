#ifndef KADU_SEARCH_H
#define KADU_SEARCH_H

#include <QCloseEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QResizeEvent>

#include "gadu.h"
#include "kadu_main_window.h"
#include "toolbar.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QLineEdit;
class Q3ListView;
class Q3ListViewItem;
class QPushButton;
class QRadioButton;

// TODO: a better name
class SearchActionsSlots : public QObject
{
	Q_OBJECT

public slots:
	void firstSearchActionActivated(QAction *sender, bool toggled);
	void nextResultsActionActivated(QAction *sender, bool toggled);
	void stopSearchActionActivated(QAction *sender, bool toggled);
	void clearResultsActionActivated(QAction *sender, bool toggled);
	void addFoundActionActivated(QAction *sender, bool toggled);
	void chatFoundActionActivated(QAction *sender, bool toggled);

};

/**
	Klasa ta reprezentuje okno dialogowe wyszukiwania w katalogu publicznym.
	\brief Wyszukiwanie w katalogu publicznym.
	\class SearchDialog
**/
class SearchDialog : public KaduMainWindow {
	Q_OBJECT

	friend class SearchActionsSlots;
	static SearchActionsSlots *searchActionsSlot;

	static ActionDescription *firstSearchAction;
	static ActionDescription *nextResultsAction;
	static ActionDescription *stopSearchAction;
	static ActionDescription *clearResultsAction;
	static ActionDescription *addFoundAction;
	static ActionDescription *chatFoundAction;

	QCheckBox *only_active;
	QLineEdit *e_uin;
	QLineEdit *e_name;
	QLineEdit *e_nick;
	QLineEdit *e_byrFrom;
	QLineEdit *e_byrTo;
	QLineEdit *e_surname;
	QComboBox *c_gender;
	QLineEdit *e_city;
	Q3ListView *results;
	QLabel *progress;
	QRadioButton *r_uin;
	QRadioButton *r_pers;
	UinType _whoisSearchUin;
	uint32_t seq;
	UserGroup *selectedUsers;

	SearchRecord *searchRecord;

	bool searchhidden;
	bool searching;
	bool workaround; // TODO: remove

	bool isPersonalDataEmpty() const;

	UserListElements selected();

private slots:
	void uinTyped(void);
	void personalDataTyped(void);
	void byrFromDataTyped(void);
	void persClicked();
	void uinClicked();
	void updateInfoClicked();
// 	void actionsAddedToToolbar(ToolButton*, ToolBar*);
// 	void stopSearchActionAddedToToolbar(ToolButton*, ToolBar*);
// 	void firstSearchActionAddedToToolbar(ToolButton*, ToolBar*);
// 	void nextResultsActionAddedToToolbar(ToolButton*, ToolBar*);
// 	void clearResultsActionAddedToToolbar(ToolButton*, ToolBar*);
	void selectionChanged();

protected:
	/**
		\fn void closeEvent(QCloseEvent * e)
		Obs�uguje zdarzenie zamkni�cia okna wyszukiwania w katalogu publicznym.
		\param e wska�nik do obiektu opisuj�cego zdarzenie zamkni�cie okna.
	**/
	virtual void closeEvent(QCloseEvent *e);
	virtual void resizeEvent(QResizeEvent *e);
	virtual void keyPressEvent(QKeyEvent *e);

public:
	/**
		\fn SearchDialog(QWidget *parent=0, const char *name=0, UinType whoisSearchUin = 0)
		Standardowy konstruktor.
		\param parent rodzic kontrolki. Domy�lnie 0.
		\param name nazwa kontrolki. Domy�lnie 0.
		\param whoisSearchUin warto�� logiczna informuj�ca o tym, czy wst�pnie ma by� wybrane
		wyszukiwanie po numerze UIN (1) czy po danych osobowych (0). Domy�lnie 0.
	**/
	SearchDialog(QWidget *parent=0, const char *name=0, UinType whoisSearchUin = 0);
	~SearchDialog(void);

	static void initModule();
	static void closeModule();

	virtual bool supportsActionType(ActionDescription::ActionType type) { return type & ActionDescription::TypeSearch; }
	virtual UserBox * getUserBox() { return 0; }
	virtual UserListElements getUserListElements() { return UserListElements(); }
	virtual ChatWidget * getChatWidget() { return 0; }

	/**
		\fn void firstSearch()
		Czy�ci list� wynik�w, a nast�pnie wyszukuje w katalogu publicznym wg.
		podanych w oknie danych. Wy�wietla tylko ograniczon� ich liczb�, ze wzgl�du
		na dzia�anie protoko�u Gadu-Gadu. Metoda ta wywo�ywana jest przy wci�ni�ciu
		przycisku "Szukaj". Aby uzyska� kolejne wyniki i doda� je do
		bierz�cych, nale�y dokona� wt�rnego zapytania metod� SearchDialog::nextSearch.
	**/
	void firstSearch();

	/**
		\fn void nextSearch()
		Kontynuuje wyszukowanie kolejnych kontakt�w, a wyniki dodaje do bierz�cych.
		Metoda ta wywo�ywana jest przy wci�ni�ciu przycisku "Nast�pne wyniki".
	**/
	void nextSearch();

	/**
		\fn void stopSearch(void)
		Zatrzymuje aktualne wyszukiwanie. Je�li w p�niejszym czasie zwr�cone
		zostan� jakie� wyniki, b�d� one zignorowane.
	**/
	void stopSearch();

	void clearResults();

	void addFound();
	void chatFound();

public slots:
	/**
		\fn void newSearchResults(SearchResults& searchResults, int seq, int fromUin)
		Interpretuje uzyskane wyniki wyszukiwania i dodaje je do listy wynik�w.
		Metoda ta jest wywo�ywana, gdy serwer Gadu-Gadu odpowie na zapytanie do katalogu publicznego.
		\param searchResults lista struktur opisuj�cych wyniki wyszukiwania.
		\param seq unikalny identyfikator zapytania do katalogu publicznego.
		\param fromUin numer UIN, od kt�rego rozpocz�to wyszukiwanie (jest r�ny dla kolejnych
		wywo�a� - najpierw SearchDialog::firstSearch, a potem kolejne SearchDialog::nextSearch).
	**/
	void newSearchResults(SearchResults& searchResults, int seq, int fromUin);

};

#endif

