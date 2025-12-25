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

#include "PDFEngine.h"

#include <qimage.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qtreeview.h>
#include <qstandarditemmodel.h>
#include <string>
#include <poppler/cpp/poppler-global.h>
#include <poppler/cpp/poppler-toc.h>
#include <poppler/cpp/poppler-destination.h>
#include "Page.h"
#include "TextBoxDialog.h"
#include "StringConv.h"
#include "NavigationBar.h"

PDFEngine::PDFEngine(std::string fileName, QWidget *parentWindow)
{
	//Save parent window from viewer
	this->parentWindow = parentWindow;
	
	//Load pdf doc
	doc = poppler::document::load_from_file(fileName);

	//Initialize variables to defaults
	outputLabel = NULL;

	currentPage = 1;
	scaleValue = 100;

	selectedRect = poppler::rectf(0, 0, 0, 0);

	pdfRotation = poppler::rotate_0;

	documentHeight = 0;

	foundPageNum = -1;
}

Page *PDFEngine::returnImage()
{
	//Create poppler image based on current page, rotation and scale values
	poppler::page* page = doc->create_page(currentPage-1);
	poppler::page_renderer pr;
	pr.set_render_hint(poppler::page_renderer::antialiasing, true);
	pr.set_render_hint(poppler::page_renderer::text_antialiasing, true);
	poppler::image img = pr.render_page(page,
		(float)72 * scaleValue / 75, (float)72 * scaleValue / 75,
		-1, -1, -1, -1, pdfRotation);

	//Convert the poppler into QImage
	QImage image((uchar*)img.data(), img.width(), img.height(), img.bytes_per_row(), QImage::Format_ARGB32);

	//Create page object based on QImage
	outputLabel = new Page(this->parentWindow, this, &image);
	outputLabel->resize(img.width(), img.height());
	
	//If there has been a selection from searching in the past, be sure to show it
	if (selectedRect.x() != 0 && selectedRect.y() != 0 && selectedRect.width() != 0 && selectedRect.height() != 0 && getCurrentPage() == foundPageNum) {
		QRectF rect(selectedRect.x() * scaleValue / 75, selectedRect.y() * scaleValue / 75, selectedRect.width() * scaleValue / 75, selectedRect.height() * scaleValue / 75);
		outputLabel->drawSelection(rect);
		selectedRect = poppler::rectf(0, 0, 0, 0);
		foundPageNum == -1;
	}

	//Delete poppler page
	delete page;

	return outputLabel;
}

int PDFEngine::getTotalNumberOfPages()
{
	return doc->pages();
}

int PDFEngine::getCurrentPage()
{
	return currentPage;
}

int PDFEngine::getScaleValue()
{
	return scaleValue;
}

bool PDFEngine::setCurrentPage(int page)
{
	if (page <= 0 || page > getTotalNumberOfPages())
		return false;

	currentPage = page;

	return true;
}

bool PDFEngine::setCurrentScale(int scale)
{
	if (scale <= 0)
		return false;

	scaleValue = scale;

	return true;
}

bool PDFEngine::findPhraseInDocument(std::string phrase, poppler::page::search_direction_enum direction)
{
	//Start with defaults
	int currentSearch = currentPage;
	bool result = false;
	selectedRect = poppler::rectf(0, 0, 0, 0);

	while (currentSearch >= 1 && currentSearch <= getTotalNumberOfPages()) {
		poppler::page* page = doc->create_page(currentSearch - 1);

		//If its a new page reset the page rect, otherwise do not so the search picks more than just the first item on the page
		if (currentPage != currentSearch)
			foundRect = page->page_rect();

		//If we are searching backwards and are on a new page we have search all the way forward on that page to search backwards
		if (direction == poppler::page::search_previous_result && currentPage != currentSearch) {
			result = true;
			int iter = 0;
			while (result) {
				result = page->search(fromStdStringToPopplerString(phrase), foundRect, poppler::page::search_next_result, poppler::case_insensitive, pdfRotation);
				iter++;
			}
			//If there is one result on the page, just say we found a result
			if (iter == 2)
				result = true;
		}
		
		//If result is still false, try to do a search in the right direction
		if (!result)
			result = page->search(fromStdStringToPopplerString(phrase), foundRect, direction, poppler::case_insensitive, pdfRotation);

		//If a result has been found, select it, then break out of the loop
		if (result) {
			selectedRect = poppler::rectf(foundRect.x(), foundRect.y(), foundRect.width(), foundRect.height());
			currentPage = currentSearch;
			foundPageNum = currentSearch;
			currentSearch = -1;
		}

		//Bidirectionally update page if nothing was found on the current page
		if (direction == poppler::page::search_next_result)
			currentSearch++;
		else if (direction == poppler::page::search_previous_result)
			currentSearch--;

		//Delete poppler page when done
		delete page;
	}

	return result;
}

void PDFEngine::displayTextBox(QRectF dim)
{
	//Grab text from selection then display it in a dialog
	poppler::page* page = doc->create_page(currentPage - 1);

	double rectX = dim.x(), rectY = dim.y(), rectWidth = dim.width(), rectHeight = dim.height();
	
	//Check if selected text rectangle has a negative width or height, if so make it positive and update X and Y accordingly
	if (rectWidth < 0) {
		rectWidth = -rectWidth;
		rectX = rectX - rectWidth;
	}

	if (rectHeight < 0) {
		rectHeight = -rectHeight;
		rectY = rectY - rectHeight;
	}

	std::string foundText = fromPopplerStringStdString(page->text(poppler::rectf(rectX / scaleValue * 75, rectY / scaleValue * 75, rectWidth / scaleValue * 75, rectHeight / scaleValue * 75)));
	delete page;
	TextBoxDialog* dialog = new TextBoxDialog(this->parentWindow, &foundText);
	dialog->show();
}

void PDFEngine::displayAllText()
{
	//Display entire text contents of a page
	QRectF rect(0, 0, outputLabel->width(), outputLabel->height());
	displayTextBox(rect);
}

void PDFEngine::addNavOutline(NavigationBar* navBar)
{
	//Add toc to navigation bar if it exists
	poppler::toc *docToc = doc->create_toc();

	if (docToc != NULL) {
		QStandardItemModel* model = new QStandardItemModel;
		
		poppler::toc_item *currentItem  = docToc->root();
		QStandardItem* rootItem = model->invisibleRootItem();

		recursivelyFillModel(currentItem, rootItem, navBar);

		navBar->returnTree()->setModel(model);
	}
}

void PDFEngine::rotatePDF(bool plus90)
{
	//Do either 90 CW or 90 CCW rotation, then update rotation value
	int x = pdfRotation;

	if (plus90) {
		x++;
		if (x == 4)
			x = 0;
	}
	else {
		x--;
		if (x == -1)
			x = 3;
	}

	switch (x) {
	case 0:
		pdfRotation = poppler::rotate_0;
		break;
	case 1:
		pdfRotation = poppler::rotate_90;
		break;
	case 2:
		pdfRotation = poppler::rotate_180;
		break;
	case 3:
		pdfRotation = poppler::rotate_270;
		break;
	}
}

unsigned long PDFEngine::getDocumentHeight()
{
	//Get total document height with spacing
	documentHeight = 0;
	
	updateHeightValues(true);
	
	return documentHeight;
}

QVector<Page*> PDFEngine::getVisiblePages()
{
	QVector <Page*> visiblePages;
	
	int maxHeight = parentWindow->height(); //Use max height as upper limit, after tha
	int i = 0; //stores current height of the pages
	int k = getCurrentPage(); //stores current page for restore later
	int j = k <= 3 ? 1 : k - 2; //Page counter, starts two pages before the current page
	int l = 0; //stores number of pages allowed after the max height limit
	
	//Run until offset limit is reached or we hit the end of the document
	while ((l <= 4) && j <= getTotalNumberOfPages()) {
		//Switch to page to be rendered
		setCurrentPage(j);

		//Only render page if necessary, otherwise keep existing page
		Page* page = NULL;
		for (int m = 0; m < previousPages.length(); m++) {
			if (previousPages.at(m)->getPageNumber() == getCurrentPage() && previousPages.at(m)->getParent() == this && previousPages.at(m)->getScale() == scaleValue && previousPages.at(m)->getRotation() == pdfRotation && 
				previousPages.at(m)->getCurrentPoint().x() == 0 && previousPages.at(m)->getCurrentPoint().y() == 0 && previousPages.at(m)->getFirstPoint().x() == 0 && previousPages.at(m)->getFirstPoint().y() == 0 && foundPageNum != getCurrentPage())
				page = previousPages.at(m);
		}
		if (page == NULL)
			page = returnImage();
		visiblePages.push_back(page);

		//Update current height and page counter
		i += page->height();
		j++;

		//If current height has exceeded max height, we have reached the offset pages
		if (i > maxHeight)
			l++;
	}

	//Restore current page
	setCurrentPage(k);
	
	//Save page list for next run
	previousPages = visiblePages;

	return visiblePages;
}

QVector<int> PDFEngine::getPageHeights()
{
	//Get the heights of all the pages
	allPageHeights.clear();

	updateHeightValues(false);
	
	return allPageHeights;
}

poppler::rotation_enum PDFEngine::getCurrentRotation()
{
	return pdfRotation;
}

bool PDFEngine::setCurrentPageSignal(int page)
{
	//Set current page using normal function
	bool result = setCurrentPage(page);

	//If successful, clear all other pages of selections and notify viewer of page change
	if (result) {
		for (int i = 0; i < previousPages.length(); i++) {
			if (previousPages.at(i)->getPageNumber() != page) {
				QRectF rect(0, 0, 0, 0);
				previousPages.at(i)->drawSelection(rect);
			}
		}
		emit pageChanged();
	}

	return result;
}

void PDFEngine::recursivelyFillModel(poppler::toc_item* currentItem, QStandardItem* rootItem, NavigationBar* navBar)
{
	//Add all the children recursively to the model, in addition saving the page number in a tuple with the item pointer
	for (int i = 0; i < currentItem->children().size(); i++) {
		poppler::toc_item* newItem = currentItem->children().at(i);
		QStandardItem *newModelItem = new QStandardItem(QString::fromStdString(fromPopplerStringStdString(newItem->title())));
		newModelItem->setEditable(false);
		rootItem->appendRow(newModelItem);

		NavTuple nTuple;
		nTuple.pageNum = newItem->destPageNum();
		nTuple.sItem = newModelItem;
		navBar->navItems.push_back(nTuple);

		if (newItem->children().size() > 0)
			recursivelyFillModel(newItem, newModelItem, navBar);
	}
}

void PDFEngine::updateHeightValues(bool total)
{
	//Either calculate the total document height with spacing or get all the page heights
	for (int i = 0; i < getTotalNumberOfPages(); i++) {
		poppler::page* page = doc->create_page(i);
		QRectF pt(page->page_rect().x(), page->page_rect().y(), page->page_rect().width(), page->page_rect().height());

		//If page is rotated 90 or 270, use width instead of height
		int h = int((((pdfRotation == poppler::rotate_0 || pdfRotation == poppler::rotate_180) ? pt.height() : pt.width()) / 72.0f) * (72.0f * scaleValue / 75.0f));

		if (total)
			documentHeight += h + 20;
		else
			allPageHeights.push_back(h);

		delete page;
	}
}

