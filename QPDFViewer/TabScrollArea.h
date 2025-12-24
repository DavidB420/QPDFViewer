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

#ifndef TABSCROLLAREA_H
#define TABSCROLLAREA_H

#include "qscrollarea.h"
#include "Page.h"
#include <QScrollBar>

class TabScrollArea : public QAbstractScrollArea
{
	Q_OBJECT
public:
	TabScrollArea(QWidget* parent = nullptr);
	~TabScrollArea();
	void updateScrollArea(QVector <Page*> *pages, bool runItself=false);
	void setDocumentHeight(unsigned long documentHeight);
	void setCurrentPages(QVector <Page*> *pages);
	void setPageHeights(QVector <int> heights);
	int getPageToLoad();
	void setBufferLock(int value);
	int bufferLock;
protected:
	void wheelEvent(QWheelEvent* event) override;
private:
	bool topOrBottom;
	long documentHeight;
	long verticalScrollValue;
	bool fromScrolling;
	int pageToLoad;
	Page* firstPageHeight;
	QVector <Page*> currentPages;
	QVector <int> allPageHeights;
	void findPageToLoad(long pageToLoad);
	Page* findPage(int pageNum);
signals:
	void hitExtremity();
private slots:
	void onVerticalScrollChanged(int value);
};

#endif