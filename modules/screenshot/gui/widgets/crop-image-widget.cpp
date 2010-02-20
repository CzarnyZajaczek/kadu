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

#include <QtGui/QCursor>
#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QResizeEvent>

#include "gui/graphics-items/selection-frame-item.h"

#include "crop-image-widget.h"

CropImageWidget::CropImageWidget(QWidget *parent) :
		QGraphicsView(parent), IsMouseButtonPressed(false)
{
	setContentsMargins(0, 0, 0, 0);
	setFrameShape(NoFrame);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setInteractive(true);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	QGraphicsScene *graphicsScene = new QGraphicsScene(this);
	setScene(graphicsScene);

	PixmapItem = new QGraphicsPixmapItem();
	PixmapItem->setCursor(Qt::CrossCursor);
	PixmapItem->setX(0);
	PixmapItem->setY(0);

	scene()->addItem(PixmapItem);

	SelectionFrame = new SelectionFrameItem();
	SelectionFrame->setPos(0, 0);
	SelectionFrame->setSize(size());

	scene()->addItem(SelectionFrame);

	TopLeftHandler = new HandlerRectItem(HandlerTopLeft);
	TopLeftHandler->setCursor(Qt::SizeFDiagCursor);
	connect(TopLeftHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(TopLeftHandler);

	TopHandler = new HandlerRectItem(HandlerTop);
	TopHandler->setCursor(Qt::SizeVerCursor);
	connect(TopHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(TopHandler);

	TopRightHandler = new HandlerRectItem(HandlerTopRight);
	TopRightHandler->setCursor(Qt::SizeBDiagCursor);
	connect(TopRightHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(TopRightHandler);

	LeftHandler = new HandlerRectItem(HandlerLeft);
	LeftHandler->setCursor(Qt::SizeHorCursor);
	connect(LeftHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(LeftHandler);

	RightHandler = new HandlerRectItem(HandlerRight);
	RightHandler->setCursor(Qt::SizeHorCursor);
	connect(RightHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(RightHandler);

	BottomLeftHandler = new HandlerRectItem(HandlerBottomLeft);
	BottomLeftHandler->setCursor(Qt::SizeBDiagCursor);
	connect(BottomLeftHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(BottomLeftHandler);

	BottomHandler = new HandlerRectItem(HandlerBottom);
	BottomHandler->setCursor(Qt::SizeVerCursor);
	connect(BottomHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(BottomHandler);

	BottomRightHandler = new HandlerRectItem(HandlerBottomRight);
	BottomRightHandler->setCursor(Qt::SizeFDiagCursor);
	connect(BottomRightHandler, SIGNAL(movedTo(HandlerType,int,int)), this, SLOT(handlerMovedTo(HandlerType,int,int)));
	scene()->addItem(BottomRightHandler);

	CropRect.setTopLeft(QPoint(0, 0));
	CropRect.setSize(size());
	updateCropRectDisplay();
}

CropImageWidget::~CropImageWidget()
{
}

void CropImageWidget::normalizeCropRect()
{
	CropRect = CropRect.normalized();
	updateCropRectDisplay();
}

void CropImageWidget::updateCropRectDisplay()
{
	QRect normalized = CropRect.normalized();
	SelectionFrame->setSelection(normalized);

	int xMiddle = (normalized.left() + normalized.right()) / 2;
	int yMiddle = (normalized.top() + normalized.bottom()) / 2;

	TopLeftHandler->setPos(CropRect.left(), CropRect.top());
	TopHandler->setPos(xMiddle, CropRect.top());
	TopRightHandler->setPos(CropRect.right(), CropRect.top());
	LeftHandler->setPos(CropRect.left(), yMiddle);
	RightHandler->setPos(CropRect.right(), yMiddle);
	BottomLeftHandler->setPos(CropRect.left(), CropRect.bottom());
	BottomHandler->setPos(xMiddle, CropRect.bottom());
	BottomRightHandler->setPos(CropRect.right(), CropRect.bottom());

	scene()->update(scene()->sceneRect());
}

void CropImageWidget::handlerMovedTo(HandlerType type, int x, int y)
{
	if (type == HandlerTopLeft || type == HandlerTop || type == HandlerTopRight)
		CropRect.setTop(y);
	else if (type == HandlerBottomLeft || type == HandlerBottom || type == HandlerBottomRight)
		CropRect.setBottom(y);

	if (type == HandlerTopLeft || type == HandlerLeft || type == HandlerBottomLeft)
		CropRect.setLeft(x);
	else if (type == HandlerTopRight || type == HandlerRight || type == HandlerBottomRight)
		CropRect.setRight(x);

	updateCropRectDisplay();
}

void CropImageWidget::mousePressEvent(QMouseEvent *event)
{
	QGraphicsView::mousePressEvent(event);
// 	if (event->isAccepted())
// 		return;
// 
// 	if (event->button() != Qt::LeftButton)
// 		return;
// 
// 	IsMouseButtonPressed = true;
// 
// 	CropRect.setTopLeft(event->pos());
// 	CropRect.setBottomRight(event->pos());
}

void CropImageWidget::mouseReleaseEvent(QMouseEvent *event)
{
	QGraphicsView::mouseReleaseEvent(event);
	normalizeCropRect();
// 	if (event->isAccepted())
// 		return;
// 
// 	if (event->button() != Qt::LeftButton)
// 		return;
// 
// 	IsMouseButtonPressed = false;
// 
// 	CropRect.setBottomRight(event->pos());
// 	updateCropRectDisplay();
}

void CropImageWidget::mouseMoveEvent(QMouseEvent *event)
{
	QGraphicsView::mouseMoveEvent(event);
// 	if (event->isAccepted())
// 		return;
// 
// 	if (!IsMouseButtonPressed)
// 		return;
// 
// 	CropRect.setBottomRight(event->pos());
// 	updateCropRectDisplay();
}

void CropImageWidget::resizeEvent(QResizeEvent *event)
{
	SelectionFrame->setSize(event->size());
    QGraphicsView::resizeEvent(event);

	scene()->setSceneRect(QRectF(0, 0, event->size().width(), event->size().height()));
}

void CropImageWidget::setPixmap(QPixmap pixmap)
{
	PixmapItem->setPixmap(pixmap);
}

void CropImageWidget::setCropRect(QRect cropRect)
{
	CropRect = cropRect.normalized();
	updateCropRectDisplay();
}
