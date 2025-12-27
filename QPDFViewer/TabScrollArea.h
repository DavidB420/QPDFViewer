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
	void setDocumentHeight(unsigned long documentHeight, bool recalculateVerticalValue=false, int pageNum=-1);
	void setCurrentPages(QVector <Page*> *pages);
	void setPageHeights(QVector <int> heights);
	void updateVerticalScrollBar(int pageNum);
	int getPageToLoad();
	void setBufferLock(int value);
protected:
	void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
private:
	long documentHeight;
	long verticalScrollValue;
	long horizontalScrollValue;
	bool horizontalEnabled;
	int pageToLoad;
	int bufferLock;
	Page* firstPageHeight;
	QVector <Page*> currentPages;
	QVector <int> allPageHeights;
	void findPageToLoad(long pageToLoad);
	Page* findPage(int pageNum);
	bool checkIfHorizontalScrollRequired();
	long findPageOffset(int pageNum);
	void triggerEventAndUpdateArea();
signals:
	void hitExtremity();
private slots:
	void onVerticalScrollChanged(int value);
	void onHorizontalScrollChanged(int value);
};

#endif