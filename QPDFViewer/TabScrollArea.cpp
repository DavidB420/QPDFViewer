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

#include "TabScrollArea.h"
#include <QWheelEvent>

TabScrollArea::TabScrollArea(QWidget* parent)
{
	topOrBottom = false;
}

bool TabScrollArea::returnTopOrBottom()
{
	return topOrBottom;
}

void TabScrollArea::wheelEvent(QWheelEvent* event)
{
	//run normal qscrollarea wheel code
	QScrollArea::wheelEvent(event);

    // Check if scrolling is enabled, if so just use delta values, otherwise check if we hit an extremity
    QScrollBar* vScrollBar = verticalScrollBar();
    if (!vScrollBar || vScrollBar->maximum() == 0) {
        
        if (event->angleDelta().y() < 0)
            topOrBottom = false;
        else if (event->angleDelta().y() > 0)
            topOrBottom = true;

        emit hitExtremity();
    }
    else {
        // Move to another page if we reach an extremity when scrolling
        if (vScrollBar->value() == vScrollBar->maximum()) {
            topOrBottom = false;
            emit hitExtremity();
        }
        else if (vScrollBar->value() == vScrollBar->minimum()) {
            topOrBottom = true;
            emit hitExtremity();
        }
    }
}