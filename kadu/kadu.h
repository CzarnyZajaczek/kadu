#ifndef KADU_H
#define KADU_H

#include <qapplication.h>
#include <qprogressdialog.h>
#include <qmultilineedit.h>
#include <qsocketnotifier.h>
#include <qmainwindow.h>
#include <qpopupmenu.h>
#include <qnetworkprotocol.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qevent.h>
#include <qframe.h>
#include <qlayout.h>
#include <qtoolbutton.h>
#include <qtoolbar.h>
#include <qmenubar.h>
#include <qvaluelist.h>
#include <qfile.h>

#include "misc.h"
#include "userbox.h"
#include "userlist.h"
#include "tabbar.h"
#include "dcc.h"
#include "status.h"

/**
	Toolbar Kadu
**/
class ToolBar : public QToolBar
{
	private:
		struct ToolButton
		{
			QIconSet iconfile;
			QString caption, name;
			QObject* receiver;
			QString slot;
			QToolButton* button;
			int position;
		};
		static QValueList<ToolButton> RegisteredToolButtons;
		void createControls();
	public:
		static ToolBar* instance;
		ToolBar(QMainWindow* parent);
		~ToolBar();
		static void registerButton(const QIconSet& iconfile, const QString& caption, 
			QObject* receiver, const char* slot, const int position=-1, const char* name="");
		static void unregisterButton(const char* name);
		static void registerSeparator(int position=-1);
		static QToolButton* getButton(const char* name);
};

/**
	G��wne okno Kadu
**/
class Kadu : public QMainWindow
{
	Q_OBJECT

	private:
		QTextBrowser* descrtb;
		QMenuBar* MenuBar;
		QPopupMenu* MainMenu;
		KaduTabBar* GroupBar;
		UserBox* Userbox;

		int commencing_startup;
		bool updateChecked;
		bool Docked;

		void createMenu();
		void createToolBar();
		void createStatusPopupMenu();
		void setActiveGroup(const QString& group);

	private slots:
		void groupTabSelected(int id);
		void userListModified();
		void userListStatusModified(UserListElement *);
		void openChat();
		void userListUserAdded(const UserListElement& user);

	protected:
		bool event(QEvent *e);
		void watchDcc(void);
		void keyPressEvent(QKeyEvent *e);
//		virtual void moveEvent(QMoveEvent *);

		struct gg_event *dcc_e;
		int dcc_ret;
		bool blinkOn;
	 
	public:
		Kadu(QWidget* parent=0, const char *name=0);
		~Kadu();
		static void InitModules();
		void changeAppearance();
		bool userInActiveGroup(UinType uin);
		void removeUser(QStringList &, bool);
		void refreshGroupTabBar();
		/**
			Zwraca wskaznik do belki menu glownego okna.
		**/
		QMenuBar* menuBar();
		/**
			Zwraca wskaznik do glownego menu programu.
		**/
		QPopupMenu* mainMenu();
		/**
			Zwraca wskaznik do zakladek z nazwami grup.
		**/
		KaduTabBar* groupBar();
		/**
			Zwraca wskaznik do userbox-a w glownym oknie.
		**/
		UserBox* userbox();
		/**
			Modu� dokowania powinien to ustawic, aby kadu
			wiedzialo, ze jest zadokowane.
		**/
		void setDocked(bool docked);
		/**
			Zwraca true je�li kadu jest zadokowane.
		**/
		bool docked();

		// te zmienne s� tylko chwilowo publiczne.
		// trzeba to uporz�dkowa�
		bool autohammer;
		bool doBlink;
		
		//potrzebne dla modu�u dokuj�cego �eby g��wne okno nie miga�o przy starcie...
		bool showMainWindowOnStart;

	public slots:
		void blink();
		void dccFinished(dccSocketClass *dcc);
		void slotHandleState(int command);
		void setCurrentStatus(int status);
		void sendMessage(QListBoxItem *);
		void dataReceived();
		void dataSent();
		void dccReceived();
		void dccSent();
		void prepareDcc(void);
		void pingNetwork(void);
		void setStatus(int);
		void disconnectNetwork(void);
		void gotUpdatesInfo(const QByteArray &data, QNetworkOperation *op);
		void currentChanged(QListBoxItem *item);
		void showdesc(bool show = true);
		void statusMenuAboutToHide(void);
		void mouseButtonClicked(int, QListBoxItem *);
		void infopanelUpdate(UinType);
		virtual bool close(bool quit = false);
		void quitApplication();

		void about();
		void addUserAction();
		void blockUser();
		void configure();
		void changePassword();
		void deleteHistory();
		void deleteUsers();
		void exportUserlist();
		void help();
		void hideKadu();
		void ignoreUser();
		void importUserlist();
		void lookupInDirectory();
		void manageIgnored();
		void notifyUser();
		void offlineToUser();
		void personalInfo();
		void quit();
		void registerUser();
		void remindPassword();
		void searchInDirectory();
		void sendFile();
		void showUserInfo();
		void unregisterUser();
		void viewHistory();
		void popupMenu();
		void show();

	signals:
		void disconnectingNetwork();
		void disconnectedNetwork();
		void keyPressed(QKeyEvent* e);
		/**
			Podczas ��czenia (mruganie ikonki) czas na zmian�
			ikony na t� reprezentuj�c� docelowy status.
		**/
		void connectingBlinkShowStatus(int status);
		/**
			Podczas ��czenia (mruganie ikonki) czas na zmian�
			ikony na t� reprezentuj�c� status offline.
		**/		
		void connectingBlinkShowOffline();
		/**
			Zmieni� si� nasz aktualny status GG.
		**/
		void currentStatusChanged(int status);
		/**
			U�ytkownik chce zmieni� status. Je�li jest to status
			bezopisowy, to zmienna sigDesc b�dzie r�wna QString::null.
			Je�li zostanie ustawiona zmienna stop, to status nie
			b�dzie zmieniony.
		**/
		void changingStatus(int status, QString sigDesc, bool &stop);
		/**
			wywo�ana zosta�a funkcja show() na g��wnym oknie
		**/
		void showed();
};

class KaduSlots : public QObject
{
	Q_OBJECT

	public slots:
		void onCreateConfigDialog();
		void onDestroyConfigDialog();
};

extern Kadu* kadu;

extern QPopupMenu* statusppm;
extern QPopupMenu* dockppm;

extern int lockFileHandle;
extern QFile *lockFile;
#endif
