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

#ifndef FORMATTED_STRING_PLAIN_TEXT_VISITOR_H
#define FORMATTED_STRING_PLAIN_TEXT_VISITOR_H

#include <QtCore/QString>

#include "formatted-string/formatted-string-visitor.h"

class FormattedStringPlainTextVisitor : public FormattedStringVisitor
{
	Q_DISABLE_COPY(FormattedStringPlainTextVisitor);

	QString Result;

public:
	FormattedStringPlainTextVisitor();
	virtual ~FormattedStringPlainTextVisitor();

	virtual void visit(const CompositeFormattedString * const compositeFormattedString);
	virtual void visit(const FormattedStringPart * const formattedStringPart);

	QString result() const;

};

#endif // FORMATTED_STRING_PLAIN_TEXT_VISITOR_H
