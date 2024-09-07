/**
 * Copyright 2024 David Badiei
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

#ifndef PAGE_H
#define PAGE_H

#include <qlabel.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <QMouseEvent>
#include <QPaintEvent>

class PDFEngine;

class Page : public QLabel
{
public:
	Page(QWidget* parent = 0, PDFEngine *pdfParent = 0, QImage *img = 0);
	QPixmap getPagePixmap();
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);
	void drawSelection(QRectF rect);
private:
	bool dragging, isDragging;
	QPixmap pagePixmap;
	QPointF firstPoint, currentPoint;
	PDFEngine* parent;
};

#endif

