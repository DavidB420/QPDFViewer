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
    documentHeight = 0;
    firstPageHeight = NULL;
    fromScrolling = false;
    verticalScrollValue = 0;
    bufferLock = 0;
}

TabScrollArea::~TabScrollArea()
{
    for (int i = 0; i < currentPages.length(); i++)
        delete currentPages.at(i);
}

void TabScrollArea::updateScrollArea(QVector <Page*> *pages, bool runItself)
{
    setCurrentPages(pages);

    for (int i = 0, k = 0, j = (- verticalScrollValue)+20; i < allPageHeights.length(); i++) {
         if (i + 1 >= currentPages.at(0)->getPageNumber() && i+1 <= currentPages.at(currentPages.length() - 1)->getPageNumber()) {
             if (runItself && currentPages.at(k) == firstPageHeight && j + (currentPages.at(k)->height() / 2) + 20 < 0 && firstPageHeight != currentPages.at(currentPages.length() - 1)) {
                 bufferLock = 1;
                 findPageToLoad(verticalScrollValue);
                 emit hitExtremity();
                 while (bufferLock > 0);
                 firstPageHeight = findPage(pageToLoad);
                 k = 0;
                 i = 0;
                 j = -verticalScrollValue;
                 updateScrollArea(&currentPages, false);
                 return;
             }
             else if (runItself && k > 0 && currentPages.at(k) == firstPageHeight && j - ((currentPages.at(k - 1)->height()) + 20) > 0) {
                 firstPageHeight = currentPages.at(k - 1);
                 bufferLock = 1;
                 findPageToLoad(verticalScrollValue);
                 emit hitExtremity();
                 while (bufferLock > 0);
                 firstPageHeight = findPage(pageToLoad);
                 k = 0;
                 i = 0;
                 j = -verticalScrollValue;
                 updateScrollArea(&currentPages, false);
                 return;
             }

             currentPages.at(k)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
             currentPages.at(k)->setParent(viewport());

             currentPages.at(k)->setGeometry(qMax(0, viewport()->width() - currentPages.at(k)->width()) / 2, j, currentPages.at(k)->width(), currentPages.at(k)->height());
             currentPages.at(k)->show();

             j += currentPages.at(k)->height() + 20;
             k++;
         }
         else {
             j += allPageHeights.at(i) + 20;
         }

        if (firstPageHeight == NULL)
            firstPageHeight = currentPages.at(i);
    }
}

void TabScrollArea::setDocumentHeight(unsigned long documentHeight)
{
    this->documentHeight = documentHeight;
    int x = this->documentHeight - viewport()->height();
    verticalScrollBar()->setRange(0,(this->documentHeight- viewport()->height())+20);
    verticalScrollBar()->setPageStep(viewport()->height());
    verticalScrollBar()->setValue(0);
    verticalScrollValue = verticalScrollBar()->value();
    connect(verticalScrollBar(), &QScrollBar::valueChanged,
        this, &TabScrollArea::onVerticalScrollChanged);
}

void TabScrollArea::findPageToLoad(long verticalScrollValue)
{
    for (int i = 0 , j = -verticalScrollValue; i < allPageHeights.length(); i++){
        if (j >= 0) {
            pageToLoad = i + 1;
            break;
        }
        j += allPageHeights.at(i) + 20;
    }
}

Page* TabScrollArea::findPage(int pageNum)
{
    for (int i = 0; i < currentPages.length(); i++) {
        if (currentPages.at(i)->getPageNumber() == pageNum)
            return currentPages.at(i);
    }

    return NULL;
}

void TabScrollArea::setCurrentPages(QVector<Page*> *pages)
{
    if (currentPages.size() > 0 && pages != &currentPages) {
        for (int i = 0; i < currentPages.size(); i++) {
            if (!pages->contains(currentPages.at(i)) || currentPages.at(i)->getParent() != pages->at(0)->getParent()) {
                delete currentPages.at(i);
                currentPages.removeAt(i);
                i--;
            }
        }
        if (!fromScrolling) {
            verticalScrollValue = 0;
            firstPageHeight = NULL;
        }
    }

    currentPages = *pages;
}

void TabScrollArea::setPageHeights(QVector<int> heights)
{
    allPageHeights = heights;
}

int TabScrollArea::getPageToLoad()
{
    return pageToLoad;
}

void TabScrollArea::setBufferLock(int value)
{
    if (value <= 0)
        bufferLock = 0;
}

void TabScrollArea::onVerticalScrollChanged(int value)
{
    QSignalBlocker blocker(verticalScrollBar());
    
    static bool inHandler = false;
    if (inHandler)
        return;

    inHandler = true;

    verticalScrollValue = value;

    fromScrolling = true;
    // value = current vertical scroll offset in pixels
    updateScrollArea(&currentPages,true);
    verticalScrollBar()->setValue(verticalScrollValue);

    fromScrolling = false;
    inHandler = false;
}

void TabScrollArea::wheelEvent(QWheelEvent* event)
{
    //run normal qscrollarea wheel code
    constexpr int speedMultiplier = 100;

    int delta = event->angleDelta().y();

    if (delta == 0) {
        event->ignore();
        return;
    }

    QScrollBar* bar = verticalScrollBar();

    bar->setValue(bar->value() - delta * speedMultiplier / 120);

    event->accept();
}



