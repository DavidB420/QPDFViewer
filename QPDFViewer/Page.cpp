/**
 * Copyright 2025 David Badiei
 *
 * This file is part of QPDFViewer, hereafter referred to as the program.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "Page.h"
#include <qpainter.h>
#include "PDFEngine.h"

Page::Page(QWidget* parent, PDFEngine* pdfParent, QImage *img)
{
	//Load pixmap from qimage and set values to defaults
	pagePixmap = QPixmap::fromImage(*img);
	this->parent = pdfParent;
	dragging = false;
	isDragging = false;
	pageNumber = this->parent->getCurrentPage();

	setFixedWidth(pagePixmap.width());
	setFixedHeight(pagePixmap.height());
}

QPixmap Page::getPagePixmap()
{
	return pagePixmap;
}

int Page::getPageNumber()
{
	return pageNumber;
}

PDFEngine* Page::getParent()
{
	return parent;
}

void Page::mousePressEvent(QMouseEvent* event)
{
	//Save first point when lmb is first pressed
	if (event->button() == Qt::LeftButton) {
		dragging = true;
		firstPoint = event->pos();
	}
}

void Page::mouseMoveEvent(QMouseEvent* event)
{
	//Save second point while user is dragging while updating the paint event
	if (dragging) {
		currentPoint = event->pos();
		isDragging = true;
		update();
	}
}

void Page::mouseReleaseEvent(QMouseEvent* event)
{
	//When lmb is released stop drawing rect and display the selected text then reset points to defaults
	if (event->button() == Qt::LeftButton && dragging) {
		dragging = false;
		update();
		if (isDragging) {
			QRectF selectionRect(firstPoint, currentPoint);
			parent->displayTextBox(selectionRect);
		}
		isDragging = false;
		currentPoint = QPointF(0,0);
		firstPoint = QPointF(0,0);
	}
}

void Page::paintEvent(QPaintEvent* event)
{
	//Draw page + any needed rects
	QPainter painter(this);

	painter.drawPixmap(0, 0, pagePixmap);

	if (dragging) {
		QRectF dragRect(firstPoint, currentPoint);
		painter.setBrush(QBrush(Qt::blue, Qt::Dense4Pattern));
		painter.drawRect(dragRect);
	}
}

void Page::drawSelection(QRectF rect)
{
	//Draw selection for searches
	dragging = true;
	firstPoint.setX(rect.x());
	firstPoint.setY(rect.y());
	currentPoint.setX(firstPoint.x() + rect.width());
	currentPoint.setY(firstPoint.y() + rect.height());
	update();
}


