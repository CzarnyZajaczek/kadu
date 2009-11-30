/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TABS_TABS_H
#define TABS_TABS_H

/*
 * autor
 * Michal Podsiadlik
 * michal at gov.one.pl
 */

#include <QtCore/QObject>
#include <QtCore/QTimer>

#include "chat/chat-manager.h"

#include "configuration/configuration-aware-object.h"
#include "gui/windows/main-configuration-window.h"
#include "misc/misc.h"
#include "storage/storable-object.h"

#include "tabwidget.h"

class QMenu;
class Action;
class ActionDescription;

class TabsManager : public ConfigurationUiHandler, ConfigurationAwareObject, StorableObject
{
	Q_OBJECT

		void createDefaultConfiguration();

		ActionDescription *openInNewTabActionDescription;
		ActionDescription *attachToTabsActionDescription;
		TabWidget *tabdialog;
		QTimer timer;
		QList<ChatWidget *> chatsWithNewMessages, newchats, detachedchats;
		bool no_tabs, autoswith, force_tabs;

		/**
		* pozycja na kt�1�7�1�7rej ma zosta�1�70�1�74 dodana nowa karta
		*/
		int target_tabs;
		void insertTab(ChatWidget *chat);
		void makePopupMenu();
		int menuitem;
		ChatWidget *selectedchat;
		QMenu *menu;
		Action *action;

		/**
		* Przywraca stan kart w momencie uruchomienia kadu.
		*/
		void loadTabs();

		/**
		* Zapisuje stan kart w momencie wyj�1�70�1�72cia z kadu.
		*/
		void saveTabs();

		/**
		* Rezerwuje lub usuwa miejsce na kartach przeznaczone
		* na przycisk zamkni�1�70�1�71cia chata na karcie
		* Od�1�70�1�72wie�1�70�1�74a r�1�7�1�7wnie�1�70�1�74 ikonki
		*/
		void repaintTabs();

		QString formatTabName(ChatWidget * chatWidget);

		void refreshTab(int tabIndex, ChatWidget * chatWidget);

		/**
		* Zmienne konfiguracyjne.
		*/
		bool config_conferencesInTabs;
		bool config_tabsBelowChats;
		bool config_autoTabChange;
		bool config_defaultTabs;
		unsigned config_minTabs;
		bool config_closeButtonOnTab;
		bool config_blinkChatTitle;
		bool config_showNewMessagesNum;

	private slots:
		void onTimer();
		void onContextMenu(QWidget *w, const QPoint &pos);
		void onMenuActionDetach();
		void onMenuActionDetachAll();
		void onMenuActionClose();
		void onMenuActionCloseAll();
		void onMessageReceived(Chat chat);

	protected:
		/**
		* Metoda jest wywo�1�70�1�70ywana po zmianie w oknie konfiguracyjnym.
		*/
		virtual void configurationUpdated();

		/**
		* Metoda odczytujaca konfiguracje.
		*/
		virtual void load();

	public:
		TabsManager(bool firstload);
		virtual ~TabsManager();

		/**
		* Metoda jest wywo�1�70�1�70ywana w momencie otwarcia okna konfiguracji
		*/
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

		/**
		* Od�1�70�1�70�1�70�1�72cza rozmow�1�70�1�71 od kart.
		* @param chat wska�1�70�1�72nik do rozmowy
		* @return prawda je�1�70�1�72li rozmowa by�1�70�1�70a w kartach
		*/
		bool detachChat(ChatWidget *chat);

		/**
		* Metoda zapisujaca konfiguracje.
		*/
		virtual void store();

	public slots:
		/**
		* Slot zostaje wywo�1�70�1�70any w momencie otwarcia okna rozmowy.
		* @param chat okno
		*/
		void onNewChat(ChatWidget *chat, bool &handled);

		/**
		* Slot zostaje wywo�1�70�1�70any w momencie zamkni�1�70�1�71cia rozmowy.
		* @param chat okno
		*/
		void onDestroyingChat(ChatWidget *chat);

		/**
		* Slot zostaje wywo�1�70�1�70any za ka�1�70�1�74dym razem gdy nast�1�70�1�72pi pr�1�7�1�7ba otwarcia okna rozmowy.
		* @param chat okno
		*/
		void onOpenChat(ChatWidget *chat);

		/**
		* Slot zostaje wywo�1�70�1�70any w momencie zmiany statusu.
		* @param account konto, na kt�1�7�1�7rym zmieni�1�70�1�70 si�1�70�1�71 status
		* @param contact kontakt, dla kt�1�7�1�7rego zmieni�1�70�1�70 si�1�70�1�71 status
		* @param oldStatus poprzedni status
		*/
		void onTitleChanged(Chat chatChanged, const QString &newTitle);

		/**
		* Slot zostaje wywo�1�70�1�70any w momencie zmiany aktywnej karty
		* @param index numer aktywowanej karty
		*/
		void onTabChange(int index);

		void onNewTab(QAction *sender, bool toggled);

		/**
		* Slot zostaje wywo�1�70�1�70any w momencie "upuszczenia" grupy kontakt�1�7�1�7w na pasek kart
		* @param altnicks lista u�1�70�1�74ytkownik�1�7�1�7w rozdzielonych "/n"
		* @param index pozycja na kt�1�7�1�7rej ma zosta�1�70�1�74 umieszczona nowa karta
		*/
		void openTabWith(QStringList altnicks, int index);

		/**
		* Slot zostaje wywo�1�70�1�70any w momencie zmiany stanu przycisku do�1�70�1�70�1�70�1�72czenia/od�1�70�1�70�1�70�1�72czania okna do/od kart.
		* @param users lista u�1�70�1�74ytkownik�1�7�1�7w rozmowy
		* @param source wska�1�70�1�72nik do widgeta
		* @param isOn stan przycisku
		*/
		void onTabAttach(QAction *sender, bool toggled);

		/**
		* Slot zostaje wywo�1�70�1�70any w momencie dodania przycisku do toolbara.
		* @param users lista u�1�70�1�74ytkownik�1�7�1�7w rozmowy
		*/
		void attachToTabsActionCreated(Action *action);

		/**
		* Slot zostaje wywo�1�70�1�70any w momencie zamykania chata skr�1�7�1�7tem klawiszowym
		*/
		void closeChat();

	signals:
		void chatWidgetActivated(ChatWidget *);
};

extern TabsManager *tabs_manager;
#endif
