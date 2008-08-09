/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QtCore/QObject>
#include <QtCore/QTimer>
#include <QtGui/QApplication>
#include <QtGui/QCursor>
#include <QtGui/QMenu>
#include <QtGui/QMouseEvent>

#include "kadu-core/config_file.h"
#include "docking.h"
#include "debug.h"
#include "gadu.h"
#include "icons_manager.h"
#include "kadu.h"
#include "pending_msgs.h"
#include "status.h"
#include "misc.h"

#include "activate.h"

/**
 * @ingroup docking
 * @{
 */
extern "C" KADU_EXPORT int docking_init(bool firstLoad)
{
	docking_manager = new DockingManager();
	MainConfigurationWindow::registerUiFile(dataPath("kadu/modules/configuration/docking.ui"), docking_manager);

	return 0;
}

extern "C" KADU_EXPORT void docking_close()
{
	kdebugf();

	MainConfigurationWindow::unregisterUiFile(dataPath("kadu/modules/configuration/docking.ui"), docking_manager);
	delete docking_manager;
	docking_manager = 0;
}

DockingManager::DockingManager()
	: newMessageIcon(StaticEnvelope), icon_timer(new QTimer(this, "icon_timer")), blink(false)
{
	kdebugf();

	createDefaultConfiguration();

	connect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	connect(kadu, SIGNAL(statusPixmapChanged(const QIcon &, const QString &)),
		this, SLOT(statusPixmapChanged(const QIcon &, const QString &)));
	connect(&pending, SIGNAL(messageFromUserAdded(UserListElement)), this, SLOT(pendingMessageAdded()));
	connect(&pending, SIGNAL(messageFromUserDeleted(UserListElement)), this, SLOT(pendingMessageDeleted()));

	connect(kadu, SIGNAL(searchingForTrayPosition(QPoint&)), this, SIGNAL(searchingForTrayPosition(QPoint&)));

	connect(dockMenu, SIGNAL(activated(int)), this, SLOT(dockletChange(int)));
	connect(this, SIGNAL(mousePressMidButton()), &pending, SLOT(openMessages()));

	configurationUpdated();

	kdebugf2();
}

void DockingManager::mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow)
{
}

void DockingManager::configurationUpdated()
{
	if (config_file.readBoolEntry("General", "ShowTooltipInTray"))
		defaultToolTip();
	else
		emit trayTooltipChanged(QString::null);

	IconType it = (IconType)config_file.readNumEntry("Look", "NewMessageIcon");
	if (newMessageIcon != it)
	{
		newMessageIcon = it;
		changeIcon();
	}
}

DockingManager::~DockingManager()
{
	kdebugf();

	disconnect(kadu, SIGNAL(statusPixmapChanged(const QIcon &, const QString &)),
		this, SLOT(statusPixmapChanged(const QIcon &, const QString &)));
	disconnect(&pending, SIGNAL(messageFromUserAdded(UserListElement)), this, SLOT(pendingMessageAdded()));
	disconnect(&pending, SIGNAL(messageFromUserDeleted(UserListElement)), this, SLOT(pendingMessageDeleted()));

	disconnect(kadu, SIGNAL(searchingForTrayPosition(QPoint&)), this, SIGNAL(searchingForTrayPosition(QPoint&)));

	disconnect(dockMenu, SIGNAL(activated(int)), this, SLOT(dockletChange(int)));
	disconnect(icon_timer, SIGNAL(timeout()), this, SLOT(changeIcon()));

	delete icon_timer;
	icon_timer=NULL;
	kdebugf2();
}

void DockingManager::changeIcon()
{
	kdebugf();
	if (pending.pendingMsgs() && !icon_timer->isActive())
	{
		switch (newMessageIcon)
		{
			case AnimatedEnvelope:
				//emit trayMovieChanged(icons_manager->loadAnimatedIcon("MessageAnim"));
				break;
			case StaticEnvelope:
				emit trayPixmapChanged(icons_manager->loadIcon("Message"), "Message");
				break;
			case BlinkingEnvelope:
				if (!blink)
				{
					emit trayPixmapChanged(icons_manager->loadIcon("Message"), "Message");
					icon_timer->start(500,TRUE);
					blink = true;
				}
				else
				{
					const UserStatus &stat = gadu->currentStatus();
					emit trayPixmapChanged(QIcon(stat.pixmap()), stat.toString());
					icon_timer->start(500,TRUE);
					blink = false;
				}
				break;
		}
	}
	else
		kdebugmf(KDEBUG_INFO, "OFF\n");
	kdebugf2();
}

void DockingManager::dockletChange(int id)
{
	if (id < 9)
		kadu->slotHandleState(id);
	else
		kadu->close(true);
}

void DockingManager::pendingMessageAdded()
{
	changeIcon();
}

void DockingManager::pendingMessageDeleted()
{
	if (!pending.pendingMsgs())
	{
		const UserStatus &stat = gadu->currentStatus();
		emit trayPixmapChanged(QIcon(stat.pixmap()), stat.toString());
	}
}

void DockingManager::defaultToolTip()
{
	if (config_file.readBoolEntry("General", "ShowTooltipInTray"))
	{
		QString tiptext = tr("Left click - hide / show window\nMiddle click or Left click - open message");
		tiptext.append(tr("\n\nCurrent status:\n%1")
			.arg(qApp->translate("@default", UserStatus::name(gadu->currentStatus().index()).ascii())));

		if (gadu->currentStatus().hasDescription())
			tiptext.append(tr("\n\nDescription:\n%2").arg(gadu->currentStatus().description()));

		emit trayTooltipChanged(tiptext);
	}
}

void DockingManager::trayMousePressEvent(QMouseEvent * e)
{
	kdebugf();
	if (e->button() == Qt::MidButton)
	{
		emit mousePressMidButton();
		return;
	}

	if (e->button() == Qt::LeftButton)
	{
		emit mousePressLeftButton();
		kdebugm(KDEBUG_INFO, "minimized: %d, visible: %d\n", kadu->isMinimized(), kadu->isVisible());

		if (pending.pendingMsgs() && (e->modifiers() != Qt::ControlModifier))
		{
			pending.openMessages();
			return;
		}

		if(kadu->isMinimized())
		{
			kadu->showNormal();
			activateWindow(kadu->winId());
			return;
		}
		else if(kadu->isVisible())
			kadu->hide();
		else
		{
			kadu->show();
			kadu->raise();

			activateWindow(kadu->winId());
		}
		return;
	}

	if (e->button() == Qt::RightButton)
	{
		emit mousePressRightButton();
		//showPopupMenu(dockMenu);
		return;
	}
	kdebugf2();
}

void DockingManager::statusPixmapChanged(const QIcon &icon, const QString &iconName)
{
 	kdebugf();
	emit trayPixmapChanged(icon, iconName);
	defaultToolTip();
	changeIcon();
}

QIcon DockingManager::defaultPixmap()
{
	return QIcon(gadu->currentStatus().pixmap());
}

void DockingManager::setDocked(bool docked, bool butDontHideOnClose)
{
	kdebugf();
	if (docked)
	{
		changeIcon();
		defaultToolTip();
		if (config_file.readBoolEntry("General", "RunDocked"))
			kadu->setShowMainWindowOnStart(false);
	}
	else
	{
		kdebugm(KDEBUG_INFO, "closing: %d\n", Kadu::closing());
		if (!Kadu::closing())
			kadu->show();
	}
	kadu->setDocked(docked, butDontHideOnClose);
	kdebugf2();
}

void DockingManager::createDefaultConfiguration()
{
	config_file.addVariable("General", "RunDocked", false);
	config_file.addVariable("General", "ShowTooltipInTray", true);
	config_file.addVariable("Look", "NewMessageIcon", 0);
}

DockingManager* docking_manager = NULL;

/** @} */

