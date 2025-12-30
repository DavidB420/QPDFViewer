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

#ifndef DETACHABLETAB_H
#define DETACHABLETAB_H

#include <qtabbar.h>
#include <qtabwidget.h>

class DetachableTabBar : public QTabBar
{
	Q_OBJECT
public:
	DetachableTabBar(QWidget* parent = 0);
	QString getTabMime();
signals:
	void detachTab(int index, const QPoint& windowPos);
	void tabMerged(int index, QObject* srcViewer);
protected:
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;
	void dropEvent(QDropEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
private:
	int detachIndex;
	QPoint detachStartPos;
	void* viewerPtr;
	QString TAB_MIME;
};

#endif