/*
 * %kadu copyright begin%
 * Copyright 2009, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010, 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009, 2010, 2011 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
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

#ifndef STATUS_WINDOW_H
#define STATUS_WINDOW_H

#include <QtGui/QDialog>

#include "os/generic/desktop-aware-object.h"

class QComboBox;
class QLabel;
class QTextEdit;

class StatusContainer;

class StatusWindow : public QDialog, DesktopAwareObject
{
	Q_OBJECT

	static QMap<QWidget *, StatusWindow *> Dialogs;

	QComboBox *StatusList;
	QTextEdit *DescriptionEdit;
	QLabel *DescriptionLimitCounter;
	QPushButton *OkButton;

	QList<StatusContainer *> StatusContainers;
	StatusContainer *FirstStatusContainer;

	explicit StatusWindow(const QList<StatusContainer *> &statusContainerList, QWidget *parent = 0);
	virtual ~StatusWindow();

private slots:
	void applyStatus();
	void checkDescriptionLengthLimit();
	void openDescriptionsList();
  	void descriptionSelected(const QString &description);

protected:
	virtual bool eventFilter(QObject *source, QEvent *event);

public:
	/**
	 * @short Shows new status window.
	 * @param statusContainerList list of statusContainers
	 * @param parent dialog's parent widget, null by default
	 * @return Pointer to the created StatusWindow dialog.
	 *
	 * Creates and shows new status window.
	 */
	static StatusWindow * showDialog(const QList<StatusContainer *> &statusContainerList, QWidget *parent = 0);

	virtual QSize sizeHint() const;
};

#endif // STATUS_WINDOW_H
