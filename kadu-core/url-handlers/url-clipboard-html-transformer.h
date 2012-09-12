/*
 * %kadu copyright begin%
 * Copyright 2012 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef URL_CLIPBOARD_HTML_TRANSFORMER_H
#define URL_CLIPBOARD_HTML_TRANSFORMER_H

#include <QtCore/QRegExp>

#include "gui/services/clipboard-html-transformer.h"

class UrlClipboardHtmlTransformer : public ClipboardHtmlTransformer
{
	QRegExp UrlRegExp;

public:
	UrlClipboardHtmlTransformer();
	virtual ~UrlClipboardHtmlTransformer();

	virtual QString transform(const QString &clipboardHtml);

};

#endif // URL_CLIPBOARD_HTML_TRANSFORMER_H
