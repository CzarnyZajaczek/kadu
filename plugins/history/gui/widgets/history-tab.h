/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HISTORY_TAB_H
#define HISTORY_TAB_H

#include <QtGui/QWidget>

#include "exports.h"

class QDate;
class QMenu;

class DatesModelItem;
class HistoryDatesModel;
class TimelineChatMessagesView;

class KADUAPI HistoryTab : public QWidget
{
	Q_OBJECT

	QMenu *TimelinePopupMenu;
	TimelineChatMessagesView *TimelineView;
	HistoryDatesModel *DatesModel;

private slots:
	void currentDateChanged();

	void showTimelinePopupMenu(const QPoint &pos);
	void removeEntries();

protected:
	void keyPressEvent(QKeyEvent *event);

	TimelineChatMessagesView * timelineView() const;
	void setDates(const QVector<DatesModelItem> &dates);

	void createGui();
	virtual void createTreeView(QWidget *parent) = 0;
	virtual void displayForDate(const QDate &date) = 0;
	virtual void removeEntriesPerDate(const QDate &date) = 0;

public:
	explicit HistoryTab(bool showTitleInTimeline, QWidget *parent = 0);
	virtual ~HistoryTab();

	virtual void updateData() = 0;

};

#endif // HISTORY_TAB_H
