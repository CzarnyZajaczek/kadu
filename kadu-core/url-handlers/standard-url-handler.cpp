/*
 * %kadu copyright begin%
 * Copyright 2010, 2011 Piotr Galiszewski (piotr.galiszewski@kadu.im)
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
 * Copyright 2010, 2011 Bartosz Brachaczek (b.brachaczek@gmail.com)
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

#include "configuration/configuration-file.h"
#include "dom/dom-processor.h"
#include "dom/ignore-links-dom-visitor.h"
#include "os/generic/url-opener.h"
#include "url-handlers/standard-url-expander.h"

#include "standard-url-handler.h"

StandardUrlHandler::StandardUrlHandler()
{
	UrlRegExp = QRegExp("\\b(http://|https://|www\\.|ftp://)([^\\s]*)");

	configurationUpdated();
}

bool StandardUrlHandler::isUrlValid(const QByteArray &url)
{
	return UrlRegExp.exactMatch(QString::fromUtf8(url));
}

void StandardUrlHandler::expandUrls(QDomDocument domDocument, bool generateOnlyHrefAttr)
{
	StandardUrlExpander urlExpander(UrlRegExp, generateOnlyHrefAttr, FoldLink, LinkFoldTreshold);
	IgnoreLinksDomVisitor ignoreLinksDomVisitor(&urlExpander);

	DomProcessor domProcessor(domDocument);
	domProcessor.accept(&ignoreLinksDomVisitor);
}

void StandardUrlHandler::openUrl(const QByteArray &url, bool disableMenu)
{
	Q_UNUSED(disableMenu)

	if (!url.contains("://"))
		UrlOpener::openUrl("http://" + url);
	else
		UrlOpener::openUrl(url);
}

void StandardUrlHandler::configurationUpdated()
{
	LinkFoldTreshold = config_file.readNumEntry("Chat", "LinkFoldTreshold");
	FoldLink = config_file.readBoolEntry("Chat", "FoldLink");
}
