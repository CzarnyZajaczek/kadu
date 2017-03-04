/*
 * %kadu copyright begin%
 * Copyright 2009, 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2009 Wojciech Treter (juzefwt@gmail.com)
 * Copyright 2010, 2011 Piotr Dąbrowski (ultr@ultr.pl)
 * Copyright 2009 Maciej Płaza (plaza.maciej@gmail.com)
 * Copyright 2009 Bartłomiej Zimoń (uzi18@o2.pl)
 * Copyright 2010, 2012, 2013, 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2009, 2010, 2011, 2012, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#include <QtGui/QKeyEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include <QtXml/QDomElement>

#include "configuration/config-file-variant-wrapper.h"
#include "configuration/configuration-manager.h"
#include "core/injected-factory.h"
#include "os/generic/window-geometry-manager.h"
#include "widgets/configuration/config-action-button.h"
#include "widgets/configuration/config-check-box.h"
#include "widgets/configuration/config-color-button.h"
#include "widgets/configuration/config-combo-box.h"
#include "widgets/configuration/config-gg-password-edit.h"
#include "widgets/configuration/config-group-box.h"
#include "widgets/configuration/config-hot-key-edit.h"
#include "widgets/configuration/config-label.h"
#include "widgets/configuration/config-line-edit.h"
#include "widgets/configuration/config-line-separator.h"
#include "widgets/configuration/config-list-widget.h"
#include "widgets/configuration/config-path-list-edit.h"
#include "widgets/configuration/config-preview.h"
#include "widgets/configuration/config-section.h"
#include "widgets/configuration/config-select-file.h"
#include "widgets/configuration/config-select-font.h"
#include "widgets/configuration/config-slider.h"
#include "widgets/configuration/config-spin-box.h"
#include "widgets/configuration/config-syntax-editor.h"
#include "widgets/configuration/config-widget.h"
#include "widgets/configuration/configuration-widget.h"
#include "windows/configuration-window.h"
#include "windows/kadu-window.h"

#include "activate.h"

ConfigurationWindow::ConfigurationWindow(const QString &name, const QString &caption, const QString &section, ConfigurationWindowDataManager *dataManager, QWidget *parent) :
		QDialog(parent, Qt::Window),
		DesktopAwareObject(this),
		m_dataManager{dataManager},
		Name(name),
		Section(section)
{
	setWindowRole("kadu-configuration");

	setAttribute(Qt::WA_DeleteOnClose);
	setWindowTitle(caption);
}

ConfigurationWindow::~ConfigurationWindow()
{
}

void ConfigurationWindow::setConfigurationManager(ConfigurationManager *configurationManager)
{
	m_configurationManager = configurationManager;
}

void ConfigurationWindow::setConfiguration(Configuration *configuration)
{
	m_configuration = configuration;
}

void ConfigurationWindow::setInjectedFactory(InjectedFactory *injectedFactory)
{
	m_injectedFactory = injectedFactory;
}

void ConfigurationWindow::init()
{
	QVBoxLayout *main_layout = new QVBoxLayout(this);

	configurationWidget = m_injectedFactory->makeInjected<ConfigurationWidget>(dataManager(), this);

	QDialogButtonBox *buttons_layout = new QDialogButtonBox(Qt::Horizontal, this);

	QPushButton *okButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogOkButton), tr("Ok"), this);
	buttons_layout->addButton(okButton, QDialogButtonBox::AcceptRole);
	QPushButton *applyButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogApplyButton), tr("Apply"), this);
	buttons_layout->addButton(applyButton, QDialogButtonBox::ApplyRole);
	QPushButton *cancelButton = new QPushButton(qApp->style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Cancel"), this);
	buttons_layout->addButton(cancelButton, QDialogButtonBox::RejectRole);

	connect(okButton, SIGNAL(clicked(bool)), this, SLOT(updateAndCloseConfig()));
	connect(applyButton, SIGNAL(clicked(bool)), this, SLOT(updateConfig()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(reject()));
	connect(cancelButton, SIGNAL(clicked(bool)), this, SLOT(close()));

	main_layout->addWidget(configurationWidget);
	main_layout->addSpacing(16);
	main_layout->addWidget(buttons_layout);

	new WindowGeometryManager(new ConfigFileVariantWrapper(m_configuration, section(), name() + "_Geometry"), QRect(0, 50, 790, 580), this);
}

InjectedFactory * ConfigurationWindow::injectedFactory() const
{
	return m_injectedFactory;
}

void ConfigurationWindow::show()
{
	if (!isVisible())
  	{
		widget()->beforeShow();
		widget()->loadConfiguration();
		QWidget::show();
	}
	else
	{
		_activateWindow(m_configuration, this);
	}
}

void ConfigurationWindow::updateAndCloseConfig()
{
	updateConfig();

	accept();
	close();
}

void ConfigurationWindow::updateConfig()
{
	emit configurationWindowApplied();
	configurationWidget->saveConfiguration();

	emit configurationSaved();
	ConfigurationAwareObject::notifyAll();

	m_configurationManager->flush();
}

void ConfigurationWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		e->accept();
		close();
	}
	else
		QDialog::keyPressEvent(e);
}

#include "moc_configuration-window.cpp"