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

    documentHeight = 0;
    viewportHeight = 20, goToNextPageHeight = viewportHeight;
    firstPageHeight = NULL;

    //verticalScrollBar()->setRange()
}

void TabScrollArea::updateScrollArea(QVector <Page*> *pages)
{
    if (currentPages.size() > 0 && pages != &currentPages) {
        int j = 0;
        for (int i = 0; i < currentPages.size(); i++) {
            if (!pages->contains(currentPages.at(i))) {
                delete currentPages.at(i);
                currentPages.removeAt(i);
                i--;
                j++;
            }
        }
        j = j;
        //viewportHeight = 20;
        //if (dontRunAgain < 1) //seems to run twice???
        //    firstPageHeight = NULL;
    }

    currentPages = *pages;

    for (int i = 0, j = viewportHeight; i < currentPages.length(); i++) {
         if (currentPages.at(i) == firstPageHeight && j + (currentPages.at(i)->height()/2) + 20 < 0 && firstPageHeight != currentPages.at(currentPages.length()-1)) {
             topOrBottom = false;
             viewportHeight = currentPages.at(1)->getPageNumber() > 1 ? viewportHeight += currentPages.at(i)->height() + 20 : viewportHeight;
             firstPageHeight = currentPages.at(i+1);
             emit hitExtremity();
             break;
         }

        currentPages.at(i)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        currentPages.at(i)->setParent(viewport());

        currentPages.at(i)->setGeometry(qMax(0,viewport()->width() - currentPages.at(i)->width()) / 2, j, currentPages.at(i)->width(), currentPages.at(i)->height());
        currentPages.at(i)->show();

        j += currentPages.at(i)->height() + 20;

        if (firstPageHeight == NULL)
            firstPageHeight = currentPages.at(i);
    }
}

void TabScrollArea::setDocumentHeight(unsigned long documentHeight)
{
    this->documentHeight = documentHeight;
    verticalScrollBar()->setRange(0,this->documentHeight);
    verticalScrollBar()->setPageStep(viewport()->height());
}

bool TabScrollArea::returnTopOrBottom()
{
	return topOrBottom;
}

void TabScrollArea::wheelEvent(QWheelEvent* event)
{
    //run normal qscrollarea wheel code
    QAbstractScrollArea::wheelEvent(event);

    int delta = event->angleDelta().y();

    if (delta < 0 && viewportHeight > -documentHeight) {
        viewportHeight += delta;
        updateScrollArea(&currentPages);
    }
    
    /*else if (delta > 0 && viewportHeight < 20) {
        viewportHeight += delta;
        updateScrollArea(&currentPages);
    }*/

    // Check if scrolling is enabled, if so just use delta values, otherwise check if we hit an extremity
    /*QScrollBar* vScrollBar = verticalScrollBar();
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
    }*/
}