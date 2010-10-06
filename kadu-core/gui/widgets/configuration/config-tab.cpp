/*
 * %kadu copyright begin%
 * Copyright 2008, 2009, 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QApplication>
#include <QtGui/QGroupBox>
#include <QtGui/QVBoxLayout>
#include <QtGui/QScrollArea>
#include <QtGui/QTabWidget>

#include "gui/widgets/configuration/config-tab.h"
#include "gui/widgets/configuration/config-section.h"
#include "gui/widgets/configuration/config-group-box.h"
#include "gui/widgets/configuration/kadu-scroll-area.h"

ConfigTab::ConfigTab(const QString &name, ConfigSection *configSection, QWidget *mainWidget) :
		MyName(name), MyConfigSection(configSection)
{
	MyScrollArea = new KaduScrollArea(mainWidget);
	MyScrollArea->setFrameStyle(QFrame::NoFrame);
	MyScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	MyScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	MyScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

	MyMainWidget = new QWidget(MyScrollArea);
	MyMainWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	MyMainLayout = new QVBoxLayout(MyMainWidget);
	MyMainLayout->addStretch(1);

	MyScrollArea->setWidget(MyMainWidget);
	MyScrollArea->setWidgetResizable(true);
}

ConfigTab::~ConfigTab()
{
	MyScrollArea->deleteLater();
	MyScrollArea = 0;
}

void ConfigTab::removedConfigGroupBox(const QString &groupBoxName)
{
	MyConfigGroupBoxes.remove(groupBoxName);

	if (!MyConfigGroupBoxes.count())
	{
		MyConfigSection->removedConfigTab(MyName);
		delete this;
	}
}

ConfigGroupBox *ConfigTab::configGroupBox(const QString &name, bool create)
{
	if (MyConfigGroupBoxes.contains(name))
		return MyConfigGroupBoxes[name];

	if (!create)
		return 0;

	QGroupBox *groupBox = new QGroupBox(name, MyMainWidget);
	QHBoxLayout *groupBoxLayout = new QHBoxLayout(groupBox);
	groupBoxLayout->setSizeConstraint(QLayout::SetMinimumSize);

	MyMainLayout->insertWidget(MyConfigGroupBoxes.count(), groupBox);

	ConfigGroupBox *newConfigGroupBox = new ConfigGroupBox(name, this, groupBox);
	MyConfigGroupBoxes[name] = newConfigGroupBox;

	groupBox->show();

	return newConfigGroupBox;
}

QWidget * ConfigTab::scrollWidget() const
{
	return MyScrollArea;
}
