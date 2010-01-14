/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@go2.pl)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2008, 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
 * %kadu copyright end%
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

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

		void saveTabs();

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
		* Metoda jest wywo�1�71�1�770�1�71�1�770ywana po zmianie w oknie konfiguracyjnym.
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
		* Metoda jest wywo�1�71�1�770�1�71�1�770ywana w momencie otwarcia okna konfiguracji
		*/
		virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow);

		/**
		* Od�1�71�1�770�1�71�1�770�1�71�1�770�1�71�1�772cza rozmow�1�71�1�770�1�71�1�771 od kart.
		* @param chat wska�1�71�1�770�1�71�1�772nik do rozmowy
		* @return prawda je�1�71�1�770�1�71�1�772li rozmowa by�1�71�1�770�1�71�1�770a w kartach
		*/
		bool detachChat(ChatWidget *chat);

		/**
		* Metoda zapisujaca konfiguracje.
		*/
		virtual void store();

		virtual StorableObject * storageParent() { return 0; }
		virtual QString storageNodeName() { return QLatin1String("ModuleTabs"); }

	public slots:
		/**
		* Slot zostaje wywo�1�71�1�770�1�71�1�770any w momencie otwarcia okna rozmowy.
		* @param chat okno
		*/
		void onNewChat(ChatWidget *chat, bool &handled);

		/**
		* Slot zostaje wywo�1�71�1�770�1�71�1�770any w momencie zamkni�1�71�1�770�1�71�1�771cia rozmowy.
		* @param chat okno
		*/
		void onDestroyingChat(ChatWidget *chat);

		/**
		* Slot zostaje wywo�1�71�1�770�1�71�1�770any za ka�1�71�1�770�1�71�1�774dym razem gdy nast�1�71�1�770�1�71�1�772pi pr�1�71�1�77�1�71�1�77ba otwarcia okna rozmowy.
		* @param chat okno
		*/
		void onOpenChat(ChatWidget *chat);

		/**
		* Slot zostaje wywo�1�71�1�770�1�71�1�770any w momencie zmiany statusu.
		* @param account konto, na kt�1�71�1�77�1�71�1�77rym zmieni�1�71�1�770�1�71�1�770 si�1�71�1�770�1�71�1�771 status
		* @param contact kontakt, dla kt�1�71�1�77�1�71�1�77rego zmieni�1�71�1�770�1�71�1�770 si�1�71�1�770�1�71�1�771 status
		* @param oldStatus poprzedni status
		*/
		void onTitleChanged(Chat chatChanged, const QString &newTitle);

		/**
		* Slot zostaje wywo�1�71�1�770�1�71�1�770any w momencie zmiany aktywnej karty
		* @param index numer aktywowanej karty
		*/
		void onTabChange(int index);

		void onNewTab(QAction *sender, bool toggled);

		/**
		* Slot zostaje wywo�1�71�1�770�1�71�1�770any w momencie "upuszczenia" grupy kontakt�1�71�1�77�1�71�1�77w na pasek kart
		* @param altnicks lista u�1�71�1�770�1�71�1�774ytkownik�1�71�1�77�1�71�1�77w rozdzielonych "/n"
		* @param index pozycja na kt�1�71�1�77�1�71�1�77rej ma zosta�1�71�1�770�1�71�1�774 umieszczona nowa karta
		*/
		void openTabWith(QStringList altnicks, int index);

		/**
		* Slot zostaje wywo�1�71�1�770�1�71�1�770any w momencie zmiany stanu przycisku do�1�71�1�770�1�71�1�770�1�71�1�770�1�71�1�772czenia/od�1�71�1�770�1�71�1�770�1�71�1�770�1�71�1�772czania okna do/od kart.
		* @param users lista u�1�71�1�770�1�71�1�774ytkownik�1�71�1�77�1�71�1�77w rozmowy
		* @param source wska�1�71�1�770�1�71�1�772nik do widgeta
		* @param isOn stan przycisku
		*/
		void onTabAttach(QAction *sender, bool toggled);

		/**
		* Slot zostaje wywo�1�71�1�770�1�71�1�770any w momencie dodania przycisku do toolbara.
		* @param users lista u�1�71�1�770�1�71�1�774ytkownik�1�71�1�77�1�71�1�77w rozmowy
		*/
		void attachToTabsActionCreated(Action *action);

		/**
		* Slot zostaje wywo�1�71�1�770�1�71�1�770any w momencie zamykania chata skr�1�71�1�77�1�71�1�77tem klawiszowym
		*/
		void closeChat();

	signals:
		void chatWidgetActivated(ChatWidget *);
};

extern TabsManager *tabs_manager;
#endif
