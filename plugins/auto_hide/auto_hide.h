/*
 * %kadu copyright begin%
 * Copyright 2014 Bartosz Brachaczek (b.brachaczek@gmail.com)
 * Copyright 2011, 2013, 2014 Rafał Przemysław Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef AUTO_HIDE_H
#define AUTO_HIDE_H

#include <QtCore/QPointer>
#include <QtCore/QTimer>
#include <injeqt/injeqt.h>

#include "configuration/configuration-aware-object.h"
#include "configuration/gui/configuration-ui-handler.h"
#include "plugin/plugin-root-component.h"

/*!
 * This class provides autohiding Kadu's main window after preset time.
 * \brief This class provides autohiding Kadu's main window after preset time
 */
class AutoHide : public PluginRootComponent, public ConfigurationUiHandler, ConfigurationAwareObject
{
	Q_OBJECT
	Q_INTERFACES(PluginRootComponent)
	Q_PLUGIN_METADATA(IID "im.kadu.PluginRootComponent")

	QPointer<PluginRepository> m_pluginRepository;

	QTimer Timer;
	Idle *MyIdle;
	int IdleTime;
	bool Enabled;

private slots:
	INJEQT_SETTER void setPluginRepository(PluginRepository *pluginRepository);

	void timerTimeoutSlot();

protected:
	virtual void configurationUpdated();
	virtual void mainConfigurationWindowCreated(MainConfigurationWindow *mainConfigurationWindow) override;
	virtual void mainConfigurationWindowDestroyed() override;
	virtual void mainConfigurationWindowApplied() override;

public:
	explicit AutoHide(QObject *parent = 0);
	virtual ~AutoHide();

	virtual bool init();
	virtual void done();

};

#endif // AUTO_HIDE_H
