/*
 * %kadu copyright begin%
 * Copyright 2012 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef KADU_DIALOG_H
#define KADU_DIALOG_H

#include <QtWidgets/QDialog>

#include "exports.h"
#include "os/generic/desktop-aware-object.h"

class DialogWidget;
class QHBoxLayout;
class QLabel;
class QVBoxLayout;
class QPushButton;
class TitleWidget;

class KADUAPI KaduDialog : public QDialog, DesktopAwareObject
{
    Q_OBJECT

    DialogWidget *CentralWidget;

    QHBoxLayout *horizontalLayout;
    QLabel *iconLabel;
    QVBoxLayout *verticalLayout;
    TitleWidget *Title;

    QPushButton *OkButton;
    QPushButton *CancelButton;

    void createGui();
    void createButtonBox();

private slots:
    void widgetValidated(bool valid);

public:
    explicit KaduDialog(DialogWidget *dialogWidget, QWidget *parent);
    virtual ~KaduDialog();

    void setAcceptButtonText(const QString &text);
    void setCancelButtonText(const QString &text);
};

#endif   // KADU_DIALOG_H
