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

#ifndef PAGE_H
#define PAGE_H

#include <qlabel.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <QMouseEvent>
#include <QPaintEvent>
#include <poppler-qt5.h>

#include "HyperlinkObject.h"

class PDFEngine;

class Page : public QLabel
{
public:
	Page(QWidget* parent = 0, PDFEngine *pdfParent = 0, QImage *img = 0);
	QPixmap getPagePixmap();
	int getPageNumber();
	int getScale();
	QPointF getFirstPoint();
	QPointF getCurrentPoint();
	Poppler::Page::Rotation getRotation();
	PDFEngine* getParent();
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);
	void drawSelection(QRectF rect);
	void addHyperlink(HyperlinkObject* obj);
private:
	bool dragging, isDragging;
	QPixmap pagePixmap;
	QPointF firstPoint, currentPoint;
	PDFEngine* parent;
	int pageNumber, scale;
	Poppler::Page::Rotation rotation;
	QVector <HyperlinkObject*> hyperlinks;
};

#endif

