/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009, 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
 * Copyright 2008, 2009, 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#ifndef HISTORY_WINDOW_H
#define HISTORY_WINDOW_H

#include <QtGui/QMainWindow>

class Chat;
class ChatHistoryTab;
class HistoryStorage;
class HistoryTab;

/*!
\class HistoryWindow
\author Juzef, Vogel
*/
class HistoryWindow : public QMainWindow
{
	Q_OBJECT

	friend class History;

	static HistoryWindow *Instance;

	QTabWidget *TabWidget;
	int CurrentTab;

	ChatHistoryTab *ChatTab;
	HistoryTab *StatusTab;
	HistoryTab *SmsTab;

	explicit HistoryWindow(QWidget *parent = 0);
	virtual ~HistoryWindow();

	void createGui();

	void updateData();
	void selectChat(const Chat &chat);

private slots:
	void currentTabChanged(int newTabIndex);
	void storageChanged(HistoryStorage *historyStorage);

protected:
	void keyPressEvent(QKeyEvent *event);

public:
	static void show(const Chat &chat);

};

#endif // HISTORY_WINDOW_H
