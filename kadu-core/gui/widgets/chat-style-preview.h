/*
 * %kadu copyright begin%
 * Copyright 2014 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#pragma once

#include "chat/style-engine/chat-messages-renderer.h"
#include "configuration/configuration-aware-object.h"
#include "misc/memory.h"

#include <QtGui/QFrame>

class KaduWebView;

class ChatStylePreview : public QFrame, public ConfigurationAwareObject
{
	Q_OBJECT

public:
	explicit ChatStylePreview(QWidget *parent = nullptr);
	virtual ~ChatStylePreview();

	void setRenderer(std::unique_ptr<ChatMessagesRenderer> renderer);

protected:
	virtual void configurationUpdated();

private:
	qobject_ptr<KaduWebView> m_view;
	qobject_ptr<HtmlMessagesRenderer> m_htmlMessagesRenderer;
	std::unique_ptr<ChatMessagesRenderer> m_messagesRenderer;

	void preparePreview();

};
