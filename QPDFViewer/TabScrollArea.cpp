/**
 * Copyright 2026 David Badiei
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
    //Displays all visible pages and handles scrolling
    documentHeight = 0;
    firstPageHeight = NULL;
    verticalScrollValue = 0;
    bufferLock = 0;
    horizontalEnabled = false;
    horizontalScrollValue = -1;
    pageToLoad = 0;


    //By default horizontal scroll bar is disabled
    horizontalScrollBar()->setEnabled(horizontalEnabled);
    horizontalScrollBar()->setValue(horizontalScrollValue);

    connect(horizontalScrollBar(), &QScrollBar::valueChanged,
        this, &TabScrollArea::onHorizontalScrollChanged);
}

TabScrollArea::~TabScrollArea()
{
    //Delete all pages on destruction
    for (int i = 0; i < currentPages.length(); i++)
        delete currentPages.at(i);
}

void TabScrollArea::updateScrollArea(QVector <Page*> *pages, bool runItself)
{
    setCurrentPages(pages);

    //Go through all pages, whether they are loaded or not
    for (int i = 0, k = 0, j = (- verticalScrollValue)+20; i < allPageHeights.length(); i++) {
        //Check if the page is loaded 
        if (i + 1 >= currentPages.at(0)->getPageNumber() && i+1 <= currentPages.at(currentPages.length() - 1)->getPageNumber()) {
            //Check if new pages need to be loaded (i.e. having more than half of the current page above the viewport counts as going down and having the whole page above the current coming down from above counts as going up
            if ((runItself && currentPages.at(k) == firstPageHeight && j + (currentPages.at(k)->height() / 2) + 20 < 0 && firstPageHeight != currentPages.at(currentPages.length() - 1)) 
                 || (runItself && k > 0 && currentPages.at(k) == firstPageHeight && j - ((currentPages.at(k - 1)->height()) + 20) > 0)) {
                 triggerEventAndUpdateArea();
                 return;
             }

             currentPages.at(k)->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
             currentPages.at(k)->setParent(viewport());

             //If the horizontal scroll bar is enabled, use its value, otherwise just center all pages based on its width
             currentPages.at(k)->setGeometry(horizontalEnabled ? -horizontalScrollValue : ((viewport()->width() - currentPages.at(k)->width()) / 2), j, currentPages.at(k)->width(), currentPages.at(k)->height());
             currentPages.at(k)->show();

             //Update positioning variable and current page counter
             j += currentPages.at(k)->height() + 20;
             k++;
         }
         else {
             j += allPageHeights.at(i) + 20; //The page isnt loaded, just leave a space its size so the layout stays the same
         }

        //If current page pointer is blank, just give it the current available page immediately
        if (firstPageHeight == NULL)
            firstPageHeight = currentPages.at(i);
    }
}

void TabScrollArea::setDocumentHeight(unsigned long documentHeight, bool recalculateVerticalValue, int pageNum)
{
    //Update document height and vertical scroll bar values accordingly
    this->documentHeight = documentHeight;
    verticalScrollBar()->setRange(0,(this->documentHeight- viewport()->height())+40); //+40 to compensate for starting spacing + horizontal scroll bar
    verticalScrollBar()->setPageStep(viewport()->height());

    //Setting value to 0 is done for loading files, recalculating is done for already loaded file operations, like changing scale or rotation
    if (!recalculateVerticalValue)
        verticalScrollBar()->setValue(0);
    else {
        verticalScrollValue = findPageOffset(pageNum);
        verticalScrollBar()->setValue(verticalScrollValue);
        triggerEventAndUpdateArea();
    }
    verticalScrollValue = verticalScrollBar()->value();
    connect(verticalScrollBar(), &QScrollBar::valueChanged,
        this, &TabScrollArea::onVerticalScrollChanged);
}


void TabScrollArea::triggerEventAndUpdateArea()
{
    //Use a spinlock to wait for loaded page buffer to be updated, not ideal but it happens really fast
    bufferLock = 1;
    
    //Find and set the page that scroll area wants to be loaded
    findPageToLoad(verticalScrollValue);

    //Tells viewer to update page number and tells the tab to tell the engine to update loaded page buffer
    emit hitExtremity();

    //Wait for tab to release the lock after update (cant do anything until this finishes)
    while (bufferLock > 0);

    //Update current page pointer
    firstPageHeight = findPage(pageToLoad);

    //Check if horizontal scroll bar needs to be updated
    horizontalScrollBar()->setEnabled(false);
    horizontalEnabled = checkIfHorizontalScrollRequired();

    //Run scroll area update (recursion not allowed)
    updateScrollArea(&currentPages, false);
}

void TabScrollArea::findPageToLoad(long verticalScrollValue)
{
    //Find page number based on the vertical scroll bar value and update page to load variable
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
    //Find pointer to correct page based on its page number
    for (int i = 0; i < currentPages.length(); i++) {
        if (currentPages.at(i)->getPageNumber() == pageNum)
            return currentPages.at(i);
    }

    return NULL;
}

bool TabScrollArea::checkIfHorizontalScrollRequired()
{
    bool needed = false;
    int maxWidth = 0;

    //Save the maximum width of the loaded pages and check if any of the widths are greater than the viewports width
    for (int i = 0; i < currentPages.length(); i++) {
        maxWidth = qMax(maxWidth, currentPages.at(i)->width());
        
        if (maxWidth > viewport()->width()) {
            horizontalScrollBar()->setEnabled(true);
            needed = true;
        }
    }
    
    //If it is greater, we need a horizontal scroll bar
    if (needed) {
        //Reset horizontal scroll value only when its outside the current range or its at the default value
        if (horizontalScrollValue > maxWidth || horizontalScrollValue < 0) {
            horizontalScrollValue = qBound((long)0, horizontalScrollValue, (long)(maxWidth-viewport()->width()));
            horizontalScrollBar()->setValue(horizontalScrollValue);
        }
        horizontalScrollBar()->setRange(0, maxWidth-viewport()->width());
        horizontalScrollBar()->setPageStep(viewport()->width());
    }

    return needed;
}

long TabScrollArea::findPageOffset(int pageNum)
{
    //Find the vertical scroll offset for a certain page
    long offset = 0;

    for (int i = 0; i < pageNum-1; i++)
        offset += allPageHeights.at(i) + 20;
    
    return offset;
}

void TabScrollArea::onHorizontalScrollChanged(int value)
{
    //Update horizontal scroll value and the scroll area
    QSignalBlocker blocker(horizontalScrollBar());

    horizontalScrollValue = value;

    updateScrollArea(&currentPages, false);

    horizontalScrollBar()->setValue(horizontalScrollValue);
}

void TabScrollArea::setCurrentPages(QVector<Page*> *pages)
{
    //Only run check if the size of loaded pages buffer is greater than 0 and it isnt the loaded buffer being passed as a pointer
    if (currentPages.size() > 0 && pages != &currentPages) {
        //Delete any pages that do not exist in the new page buffer
        for (int i = 0; i < currentPages.size(); i++) {
            if (!pages->contains(currentPages.at(i)) || currentPages.at(i)->getParent() != pages->at(0)->getParent() || currentPages.at(i)->getScale() != pages->at(0)->getScale() || currentPages.at(i)->getRotation() != pages->at(0)->getRotation()){
                delete currentPages.at(i);
                currentPages.removeAt(i);
                i--;
            }
        }
    }

    //Update loaded page buffer
    currentPages = *pages;
}

void TabScrollArea::setPageHeights(QVector<int> heights)
{
    allPageHeights = heights;
}

void TabScrollArea::updateVerticalScrollBar(int pageNum)
{
    //Update vertical scroll bar and scroll area based on a given page number
    verticalScrollBar()->blockSignals(true);
    verticalScrollValue = findPageOffset(pageNum);
    verticalScrollBar()->setValue(verticalScrollValue);
    triggerEventAndUpdateArea();
    verticalScrollBar()->blockSignals(false);
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
    //Update scroll bar value
    QSignalBlocker blocker(verticalScrollBar());
    
    static bool inHandler = false;
    if (inHandler)
        return;

    inHandler = true;

    verticalScrollValue = value;

    //Update scroll area (recursive)
    updateScrollArea(&currentPages,true);
    verticalScrollBar()->setValue(verticalScrollValue);

    inHandler = false;
}

void TabScrollArea::wheelEvent(QWheelEvent* event)
{
    //Runs the vertical scroll code, just with a speed multiplier of 100 applied to the wheel delta
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

void TabScrollArea::resizeEvent(QResizeEvent* event)
{
    QAbstractScrollArea::resizeEvent(event);

    //Assume we dont need a scroll bar, will be enabled if required
    horizontalScrollBar()->setEnabled(false);

    //Check if horizontal scroll bar is needed
    horizontalEnabled = checkIfHorizontalScrollRequired();

    //Update scroll area upon resize
    updateScrollArea(&currentPages, false);
}



