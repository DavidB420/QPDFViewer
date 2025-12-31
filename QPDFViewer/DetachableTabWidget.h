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

#ifndef DETACHABLETABWIDGET_H
#define DETACHABLETABWIDGET_H

#include <qtabwidget.h>

#include "DetachableTabBar.h"

class DetachableTabWidget : public QTabWidget
{
	Q_OBJECT
public:
	DetachableTabWidget(QWidget* parent = 0, DetachableTabBar* tBar = 0);
signals:
	void tabMerged(int index, QObject* srcViewer);
protected:
	void dropEvent(QDropEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;
private:
	void* viewerPtr;
	QString TAB_MIME;
	DetachableTabBar* tBar;
};

#endif