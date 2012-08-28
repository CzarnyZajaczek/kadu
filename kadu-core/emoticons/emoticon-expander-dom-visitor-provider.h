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

#ifndef EMOTICON_EXPANDER_DOM_VISITOR_PROVIDER
#define EMOTICON_EXPANDER_DOM_VISITOR_PROVIDER

#include <QtCore/QScopedPointer>

#include "dom/ignore-links-dom-visitor.h"
#include "emoticons/emoticon-prefix-tree.h"
#include "emoticons/emoticons.h"
#include "emoticons/emoticon-expander.h"

#include "dom/dom-visitor-provider.h"

class EmoticonExpanderDomVisitorProvider : public DomVisitorProvider
{
	QScopedPointer<EmoticonExpander> Expander;
	QScopedPointer<IgnoreLinksDomVisitor> LinksVisitor;

	QScopedPointer<EmoticonPrefixTree> Tree;
	EmoticonsStyle Style;

	void rebuildVisitor();

public:
	EmoticonExpanderDomVisitorProvider();
	virtual ~EmoticonExpanderDomVisitorProvider();

	virtual DomVisitor * provide() const;

	void setEmoticonTree(EmoticonPrefixTree *);
	void setStyle(EmoticonsStyle style);

};

#endif // EMOTICON_EXPANDER_DOM_VISITOR_PROVIDER
