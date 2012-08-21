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

#ifndef MAIL_URL_EXPANDER_H
#define MAIL_URL_EXPANDER_H

#include "dom/dom-text-regexp-visitor.h"

class MailUrlExpander : public DomTextRegexpVisitor
{
	bool OnlyHref;

public:
	explicit MailUrlExpander(QRegExp regExp, bool onlyHref);
	virtual ~MailUrlExpander();

	virtual QDomNode matchToDomNode(QDomDocument document, QRegExp regExp);

};

#endif // MAIL_URL_EXPANDER_H
