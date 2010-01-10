/*
 * %kadu copyright begin%
 * Copyright 2010 Bartlomiej Zimon (uzi18@o2.pl)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2009 Michał Podsiadlik (michal@kadu.net)
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

#ifndef CORE_H
#define CORE_H

#include <QtCore/QObject>

#include "accounts/accounts-aware-object.h"
#include "chat/chat.h"
#include "configuration/configuration-aware-object.h"
#include "buddies/buddy.h"
#include "buddies/buddy-list.h"
#include "status/status.h"

#include "exports.h"
#include "kadu-config.h"

class KaduWindow;
class Message;

class KADUAPI Core : public QObject, private AccountsAwareObject, public ConfigurationAwareObject
{
	Q_OBJECT
	Q_DISABLE_COPY(Core)

	static Core *Instance;

	Buddy Myself;
	KaduWindow *Window;
	bool ShowMainWindowOnStart; // TODO: 0.7.1, it is a hack

	Core();
	virtual ~Core();

	void createDefaultConfiguration();
	void createAllDefaultToolbars();

	void init();
	void loadDefaultStatus();

	void storeConfiguration();

private slots:
	void statusChanged();

	void deleteOldConfigurationFiles();
	void kaduWindowDestroyed();

protected:
	virtual void accountAdded(Account account);
	virtual void accountRegistered(Account account);
	virtual void accountUnregistered(Account account);
	virtual void configurationUpdated();

public:
	static Core * instance();
	
	QString version() { return QString(VERSION);}

	Buddy myself() { return Myself; }

	void createGui();
	void setShowMainWindowOnStart(bool show);
	KaduWindow * kaduWindow();

	void setIcon(const QPixmap &icon);

public slots:
	void quit();

signals:
	void connecting();
	void connected();
	void disconnected();

	void messageReceived(const Message &message);
	void messageSent(const Message &message);

	// TODO: remove
	void settingMainIconBlocked(bool &blocked);

	//TODO:
	void searchingForTrayPosition(QPoint &);

	void mainIconChanged(const QIcon &);


};

#endif // CORE_H
