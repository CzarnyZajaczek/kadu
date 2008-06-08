/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QLabel>

#include <sys/stat.h>
#include <sys/types.h>

#include "action.h"
#include "chat_manager.h"
#include "chat_message.h"
#include "config_file.h"
#include "debug.h"
#include "history.h"
#include "history_dialog.h"
#include "hot_key.h"
#include "kadu.h"
#include "misc.h"
#include "message_box.h"
#include "pending_msgs.h"
#include "toolbar.h"
#include "userbox.h"

#include "history_module.h"



extern "C" int history_init()
{
	kdebugf();

	history_module = new HistoryModule();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/history.ui"), history_module);

	kdebugf2();
	return 0;
}

extern "C" void history_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/history.ui"), history_module);
	delete history_module;
	history_module = 0;

	kdebugf2();
}

HistoryModule::HistoryModule()
{
	kdebugf();

	createDefaultConfiguration();

	QString path_ = ggPath(QString::null);
	path_.append("/history/");
	mkdir(path_.local8Bit().data(), 0700);

	history = new HistoryManager();

	connect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));
	connect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget*)));

	foreach(ChatWidget *it, chat_manager->chats())
		chatCreated(it);

	connect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString&, time_t)),
		history, SLOT(messageReceived(Protocol *, UserListElements, const QString&, time_t)));
	connect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		history, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
	connect(kadu, SIGNAL(removingUsers(UserListElements)), this, SLOT(removingUsers(UserListElements)));


	historyActionDescription = new ActionDescription(
		ActionDescription::TypeUser, "showHistoryAction",
		this, SLOT(historyActionActivated(QAction *, bool)),
		"History", tr("Show history")
	);

	ToolBar::addDefaultAction("Kadu toolbar", "showHistoryAction", 4);
	ToolBar::addDefaultAction("Chat toolbar 1", "showHistoryAction", 3);

	UserBox::userboxmenu->addItemAtPos(5, "History", tr("History"), this, SLOT(viewHistory()), HotKey::shortCutFromFile("ShortCuts", "kadu_viewhistory"));
	UserBox::management->addItemAtPos(7, "ClearHistory", tr("Clear history"),  this, SLOT(deleteHistory()));
	connect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(userboxMenuPopup()));

	kdebugf2();
}

HistoryModule::~HistoryModule()
{
	kdebugf();

	int history_item = UserBox::userboxmenu->getItem(tr("History"));
	int delete_history_item = UserBox::management->getItem(tr("Clear history"));
	UserBox::userboxmenu->removeItem(history_item);
	UserBox::management->removeItem(delete_history_item);
	disconnect(UserBox::userboxmenu, SIGNAL(popup()), this, SLOT(userboxMenuPopup()));

	disconnect(chat_manager, SIGNAL(chatWidgetCreated(ChatWidget *)), this, SLOT(chatCreated(ChatWidget *)));
	disconnect(chat_manager, SIGNAL(chatWidgetDestroying(ChatWidget *)), this, SLOT(chatDestroying(ChatWidget *)));

	foreach(ChatWidget *it, chat_manager->chats())
		chatDestroying(it);

	delete historyActionDescription;
	historyActionDescription = 0;

	disconnect(gadu, SIGNAL(messageReceived(Protocol *, UserListElements, const QString&, time_t)),
		history, SLOT(messageReceived(Protocol *, UserListElements, const QString&, time_t)));
	disconnect(gadu, SIGNAL(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)),
		history, SLOT(imageReceivedAndSaved(UinType, uint32_t, uint32_t, const QString &)));
	disconnect(kadu, SIGNAL(removingUsers(UserListElements)),
		this, SLOT(removingUsers(UserListElements)));

	delete history;
	history = 0;

	kdebugf2();
}

void HistoryModule::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
	dontCiteOldMessagesLabel = dynamic_cast<QLabel *>(mainConfigurationWindow->widgetById("history/dontCiteOldMessagesLabel"));
	connect(mainConfigurationWindow->widgetById("history/dontCiteOldMessages"), SIGNAL(valueChanged(int)),
		this, SLOT(updateQuoteTimeLabel(int)));
	connect(mainConfigurationWindow->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("history/statusChanges"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("history/citation"), SLOT(setEnabled(bool)));
	connect(mainConfigurationWindow->widgetById("history/save"), SIGNAL(toggled(bool)),
		mainConfigurationWindow->widgetById("history/shortCuts"), SLOT(setEnabled(bool)));
}

void HistoryModule::updateQuoteTimeLabel(int value)
{
	dontCiteOldMessagesLabel->setText(tr("%1 day(s) %2 hour(s)").arg(-value / 24).arg((-value) % 24));
}

void HistoryModule::historyActionActivated(QAction *sender, bool toggled)
{
	kdebugf();

	KaduMainWindow *window = dynamic_cast<KaduMainWindow *>(sender->parent());
	if (!window)
		return;

	UserListElements users = window->getUserListElements();

	if (users.count() == 0)
		return;

	UinsList uins;
	foreach(UserListElement user, users)
		uins.append(user.ID("Gadu").toUInt());
	//TODO: throw out UinsList as soon as possible!
	(new HistoryDialog(uins))->show();
	kdebugf2();
}

void HistoryModule::chatKeyPressed(QKeyEvent *e, ChatWidget *chatWidget, bool &handled)
{
	if (HotKey::shortCut(e, "ShortCuts", "kadu_viewhistory"))
	{
		const UserGroup *users = chatWidget->users();
		UinsList uins;
		foreach(UserListElement *user, *users)
			uins.append(user->ID("Gadu").toUInt());
		//TODO: throw out UinsList as soon as possible!
		(new HistoryDialog(uins))->show();

		handled = true;
	}
}

void HistoryModule::appendHistory(ChatWidget *chat)
{
	UserListElements senders = chat->users()->toUserListElements();

	QList<HistoryEntry> entries;
	QList<HistoryEntry> entriestmp;
	QDateTime date = QDateTime::currentDateTime();
	unsigned int from, end, count;

	UinsList uins;//TODO: throw out UinsList as soon as possible!
	foreach(UserListElement user, senders)
		uins.append(user.ID("Gadu").toUInt());

	count = history->getHistoryEntriesCount(uins);
	end = count - 1;
	from = count;
	unsigned int chatHistoryQuotation = config_file.readUnsignedNumEntry("History", "ChatHistoryCitation") + pending.pendingMsgsCount(senders);
	while (from >= 1 && entries.count() < chatHistoryQuotation)
	{
		if (end < chatHistoryQuotation)
			from = 0;
		else
			from = end - chatHistoryQuotation + 1;

		entriestmp = history->getHistoryEntries(uins, from, end - from + 1, HISTORYMANAGER_ENTRY_CHATSEND
			| HISTORYMANAGER_ENTRY_MSGSEND | HISTORYMANAGER_ENTRY_CHATRCV | HISTORYMANAGER_ENTRY_MSGRCV);
		kdebugmf(KDEBUG_INFO, "temp entries = %u\n", entriestmp.count());

		QList<HistoryEntry>::iterator it = entriestmp.begin();
		while (it != entriestmp.end())
		{
			if ((*it).type == HISTORYMANAGER_ENTRY_CHATRCV
				|| (*it).type == HISTORYMANAGER_ENTRY_MSGRCV)
			{
				kdebugmf(KDEBUG_INFO, "%s %s\n",
					date.toString("dd.MM.yyyy hh:mm:ss").local8Bit().data(),
					(*it).date.toString("dd.MM.yyyy hh:mm:ss").local8Bit().data());
				if (date <= (*it).date)
					it = entriestmp.remove(it);
				else
					++it;
			}
			else
				++it;
		}

		if (!entriestmp.isEmpty())
			entries = entriestmp + entries;
		kdebugmf(KDEBUG_INFO, "entries = %u\n", entries.count());
		end = from - 1;
	}

	unsigned int entryCount = entries.count();
	if (entryCount < chatHistoryQuotation)
		from = 0;
	else
		from = entryCount - chatHistoryQuotation;

	QList<ChatMessage *> messages;

	int quotTime = config_file.readNumEntry("History", "ChatHistoryQuotationTime");

	QList<HistoryEntry>::const_iterator entry = entries.begin() + from;
	QList<HistoryEntry>::const_iterator entriesEnd = entries.end();
	for (; entry != entriesEnd; ++entry)
		if ((*entry).date.secsTo(QDateTime::currentDateTime()) <= -quotTime * 3600)
		{
			ChatMessage *message;
			if ((*entry).type == HISTORYMANAGER_ENTRY_MSGSEND || (*entry).type == HISTORYMANAGER_ENTRY_CHATSEND)
				message = new ChatMessage(kadu->myself(), senders, (*entry).message, TypeSent, (*entry).date);
			else
			message = new ChatMessage(userlist->byID("Gadu", QString::number((*entry).uin)), senders, (*entry).message, TypeReceived, (*entry).date, (*entry).sdate);
			messages.append(message);
		}

	if (!messages.empty())
		chat->appendMessages(messages);
}

void HistoryModule::chatCreated(ChatWidget *chat)
{
	kdebugf();

	connect(chat, SIGNAL(keyPressed(QKeyEvent *, ChatWidget *, bool &)),
		this, SLOT(chatKeyPressed(QKeyEvent *, ChatWidget *, bool &)));

	connect(chat, SIGNAL(messageSentAndConfirmed(UserListElements, const QString&)),
		this, SLOT(messageSentAndConfirmed(UserListElements, const QString&)));

	// don't do it for already opened chats with discussions
	if (!chat->countMessages())
		appendHistory(chat);

	kdebugf2();
}

void HistoryModule::chatDestroying(ChatWidget *chat)
{
	disconnect(chat, SIGNAL(keyPressed(QKeyEvent *, ChatWidget *, bool &)),
		this, SLOT(chatKeyPressed(QKeyEvent *, ChatWidget *, bool &)));
}

void HistoryModule::messageSentAndConfirmed(UserListElements receivers, const QString& message)
{
	UinsList uins;
	foreach(UserListElement user, receivers)
		uins.append(user.ID("Gadu").toUInt());
	//TODO: throw out UinsList as soon as possible!
	history->addMyMessage(uins, message);
}

void HistoryModule::viewHistory()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();

	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}
	UserListElements users = activeUserBox->selectedUsers();

	UinsList uins;
	foreach(UserListElement user, users)
		uins.append(user.ID("Gadu").toUInt());
	//TODO: throw out UinsList as soon as possible!
	(new HistoryDialog(uins))->show();

	kdebugf2();
}

void HistoryModule::deleteHistory()
{
	kdebugf();
	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}
	//TODO: throw out UinsList as soon as possible!
	UinsList uins;
	UserListElements users = activeUserBox->selectedUsers();
	foreach(UserListElement user, users)
		if (user.usesProtocol("Gadu"))
			uins.append(user.ID("Gadu").toUInt());

	history->removeHistory(uins);
	kdebugf2();
}

void HistoryModule::userboxMenuPopup()
{
	kdebugf();

	UserBox *activeUserBox = UserBox::activeUserBox();
	if (activeUserBox == NULL)
	{
		kdebugf2();
		return;
	}

	UserListElements users = activeUserBox->selectedUsers();
	if (!users.count())
	{
		kdebugf2();
		return;
	}

	int history_item = UserBox::userboxmenu->getItem(tr("History"));
	int delete_history_item = UserBox::management->getItem(tr("Clear history"));

	bool any_ok = false;
	foreach(UserListElement user, users)
		if (!user.protocolList().isEmpty())
		{
			any_ok = true;
			break;
		}
	UserBox::userboxmenu->setItemVisible(history_item, any_ok);
	UserBox::userboxmenu->setItemVisible(delete_history_item, any_ok);

	kdebugf2();
}

void HistoryModule::removingUsers(UserListElements users)
{
	kdebugf();
	if (!users.count())
		return;

	QString altNicks = users.altNicks().join(", ");
	QString tmp;

	for (unsigned int i = 0; i < users.count(); i += 10)
		tmp += (altNicks.section(", ", i, i + 9) + ",\n");

	if (!MessageBox::ask(tr("The following users were deleted:\n%0Do you want to remove history as well?").arg(tmp), "Warning", kadu))
		return;
	QString fname;
	foreach(UserListElement user, users)
	{
		if (user.usesProtocol("Gadu"))
		{
			fname = ggPath("history/") + user.ID("Gadu");
			kdebugmf(KDEBUG_INFO, "deleting %s\n", (const char *)fname.local8Bit());
			QFile::remove(fname);
			QFile::remove(fname + ".idx");
		}
	}
	kdebugf2();
}

void HistoryModule::createDefaultConfiguration()
{
	config_file.addVariable("History", "ChatHistoryCitation", 10);
	config_file.addVariable("History", "ChatHistoryQuotationTime", -24);
	config_file.addVariable("History", "DontSaveStatusChanges", true);
	config_file.addVariable("History", "DontShowStatusChanges", true);
	config_file.addVariable("History", "Logging", true);

	config_file.addVariable("ShortCuts", "kadu_viewhistory", "Ctrl+H");
}

HistoryModule* history_module = 0;
