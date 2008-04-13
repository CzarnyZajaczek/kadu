#ifndef KADU_H
#define KADU_H

#include <QCustomEvent>
#include <QDomElement>
#include <QKeyEvent>
#include <QMenu>
#include <QPixmap>
#include <QResizeEvent>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>

#include "action.h"
#include "configuration_aware_object.h"
#include "gadu.h"
#include "kadu_main_window.h"
#include "status.h"
#include "usergroup.h"
#include "userlistelement.h"

class ActionDescription;
class KaduTabBar;
class KaduTextBrowser;
class MainConfigurationWindow;
class SplitStatusChanger;
class UserBox;
class UserStatusChanger;

class QFile;
class Q3ListBoxItem;
class QMenuBar;
class QPushButton;

/**
	G��wne okno Kadu
**/
class Kadu : public KaduMainWindow, ConfigurationAwareObject, public ActionWindow
{
	Q_OBJECT

	ActionDescription *inactiveUsersAction;
	ActionDescription *descriptionUsersAction;
	ActionDescription *onlineAndDescriptionUsersAction;
	ActionDescription *configurationActionDescription;
	ActionDescription *editUserActionDescription;
	ActionDescription *addUserActionDescription;
	ActionDescription *openSearchActionDescription;

	QActionGroup *changeStatusActionGroup;
	QAction *changeStatusToOnline;
	QAction *changeStatusToOnlineDesc;
	QAction *changeStatusToBusy;
	QAction *changeStatusToBusyDesc;
	QAction *changeStatusToInvisible;
	QAction *changeStatusToInvisibleDesc;
	QAction *changeStatusToOffline;
	QAction *changeStatusToOfflineDesc;
	QAction *changePrivateStatus;

	// TODO: remove
	friend class Wizard;

	private:
		UserListElement Myself;

		static bool Closing;
		KaduTextBrowser* InfoPanel;
		QString InfoPanelSyntax;
		QMenuBar* MenuBar;
		QMenu* MainMenu;
		QMenu* RecentChatsMenu;
		KaduTabBar* GroupBar;
		UserBox* Userbox;
		QWidget* MainWidget;
		QVBoxLayout *MainLayout;

		QMenu* statusMenu;
		QPushButton* statusButton;
		QPoint lastPositionBeforeStatusMenuHide;
		QDateTime StartTime;
		QTimer updateInformationPanelTimer;

		UserStatus NextStatus;
		UserGroup *selectedUsers; //don't touch!

		UserStatusChanger *userStatusChanger;
#if 0
		SplitStatusChanger *splitStatusChanger;
#endif

		bool ShowMainWindowOnStart;
		bool DoBlink;
		bool BlinkOn;
		bool Docked;
		bool dontHideOnClose;

		void createMenu();
		void createStatusPopupMenu();

		void showStatusOnMenu(int);

		void import_0_5_0_configuration();
		void createDefaultConfiguration();

	public slots:
		virtual void show();
		virtual void hide();
		void mouseButtonClicked(int, Q3ListBoxItem *);
		void updateInformationPanel(UserListElement);
		void updateInformationPanel();
		void updateInformationPanelLater();

		void sendMessage(UserListElement elem);
		void configure();

		void setStatus(const UserStatus &status);
		void setOnline(const QString &description = QString::null);
		void setBusy(const QString &description = QString::null);
		void setInvisible(const QString &description = QString::null);
		void setOffline(const QString &description = QString::null);

	private slots:
		void inactiveUsersActionActivated(QAction *sender, bool toggled);
		void descriptionUsersActionActivated(QAction *sender, bool toggled);
		void onlineAndDescUsersActionActivated(QAction *sender, bool toggled);
		void configurationActionActivated(QAction *sender, bool toggled);
		void addUserActionActivated(QAction *sender, bool toggled);
		void editUserActionActivated(QAction *sender, bool toggled);
		void searchInDirectoryActionActivated(QAction *sender, bool toggled);

		void openChat();
		void messageReceived(Protocol *protocol, UserListElements senders, const QString &msg, time_t time);
		void createRecentChatsMenu();
		void openRecentChats(int index);
		void openChatWith();

		void goOnline();
		void goOnlineDesc();
		void goBusy();
		void goBusyDesc();
		void goInvisible();
		void goInvisibleDesc();
		void goOffline();
		void goOfflineDesc();

		void changePrivateStatusSlot(bool toggled);

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

		void changeStatus(UserStatus status);

	protected:
		void keyPressEvent(QKeyEvent *e);
		virtual void resizeEvent(QResizeEvent *);
		virtual void customEvent(QCustomEvent *);

		virtual void configurationUpdated();
//		virtual void moveEvent(QMoveEvent *);

	public:
		Kadu(QWidget* parent=0);
		~Kadu();

		virtual bool supportsActionType(ActionDescription::ActionType type) {
			return type & (ActionDescription::TypeGlobal | ActionDescription::TypeUserList); }
		virtual UserBox * getUserBox() { return Userbox; }
		virtual UserListElements getUserListElements();
		virtual ChatWidget * getChatWidget() { return 0; }

		const UserListElement & myself() { return Myself; };

		bool userInActiveGroup(UinType uin);
		void removeUsers(UserListElements);

		/**
			Zwraca wskaznik do glownego menu programu.
		**/
		QMenu* mainMenu() const;

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
		static const char *SyntaxTextExtended;
		static const char *SyntaxTextNotify;

		static bool closing() { return Closing; }
		static void setClosing() { Closing = true; }

		const QDateTime &startTime() const;
		void refreshPrivateStatusFromConfigFile();

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
		void openDescriptionLink();
		void writeMail();
		void copyPersonalInfo();
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
		void showUserInfo();
		void popupMenu();

		// odczytuje z obrazka tekst i zapisuje go w drugim parametrze
		void readTokenValue(QPixmap, QString &);

		void setMainWindowIcon(const QPixmap &);

		void selectedUsersNeeded(const UserGroup*& users);
		void editUserActionAddedToToolbar(const UserGroup* users);
		void editUserActionSetParams(QString protocolName, UserListElement users);
//		void showStatusActionActivated();
//		void showStatusActionAddedToToolbar(ToolButton* button);

	signals:
		void keyPressed(QKeyEvent* e);
		void statusPixmapChanged(const QIcon &icon, const QString &icon_name);

		// TODO: workaround
		void messageReceivedSignal(Protocol *, UserListElements, const QString &, time_t);

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

		void removingUsers(UserListElements users);
};

class OpenGGChatEvent : public QCustomEvent
{
	public:
	OpenGGChatEvent(int num) : QCustomEvent(5432), Number(num) {}
	int number() const { return Number; }
	private:
	int Number;
};

extern Kadu* kadu;
extern QMenu* dockMenu;
extern int lockFileHandle;
extern QFile *lockFile;
struct flock;
extern struct flock *lock_str;

#endif
