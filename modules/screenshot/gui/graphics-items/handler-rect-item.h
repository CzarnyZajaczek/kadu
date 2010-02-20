/*
 * %kadu copyright begin%
 * Copyright 2010 Rafał Malinowski (rafal.przemyslaw.malinowski@gmail.com)
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

#ifndef HANDLER_RECT_ITEM_H
#define HANDLER_RECT_ITEM_H

#include <QtGui/QGraphicsWidget>

class HandlerRectItem : public QGraphicsWidget
{
	Q_OBJECT

	bool IsMouseButtonPressed;
	QPointF LastMousePosition;

protected:
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);

	virtual QRectF boundingRect() const;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

public:
	explicit HandlerRectItem(QGraphicsItem *parent = 0);
	virtual ~HandlerRectItem();

signals:
	void movedTo(int x, int y);

};

#endif // HANDLER_RECT_ITEM_H
