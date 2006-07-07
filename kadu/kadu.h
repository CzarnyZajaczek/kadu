#ifndef KADU_H
#define KADU_H

#include <qmainwindow.h>
#include <qstring.h>
#include <qtimer.h>
#include <qvaluelist.h>

#include "dockarea.h"
#include "gadu.h"
#include "status.h"
#include "usergroup.h"
#include "userlistelement.h"

class QFile;
class QListBoxItem;
class QMenuBar;
class KaduTabBar;
class UserBox;
class KaduTextBrowser;
class QPushButton;
class KaduSlots;

/**
	G��wne okno Kadu
**/
class Kadu : public QWidget
{
	Q_OBJECT

	private:
		static bool Closing;
		friend class KaduSlots;
		KaduSlots *kaduslots;
		DockArea* TopDockArea;
		KaduTextBrowser* InfoPanel;
		QMenuBar* MenuBar;
		QPopupMenu* MainMenu;
		KaduTabBar* GroupBar;
		UserBox* Userbox;
		QVBoxLayout *MainLayout;

		QPopupMenu* statusMenu;
		QPushButton* statusButton;
		QPoint lastPositionBeforeStatusMenuHide;
		QDateTime StartTime;
		QTimer updateInformationPanelTimer;

		UserStatus status;

		bool ShowMainWindowOnStart;
		bool DoBlink;
		bool BlinkOn;
		bool Docked;
		bool dontHideOnClose;

		void createMenu();
		void createStatusPopupMenu();

		void showStatusOnMenu(int);

	public slots:
		virtual void show();
		virtual void hide();
		void mouseButtonClicked(int, QListBoxItem *);
		void updateInformationPanel(UserListElement);
		void updateInformationPanel();
		void updateInformationPanelLater();

		void sendMessage(UserListElement elem);
		void configure();

	private slots:
		void openChat();
		void chatMsgReceived(Protocol *protocol, UserListElements senders, const QString &msg, time_t time);

		void wentOnline(const QString &);
		void wentBusy(const QString &);
		void wentInvisible(const QString &);
		void wentOffline(const QString &);
		void connected();
		void connecting();
		void disconnected();
		void imageReceivedAndSaved(UinType sender, uint32_t size, uint32_t crc32, const QString &path);
		void systemMessageReceived(const QString &msg);
		void deleteOldConfigFiles();

		void currentChanged(UserListElement);

		void statusMenuAboutToHide(void);
		void dockMenuAboutToHide(void);
	protected:
		void keyPressEvent(QKeyEvent *e);
		virtual void resizeEvent(QResizeEvent *);
//		virtual void moveEvent(QMoveEvent *);

	public:
		Kadu(QWidget* parent=0, const char *name=0);
		~Kadu();
		bool userInActiveGroup(UinType uin);
		void removeUsers(UserListElements);

		/**
			Zwraca wskaznik do belki menu glownego okna.
		**/
		QMenuBar* menuBar() const;

		/**
			Zwraca wskaznik do glownego menu programu.
		**/
		QPopupMenu* mainMenu() const;

		/**
			Zwraca wskaznik do zakladek z nazwami grup.
		**/
		KaduTabBar* groupBar() const;

		/**
			Zwraca wskaznik do userbox-a w glownym oknie.
		**/
		UserBox* userbox() const;

		/**
			Zwraca wska�nik na g��wny layout w obiekcie, teraz jest to QVBoxLayout.
			(hint) U�ywaj�c nale�y bada� nazwy i klasy obiekt�w GUI dost�pnych przez ten obiekt.
		**/
		QVBoxLayout* mainLayout() const;
		
		/**
			Zwraca true je�li kadu jest zadokowane.
		**/
		bool docked() const;

		/**
			Set default status based on values in configuration file
		**/
		void setDefaultStatus();

		/**
		**/
		void startupProcedure();

		/**
		**/
		int personalInfoMenuId;//potrzebne dla modu�u account_management

		/**
			w zwi�zku z tym, �e opis sk�adni dla parsera jest u�ywany w kilku miejscach
			dodane zosta�o to pole, �eby nie trzeba by�o zmienia� tekstu w kilku miejscach
		**/
		static const char *SyntaxText;

		static bool closing() { return Closing; }
		static void setClosing() { Closing = true; }

		const QDateTime &startTime() const;
	public slots:
		void slotHandleState(int command);
		void changeAppearance();
		void blink();
		void showdesc(bool show = true);
		virtual bool close(bool quit = false);
		void quitApplication();

		/**
			Potrzebne dla modu�u dokuj�cego �eby
			g��wne okno nie miga�o przy starcie...
		**/
		void setShowMainWindowOnStart(bool show);

		/**
			Modu� dokowania powinien to ustawic, aby kadu
			wiedzialo, ze jest zadokowane.
		**/
		void setDocked(bool docked, bool dontHideOnClose);

		void about();
		void addUserAction();
		void blockUser();
		void copyDescription();
		void copyPersonalInfo();
		void deleteHistory();
		void deleteUsers();
		void help();
		void hideKadu();
		void ignoreUser();
		void importExportUserlist();
		void lookupInDirectory();
		void manageIgnored();
		void notifyUser();
		void offlineToUser();
		void hideDescription();
		void personalInfo();
		void quit();
		void searchInDirectory();
		void showUserInfo();
		void viewHistory();
		void popupMenu();

		// odczytuje z obrazka tekst i zapisuje go w drugim parametrze
		void readTokenValue(QPixmap, QString &);

		void setMainWindowIcon(const QPixmap &);

		void selectedUsersNeeded(const UserGroup*& users);
		void inactiveUsersActionActivated();
		void descriptionUsersActionActivated();
		void configurationActionActivated();
		void editUserActionActivated(const UserGroup* users);
		void addUserActionActivated(const UserGroup* users);

	signals:
		void keyPressed(QKeyEvent* e);
		void statusPixmapChanged(const QPixmap &icon, const QString &icon_name);

		/**
			U�ytkownik chce zmieni� status. Je�li jest to status
			bezopisowy, to zmienna sigDesc b�dzie r�wna QString::null.
			Je�li zostanie ustawiona zmienna stop, to status nie
			b�dzie zmieniony.
		**/
		void changingStatus(UserStatus &, bool &stop);

		/**
			wywo�ana zosta�a funkcja show() na g��wnym oknie
		**/
		void shown();

		void hiding();

		/**
			u�ywany przez modu� hints do zdobycia pozycji traya od modu�u docking
			TODO: trzeba wymy�li� jaki� elegancki spos�b na komunikacj� pomi�dzy modu�ami, kt�re nie zale�� od siebie
		**/
		void searchingForTrayPosition(QPoint &point);

		void settingMainIconBlocked(bool &);
};

class KaduSlots : public QObject
{
	Q_OBJECT

	public:
		KaduSlots(QObject *parent=0, const char *name=0);
	public slots:
		void onCreateTabGeneral();
		void onCreateTabLook();
		void onApplyTabLook();
		void onApplyTabGeneral();
		void chooseColor(const char *name, const QColor& color);
		void chooseFont(const char *name, const QFont& font);
		void updatePreview();
		void updateStatus(bool);
};

extern Kadu* kadu;
extern QPopupMenu* dockMenu;
extern int lockFileHandle;
extern QFile *lockFile;
struct flock;
extern struct flock *lock_str;

#endif
