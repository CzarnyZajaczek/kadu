/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QCheckBox>
#include <QtGui/QGroupBox>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QVBoxLayout>

#include "contacts/contact.h"
#include "buddies/group.h"
#include "buddies/group-manager.h"
#include "misc/misc.h"

#include "buddy-groups-configuration-widget.h"

BuddyGroupsConfigurationWidget::BuddyGroupsConfigurationWidget(Buddy &buddy, QWidget *parent) :
		QWidget(parent), MyBuddy(buddy)
{
	setAttribute(Qt::WA_DeleteOnClose);

	createGui();
}

BuddyGroupsConfigurationWidget::~BuddyGroupsConfigurationWidget()
{
}

void BuddyGroupsConfigurationWidget::createGui()
{
	QVBoxLayout *layout = new QVBoxLayout(this);

	QLabel *label = new QLabel(tr("Add <b>%1</b> to the groups below by checking the box next to the appropriate groups.").arg(MyBuddy.display()), this);
	label->setWordWrap(true);

	layout->addWidget(label);

	GroupList = new QListWidget(this);

	foreach (const Group &group, GroupManager::instance()->items())
	{
		QListWidgetItem *item = new QListWidgetItem(GroupList);
		item->setText(group.name());

		if (MyBuddy.isInGroup(group))
			item->setCheckState(Qt::Checked);
		else
			item->setCheckState(Qt::Unchecked);
	}

	layout->addWidget(GroupList);
}

void BuddyGroupsConfigurationWidget::save()
{
	foreach (const Group &group, MyBuddy.groups())
		MyBuddy.removeFromGroup(group);

	const int count = GroupList->count();
	for (int i = 0; i < count; i++)
	{
		const QListWidgetItem * const item = GroupList->item(i);
		if (item->checkState() == Qt::Checked)
			MyBuddy.addToGroup(GroupManager::instance()->byName(item->text()));
	}
}
