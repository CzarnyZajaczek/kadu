/*
 * %kadu copyright begin%
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Piotr Galiszewski (piotrgaliszewski@gmail.com)
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

#ifndef GADU_EDIT_ACCOUNT_WIDGET_H
#define GADU_EDIT_ACCOUNT_WIDGET_H

#include "gui/widgets/account-edit-widget.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QVBoxLayout;

class GaduPersonalInfoWidget;
class IdentitiesComboBox;
class ProxyGroupBox;

class GaduEditAccountWidget : public AccountEditWidget
{
	Q_OBJECT

	QLineEdit *AccountName;
	QCheckBox *ConnectAtStart;
	QLineEdit *AccountId;
	QLineEdit *AccountPassword;
	QCheckBox *RememberPassword;

	IdentitiesComboBox *Identities;

	GaduPersonalInfoWidget *gpiw;
	ProxyGroupBox *Proxy;

	QCheckBox *useDefaultServers;
	QLineEdit *ipAddresses;
	QComboBox *port;

	QPushButton *ApplyButton;
	QPushButton *CancelButton;

	void createGui();
	void createGeneralTab(QTabWidget *);
	void createPersonalDataTab(QTabWidget *);
	void createBuddiesTab(QTabWidget *);
	void createConnectionTab(QTabWidget *);
	void createGeneralGroupBox(QVBoxLayout *layout);

	void loadAccountData();
	void loadConnectionData();

private slots:
	void dataChanged();
	void removeAccount();
	void importListAsFile();
	void contactListDownloaded(QString);

public:
	explicit GaduEditAccountWidget(Account account, QWidget *parent = 0);
	virtual ~GaduEditAccountWidget();

public slots:
	virtual void apply();
	virtual void cancel();

};

#endif // GADU_EDIT_ACCOUNT_WIDGET_H
