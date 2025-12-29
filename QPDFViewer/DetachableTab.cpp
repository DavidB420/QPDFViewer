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

#include "DetachableTab.h"

#include <qapplication.h>
#include <qevent.h>

DetachableTabBar::DetachableTabBar(QWidget* parent)
{
	this->setMovable(true);
	this->setTabsClosable(true);
	
	detachIndex = -1;
}

void DetachableTabBar::mousePressEvent(QMouseEvent* event)
{
	detachIndex = tabAt(event->pos());

	detachStartPos = event->pos();

	QTabBar::mousePressEvent(event);
}

void DetachableTabBar::mouseMoveEvent(QMouseEvent* event)
{
	if ((event->pos() - detachStartPos).manhattanLength() > QApplication::startDragDistance()) {
		if (!rect().contains(event->pos()) && detachIndex >= 0) {
			emit detachTab(detachIndex, mapFromGlobal(event->pos()));
			detachIndex = -1;
		}
	}


	QTabBar::mouseMoveEvent(event);
}

void DetachableTabBar::mouseReleaseEvent(QMouseEvent* event)
{
	detachIndex = -1;
	QTabBar::mouseReleaseEvent(event);
}
