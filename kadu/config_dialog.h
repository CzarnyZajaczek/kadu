#ifndef CONFIG_DIALOG_H
#define CONFIG_DIALOG_H

#include <qdict.h>
#include <qhbox.h>
#include <qkeysequence.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qvbox.h>

#include "config_file.h"

class QApplication;
class QCheckBox;
class QComboBox;
class QGrid;
class QHButtonGroup;
class QHGroupBox;
class QLabel;
class QListBox;
class QListView;
class QScrollView;
class QSlider;
class QSpinBox;
class QTextEdit;
class QVGroupBox;
class QVButtonGroup;

/**
    Kontrolka umo�liwiajaca wybranie skr�tu klawiszowego
**/
class HotKey : public QLineEdit
{
	public:
		HotKey(QWidget *parent =0, const char* name =0);
		/**
		  Pobiera skr�t klawiszowy
		**/
		QString getShortCutString() const;

		/**
		  Pobiera skr�t klawiszowy
		**/
		QKeySequence getShortCut() const;

		/**
		  Ustawia skr�t klawiszowy
		**/
		void setShortCut(const QString& shortcut);

		/**
		  Ustawia skr�t klawiszowy
		**/
		void setShortCut(const QKeySequence& shortcut);

		/**
		 Zwraca skr�t klawiszowy jaki zosta� naci�ni�ty przy zdarzeniu QKeyEvent
		**/
		static QString keyEventToString(QKeyEvent *e);

		/**
		 Pobiera skr�t klawiszowy, z pliku konfiguracyjnego
		 z grupy "groupname", o polu "name"
		**/
		static QKeySequence shortCutFromFile(const QString& groupname, const QString &name);

		/**
		 Sprawdza czy skr�t naci�ni�ty przy zdarzeniu QKeyEvent zgadza si�
		 ze skr�tem klawiszowym, z pliku konfiguracyjnego, o polu "name"
		**/
		static bool shortCut(QKeyEvent *e, const QString& groupname, const QString &name);
	protected:
		virtual void keyPressEvent(QKeyEvent *e);
		virtual void keyReleaseEvent(QKeyEvent *e);
};
/**
    Kontrolka do wyboru koloru
**/
class ColorButton : public QPushButton
{
	Q_OBJECT
	private:
		QColor actualcolor;
	public:
		/**
			Konstruktor obiektu w kt�rym ustawia sie kolor "color",
			kt�ry ma rodzica "parent" oraz nazywa sie "name"
		**/
		ColorButton(const QColor &color, QWidget *parent =0, const char* name =0);

		/**
			Pobiera kolor z przycisku
		**/
		const QColor &color() const;

	private slots:
		void onClick();
	public slots:
		/**
			Ustawia kolor na przycisku
		**/
		void setColor(const QColor &color);
	signals:
		/**
			Sygna� jest emitowany przy zmianie koloru, przesy�a on nazw� komponentu oraz nowy kolor
		**/
		void changed(const char *name, const QColor& color);
};

class SelectFont : public QHBox
{
	Q_OBJECT
	public:
		SelectFont(const QString &text, const QFont &def_val, QWidget *parent=0,  const char *name=0, const QString &tip="");
		const QFont &font() const;
	public slots:
		void setFont(const QFont &font);
	private slots:
		void onClick();
	signals:
		/**
			Sygna� jest emitowany przy zmianie czcionki, przesy�a on nazw� komponentu oraz now� czcionk�
		**/
		void changed(const char *name, const QFont& font);
	private:
		QFont currentFont;
		QLineEdit *fontEdit;
};

class SelectPaths : public QHBox
{
	Q_OBJECT
	protected:
		QStringList releaseList;
		QListBox *pathListBox;
		QPushButton *pb_ok, *pb_cancel;
		QPushButton *pb_add, *pb_change, *pb_remove;
		QLineEdit *pathEdit;
		QPushButton *pb_choose;
	protected slots:
		void okButton();
		void cancelButton();
		void addPath();
		void replacePath();
		void deletePath();
		void choosePath();
		void closeEvent(QCloseEvent *e);
		void keyPressEvent(QKeyEvent *e);
	public:
		SelectPaths(QWidget *parent =0, const char* name =0);
		~SelectPaths();
		/**
			Pobiera list� �cie�ek katalog�w
		**/
		const QStringList &getPathList() const;

	public slots:
		/**
			Ustawia �cie�ki z katalogami
		**/
		void setPathList(const QStringList& list);

	signals:

		/**
			Sygna� jest emitowany przy zamkni�ciu okna przez "ok",
			przesy�a on zmienion� list� �cie�ek katalog�w
	    **/
		void changed(const QStringList& paths);
};

enum UserLevel {
	Beginner,
	Advanced,
	Expert
};

/**
	Okno z konfiguracj� Kadu
**/
class ConfigDialog : public QVBox
{
	Q_OBJECT

	private:

		enum RegisteredControlType
		{
			CONFIG_CHECKBOX,
			CONFIG_COLORBUTTON,
			CONFIG_COMBOBOX,
			CONFIG_GRID,
			CONFIG_HBOX,
			CONFIG_HGROUPBOX,
			CONFIG_HRADIOGROUP,
			CONFIG_HOTKEYEDIT,
			CONFIG_LABEL,
			CONFIG_LINEEDIT,
			CONFIG_LINEEDIT2,
			CONFIG_TEXTEDIT,
			CONFIG_LISTBOX,
			CONFIG_LISTVIEW,
			CONFIG_PUSHBUTTON,
			CONFIG_RADIOGROUP,
			CONFIG_SELECTPATHS,
			CONFIG_SLIDER,
			CONFIG_SPINBOX,
			CONFIG_TAB,
			CONFIG_VBOX,
			CONFIG_VGROUPBOX,
			CONFIG_VRADIOGROUP,
			CONFIG_SELECTFONT,
			CONFIG_NULL
		};

		struct ElementConnections
		{
			ElementConnections(QString signal, const QObject *receiver, QString slot);
			ElementConnections();
			QString signal;
			const QObject* receiver;
			QString slot;
			bool operator== (const ElementConnections& r) const;
		};
		struct RegisteredControl
		{
			RegisteredControlType type;
			RegisteredControl(RegisteredControlType t=CONFIG_NULL,
				const QString &groupname=QString::null,
				const QString &parent=QString::null,
				const QString &caption=QString::null,
				const QString &name=QString::null,
				const UserLevel userLevelRequired = Beginner);
			QString parent;
			QString name;
			QString caption;
			QString group;
			QString entry;
			QString defaultS;
			QString tip;

			//dodatkowe dane
			QValueList<QVariant> additionalParams;
			QValueListIterator<RegisteredControl> parentControl;
			QWidget *widget;
			
			//w przypadku kontrolek sk�adaj�cych si� z kilku mniejszych,
			// wska�nik na rodzica, w przypadku pojedynczej kontrolki == widget
			QWidget *entireWidget; 

			int nrOfControls;
			UserLevel userLevelRequired;

			ConfigFile* config;
			QValueList <ElementConnections> ConnectedSlots;
		};

		static QMap<QString, QValueList <ElementConnections> > SlotsOnCreateTab;
		static QMap<QString, QValueList <ElementConnections> >SlotsOnCloseTab;
		static QMap<QString, QValueList <ElementConnections> >SlotsOnApplyTab;
		static QApplication* appHandle;

		static QMap<QString, QValueList <RegisteredControl> > Tabs;
		static QStringList TabNames;
		static QMap<QString, int> TabSizes;
		static QDict<QSignal> tabChangesIn, tabChangesOut;//jakie� lepsze nazwy by si� przyda�y ;)


		QVGroupBox* box;
		/**
		    Dodaje kontrolk� do listy
		**/
		static bool addControl(const QString& groupname, ConfigDialog::RegisteredControl& control);
		
		void createWidget(QValueListIterator <RegisteredControl> control);
		void createTabAndWidgets(const QString& tab);

	public:
		/**
		    Konstruktor
		**/

		ConfigDialog(QApplication *application, QWidget *parent = 0, const char *name = 0);
		~ConfigDialog();
		/**
		    Pokazuje okno konfiguracji
		**/
		static void showConfigDialog(QApplication* application);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Warto�� kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------

			Domy�ln� warto�ci� kontrolki przy pierwszym uruchomieniu programu
			jest "defaultS".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
 		static void addCheckBox(
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const bool defaultS=false, const QString &tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);
		static void addCheckBox(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const bool defaultS=false, const QString &tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Kolor przycisku jest ustawiony na "color"
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addColorButton(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QColor& color, const QString& tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);
		static void addColorButton(ConfigFile *config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QColor& color, const QString& tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addComboBox(
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& tip="", const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".

			Warto�� kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Lista opcji do wyboru to "options", kt�rym w pliku konfiguracyjnym odpowiadaj� "values".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addComboBox(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS="", const QString& tip="", const QString& name="",
				UserLevel userLevelRequired = Beginner);
		static void addComboBox(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS="", const QString& tip="", const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Ilo�� kolumn w kontrolce ustwiona jest na nrColumns
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addGrid(
				const QString& groupname, const QString& parent, const QString& caption,
				const int nrColumns=3, const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addHBox(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addHGroupBox(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& name="",
				UserLevel userLevelRequired = Beginner);


		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Warto�� kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------

			Domy�ln� warto�ci� kontrolki przy pierwszym uruchomieniu programu
			jest "defaultS".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addHotKeyEdit(
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);
		static void addHotKeyEdit(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".

			Warto�� kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Lista opcji do wyboru to "options", kt�rym w pliku konfiguracyjnym odpowiadaj� "values".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addHRadioGroup(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS="", const QString& tip="", const QString& name="",
				UserLevel userLevelRequired = Beginner);
		static void addHRadioGroup(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS="", const QString& tip="", const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addLabel(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Warto�� kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------

			Domy�ln� warto�ci� kontrolki przy pierwszym uruchomieniu programu
			jest "defaultS".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addLineEdit(
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);
		static void addLineEdit(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);


		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Domy�ln� warto�ci� kontrolki ustawion jest na "defaultS".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addLineEdit2(
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& defaultS="", const QString& tip="", const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Warto�� kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------

			Domy�ln� warto�ci� kontrolki przy pierwszym uruchomieniu programu
			jest "defaultS".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addTextEdit(
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);
		static void addTextEdit(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const QString& defaultS="", const QString& tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);


		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addListBox(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& tip= "", const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addListView(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& tip= "", const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
			@param iconname nazwa ikony z zestawu lub �cie�ka do pliku
		**/

		static void addPushButton(
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& iconFileName="", const QString& tip="", const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".

			Warto�� kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Lista opcji do wyboru to "options", kt�rym w pliku konfiguracyjnym odpowiadaj� "values".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addRadioGroup(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				int strips, Orientation orientation,
				const QString &defaultS="", const QString& tip="", const QString& name="",
				UserLevel userLevelRequired = Beginner);
		static void addRadioGroup(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				int strips, Orientation orientation,
				const QString &defaultS="", const QString& tip="", const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia tekst kontrolki na "caption".
			Warto�� kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------

			Domy�ln� warto�ci� kontrolki przy pierwszym uruchomieniu programu
			jest "defaultS".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addSelectFont(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& entry, const QString& defaultS="", const QString &tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);
		static void addSelectFont(ConfigFile *config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& entry, const QString& defaultS="", const QString &tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addSelectPaths(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Warto�� kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Minimalna warto�� kontrolki ustawiona jest na "minValue", maksymalna
			na "maxValue", krok jest ustawiony na "pageStep"
			Domy�ln� warto�ci� kontrolki przy pierwszym uruchomieniu programu
			jest "value"
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addSlider(
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const int minValue=0, const int maxValue=100,
			    const int pageStep=1, const int value=50, const QString& tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);
		static void addSlider(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const int minValue=0, const int maxValue=100,
			    const int pageStep=1, const int value=50, const QString& tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);


		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Warto�� kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Minimalna warto�� kontrolki ustawiona jest na "minValue", maksymalna
			na "maxValue", krok jest ustawiony na "step"
			Domy�ln� warto�ci� kontrolki przy pierwszym uruchomieniu programu
			jest "value"
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addSpinBox(
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const int minValue=0, const int maxValue=100,
				const int step=1, const int value=50, const QString& tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);
		static void addSpinBox(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
			    const QString& entry, const int minValue=0, const int maxValue=100,
				const int step=1, const int value=50, const QString& tip="",
				const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje zak�adk� o nazwie "caption"
		    @param iconname nazwa ikony z zestawu lub �cie�ka do pliku
		**/
		static void addTab(const QString& caption, const QString& iconFileName="",
			UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addVBox(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".
			Nazwa kontrolki ustawiona jest na "name".
		**/

		static void addVGroupBox(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Dodaje kontrolk� do zak�adki "groupname",
			Rodzicem kontrolki jest kontrolka "parent".
			Ustawia text kontrolki na "caption".

			Warto�� kontrolki jest zapisana do pliku konfiguracyjnego "config" w postaci
			-------------
			[groupname]
			entry= value {defaultS}
			-------------
			Lista opcji do wyboru to "options", kt�rym w pliku konfiguracyjnym odpowiadaj� "values".
			Podpowied� kontrolki ustawiona jest na "tip".
			Nazwa kontrolki ustawiona jest na "name".
		**/
		static void addVRadioGroup(
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS="", const QString& tip="", const QString& name="",
				UserLevel userLevelRequired = Beginner);
		static void addVRadioGroup(ConfigFile* config,
				const QString& groupname, const QString& parent, const QString& caption,
				const QString &entry, const QStringList &options, const QStringList &values,
				const QString &defaultS="", const QString& tip="", const QString& name="",
				UserLevel userLevelRequired = Beginner);

		/**
		    Pobiera wska�nik do kontrolki ButtonGroup(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QButtonGroup* getButtonGroup(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki CheckBox(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QCheckBox*   getCheckBox(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki ColorButton(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static ColorButton* getColorButton(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki ComboBox(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QComboBox*   getComboBox(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki Grid(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QGrid*       getGrid(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki HBox(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QHBox* 	    getHBox(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki HGroupBox(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QHGroupBox*  getHGroupBox(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki HButtonGroup(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QHButtonGroup* getHButtonGroup(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki HotKeyEdit(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static HotKey* 	    getHotKeyEdit(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki LineEdit(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QLineEdit*   getLineEdit(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki TextEdit(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QTextEdit*   getTextEdit(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki Label(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QLabel*      getLabel(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki ListBox(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QListBox*    getListBox(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki ListView(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QListView*   getListView(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki PushButton(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QPushButton* getPushButton(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki SelectFont(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static SelectFont* getSelectFont(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki SelectPaths(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static SelectPaths* getSelectPaths(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki Slider(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QSlider*     getSlider(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki SpinBox(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QSpinBox*    getSpinBox(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki VBox(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QVBox*  	    getVBox(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki VGroupBox(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QVGroupBox*  getVGroupBox(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki VButtonGroup(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QVButtonGroup* getVButtonGroup(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    W przypadku kontrolki zawieraj�cej kilka mniejszych (z czego jedn� z nich zwraca jedna z funkcji getXXX)
			zwraca wska�nik do niej, w przeciwnym wypadku zwraca t� (pojedyncz�) kontrolk�
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
			UWAGA: nie wolno rzutowa� zwracanego wska�nika na �adn� klas� dziedzicz�c� pod QWidget!
			(np. addSpinBox dodaje QHBox zawieraj�c� QLabel i QSpinBox, getSpinBox() zwr�ci SpinBoksa, getEntireWidget QHBoksa())
		**/
		static QWidget* getEntireWidget(const QString& groupname, const QString& caption, const QString& name="");

		/**
		    Pobiera wska�nik do kontrolki Widget(groupname, caption, name)
			je�eli okno konfiguracji jest zamkni�te zwraca NULL
		**/
		static QWidget* widget(const QString& groupname, const QString& caption, const QString& name = "");

		/**
			zwraca informacj� czy kontrolka (groupname, caption, name) jest w konfiguracji
			przy okazji przekazuj�c jej iterator je�eli control != 0
		**/
		static bool controlExists(const QString& groupname, const QString& caption, const QString& name = "", QValueListConstIterator<RegisteredControl> *control = 0);

		/**
			zwraca informacj� czy kontrolka (groupname, caption, name) jest w konfiguracji
			przy okazji przekazuj�c jej iterator je�eli control != 0
		**/
		static bool controlExists(const QString& groupname, const QString& caption, const QString& name, QValueListIterator<RegisteredControl> *control);

		/**
		    Wyszukuje pozycj� zak�adki "name",
			zwraca przez parametry jej iterator oraz informacj� o znalezieniu je�eli found != 0
		**/
		static void tab(const QString &name, QValueListConstIterator<RegisteredControl> &ret, bool *found = 0);

		/**
		    Wyszukuje pozycj� zak�adki "name",
			zwraca przez parametry jej iterator oraz informacj� o znalezieniu je�eli found != 0
		**/
		static void tab(const QString &name, QValueListIterator<RegisteredControl> &ret, bool *found = 0);

		/**
			Pod��cza slot "slot" obiektu "receiver"
			pod sygna� "signal" kontrolki (groupname, caption, name)
		**/
		static void connectSlot(const QString& groupname, const QString& caption, const char* signal,
			    const QObject* receiver, const char* slot,const QString& name="");

		/**
			Od��cza slot "slot" obiektu "receiver"
			od sygna�u "signal" kontrolki (groupname, caption, name)
		**/
		static void disconnectSlot(const QString& groupname, const QString& caption, const char* signal,
			    const QObject* receiver, const char* slot,const QString& name="");

		/**
			Rejestruje slot "slot" obiektu "receiver",
			kt�ry jest wywo�ywany przytworzeniu zakladki
		**/
		static void registerSlotOnCreateTab(const QString& tab, const QObject* receiver, const char* slot);


		/**
			Wyrejestrowuje slot "slot" obiektu "receiver",
			kt�ry jest wywo�ywany przytworzeniu zakladki
		**/

		static void unregisterSlotOnCreateTab(const QString& tab, const QObject* receiver, const char* slot);


		/**
			Rejestruje slot "slot" obiektu "receiver",
			kt�ry jest wywo�ywany przy zamykaniu okna konfiguracji
		**/
		static void registerSlotOnCloseTab(const QString& tab, const QObject* receiver, const char* slot);

		/**
			Wyrejestrowuje slot "slot" obiektu "receiver",
			kt�ry jest wywo�ywany przy zamykaniu okna konfiguracji
		**/
		static void unregisterSlotOnCloseTab(const QString& tab, const QObject* receiver, const char* slot);

		/**
			Rejestruje slot "slot" obiektu "receiver",
			kt�ry jest wywo�ywany przy zapisywaniu konfiguracji
		**/
		static void registerSlotOnApplyTab(const QString& tab, const QObject* receiver, const char* slot);

		/**
			Wyrejestrowuje slot "slot" obiektu "receiver",
			kt�ry jest wywo�ywany przy zapisywaniu konfiguracji
		**/
		static void unregisterSlotOnApplyTab(const QString& tab, const QObject* receiver, const char* slot);

		/**
			Rejestruje 2 sloty obiektu "receiver", kt�re s�
			wywo�ywane przy zmianie zak�adki "name"
			slotIn - przy zmianie aktywnej zak�adki na "name" (mo�e by� == 0)
			slotOut - przy zmianie aktywnej zak�adki z "name" na inn� (mo�e by� == 0)
		**/
		static void registerSlotsOnTabChange(const QString &name, const QObject *receiver, const char *slotIn, const char *slotOut);

		/**
			Wyrejestrowuje 2 sloty obiektu "receiver", kt�re s�
			wywo�ywane przy zmianie zak�adki "name"
			slotIn - przy zmianie aktywnej zak�adki na "name" (mo�e by� == 0)
			slotOut - przy zmianie aktywnej zak�adki z "name" na inn� (mo�e by� == 0)
		**/
		static void unregisterSlotsOnTabChange(const QString &name, const QObject *receiver, const char *slotIn, const char *slotOut);

		/**
		    Usuwa kontrolk� z zak�adki "groupname", o etykiecie "caption" i nazwie "name".
		**/
		static void removeControl(const QString& groupname, const QString& caption, const QString& name = "");

		/**
		    Usuwa zak�adk� o nazwie caption
		**/
		static bool removeTab(const QString& caption);

		/**
			Zwraca prawd� je�eli zak�adka znajduje si� w konfiguracji
		**/
		static bool tabExists(const QString& caption);

		static bool dialogOpened();
		static void closeDialog();

		//u�ywa� tylko w wyj�tkowych sytuacjach
		static ConfigDialog *configdialog;

	protected:
		UserLevel userLevel;
		QListBox* listBox;
		QScrollView* view;
		QPushButton* okButton;
		QPushButton* applyButton;
		QPushButton* cancelButton;
		static QString currentTab;
		virtual void keyPressEvent(QKeyEvent *e);
		void updateUserLevel(QMapConstIterator<QString,
			QValueList<ConfigDialog::RegisteredControl> > tab);



	signals:

		void createTab();
		/**
		    Sygna� jest emitowany przy otwieraniu okna
		**/
		void create();

		/**
		    Sygna� jest emitowany przy zamykaniu okna
		**/
		void destroy();

		/**
		   Sygna� jest emitowany przy zapisywaniu konfiguracji
		**/
		void apply();

	protected slots:
		/**
		    Zapisuje warto�ci kontrolek do pliku
		**/

		void updateConfig();

		/**
		    Zapisuje warto�ci kontrolek do pliku i zamyka okno
		**/

		void updateAndCloseConfig();

		/**
		    Zmienia zak�adk� na "name"
		**/
		void changeTab(const QString& name);

		/**
		    Ukrywa kontrolki o poziomie mniejszym od newUserLevel
		**/
		void changeUserLevel(int newUserLevel);
};
#endif
