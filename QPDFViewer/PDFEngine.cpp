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

#include "PDFEngine.h"

#include <qimage.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qtreeview.h>
#include <qstandarditemmodel.h>
#include <string>
#include <qmessagebox.h>
#include <poppler-qt5.h>
#include <qthread.h>
#include <qfileinfo.h>
#include "Page.h"
#include "Viewer.h"
#include "TextBoxDialog.h"
#include "NavigationBar.h"
#include "HyperlinkObject.h"
#include "PasswordBoxDialog.h"
#include "FindAllWorker.h"

PDFEngine::PDFEngine(std::string fileName, QWidget *parentWindow)
{
	//Save parent window from viewer
	this->parentWindow = parentWindow;
	
	//Before loading document, set success to true
	success = true;

	//Load pdf doc
	this->fileName = fileName;
	
	if (!refreshEngine())
		return;

	//Unlock document if necessary
	unlockDocument();

	//Initialize variables to defaults
	outputLabel = NULL;

	currentPage = 1;
	scaleValue = 100;

	selectedRect = QRectF(0, 0, 0, 0);

	pdfRotation = Poppler::Page::Rotate0;

	documentHeight = 0;

	foundPageNum = -1;

	searchPos = 0;

	currentFindAllWorker = NULL;

	rerender = false;

	qRegisterMetaType<SearchResult>("SearchResult");
}

PDFEngine::~PDFEngine()
{
	delete doc;
}

Page *PDFEngine::returnImage()
{
	//Create poppler image based on current page, rotation and scale values
	Poppler::Page* page = doc->page(currentPage-1);

	//Reload if page is unavailable
	if (page == NULL) {
		page = reloadDocAndPage();
		if (page == NULL)
			return NULL;
	}

	//Convert the poppler into QImage
	QImage image = page->renderToImage((float)72 * scaleValue / 75, (float)72 * scaleValue / 75,
		-1, -1, -1, -1, pdfRotation);

	//Reload page if image is null
	if (image.isNull()) {
		page = reloadDocAndPage();
		image = page->renderToImage((float)72 * scaleValue / 75, (float)72 * scaleValue / 75,
			-1, -1, -1, -1, pdfRotation);
		if (image.isNull())
			return NULL;
	}

	//Create page object based on QImage
	outputLabel = new Page(this->parentWindow, this, &image);
	outputLabel->resize(image.width(), image.height());

	addHyperlinksToPage(outputLabel, page, image);
	
	//If there has been a selection from searching in the past, be sure to show it
	if (selectedRect.x() != 0 && selectedRect.y() != 0 && selectedRect.width() != 0 && selectedRect.height() != 0 && getCurrentPage() == foundPageNum) {
		QRectF rect(selectedRect.x() * scaleValue / 75, selectedRect.y() * scaleValue / 75, selectedRect.width() * scaleValue / 75, selectedRect.height() * scaleValue / 75);
		outputLabel->drawSelection(rect);
		selectedRect = QRectF(0, 0, 0, 0);
		foundPageNum = -1;
	}

	//Delete poppler page
	delete page;

	return outputLabel;
}

int PDFEngine::getTotalNumberOfPages()
{
	return doc->numPages();
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

bool PDFEngine::findPhraseInDocument(std::string phrase, Poppler::Page::SearchDirection direction)
{
	//Start with defaults
	int currentSearch = currentPage;
	bool result = false;
	selectedRect = QRectF(0, 0, 0, 0);

	while (currentSearch >= 1 && currentSearch <= getTotalNumberOfPages()) {
		Poppler::Page* page = doc->page(currentSearch - 1);

		//If its a new page reset the page rect, otherwise do not so the search picks more than just the first item on the page
		if (currentPage != currentSearch)
			foundRect = QRectF(0.0, 0.0, page->pageSizeF().width(), page->pageSizeF().height());

		//If we are searching backwards and are on a new page we have search all the way forward on that page to search backwards
		if (direction == Poppler::Page::SearchDirection::PreviousResult && currentPage != currentSearch) {
			result = true;
			int iter = 0;
			while (result) {
				result = documentSearch(page, currentSearch, phrase, &foundRect, Poppler::Page::NextResult, pdfRotation);
				iter++;
			}
			//If there is one result on the page, just say we found a result
			if (iter == 2)
				result = true;
		}
		
		//If result is still false, try to do a search in the right direction
		if (!result)
			result = documentSearch(page, currentSearch, phrase, &foundRect, direction, pdfRotation);

		//If a result has been found, select it, then break out of the loop
		if (result) {
			selectedRect = QRectF(foundRect.x(), foundRect.y(), foundRect.width(), foundRect.height());
			currentPage = currentSearch;
			foundPageNum = currentSearch;
			currentSearch = -1;
		}

		//Bidirectionally update page if nothing was found on the current page
		if (direction == Poppler::Page::SearchDirection::NextResult)
			currentSearch++;
		else if (direction == Poppler::Page::SearchDirection::PreviousResult)
			currentSearch--;

		//Delete poppler page when done
		delete page;
	}

	return result;
}

void PDFEngine::displayTextBox(QRectF dim)
{
	//Grab text from selection then display it in a dialog
	Poppler::Page* page = doc->page(currentPage - 1);

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

	QRectF grabRect;

	QRectF pt(0.0, 0.0, page->pageSizeF().width(), page->pageSizeF().height());

	double x = rectX / scaleValue * 75;
	double y = rectY / scaleValue * 75;
	double width = rectWidth / scaleValue * 75;
	double height = rectHeight / scaleValue * 75;

	//Transform rectangle back to original unrotated version
	switch (pdfRotation) {
	case 1:
		grabRect = QRectF(y, pt.height() - x - width, height, width);
		break;
	case 2:
		grabRect = QRectF(pt.width() - x - width, pt.height() - y - height, width, height);
		break;
	case 3:
		grabRect = QRectF(pt.width() - y - height, x, height, width);
		break;
	case 0:
	default:
		grabRect = QRectF(x, y, width, height);
		break;
	}

	std::string foundText = page->text(grabRect).toStdString();
	delete page;
	TextBoxDialog* dialog = new TextBoxDialog(this->parentWindow, &foundText);
	dialog->setAttribute(Qt::WA_DeleteOnClose);
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
	QVector<Poppler::OutlineItem> outline = doc->outline();

	if (!outline.isEmpty()) {
		QStandardItemModel* model = new QStandardItemModel;
		
		//QDomElement currentItem  = docToc->documentElement();
		QStandardItem* rootItem = model->invisibleRootItem();

		recursivelyFillModel(outline, rootItem, navBar);

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
		pdfRotation = Poppler::Page::Rotate0;
		break;
	case 1:
		pdfRotation = Poppler::Page::Rotate90;
		break;
	case 2:
		pdfRotation = Poppler::Page::Rotate180;
		break;
	case 3:
		pdfRotation = Poppler::Page::Rotate270;
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
			if (!rerender && previousPages.at(m)->getPageNumber() == getCurrentPage() && previousPages.at(m)->getParent() == this && previousPages.at(m)->getScale() == scaleValue && previousPages.at(m)->getRotation() == pdfRotation && 
				previousPages.at(m)->getCurrentPoint().x() == 0 && previousPages.at(m)->getCurrentPoint().y() == 0 && previousPages.at(m)->getFirstPoint().x() == 0 && previousPages.at(m)->getFirstPoint().y() == 0 && foundPageNum != getCurrentPage())
				page = previousPages.at(m);
		}
		if (page == NULL)
			page = returnImage();
		if (page == NULL)
			return QVector<Page*>{};
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

Poppler::Page::Rotation PDFEngine::getCurrentRotation()
{
	return pdfRotation;
}

bool PDFEngine::getSuccess()
{
	return success;
}

bool PDFEngine::getAllSearchResults(int direction, std::string phrase)
{
	//Kill all workers currently running
	cancelFindAllWorker();
	
	//Check if file is still available before running
	std::string foundFileName = checkFileAvailable(fileName);
	
	if (foundFileName != "") {
		//Create a find all worker with proper parameters and run in seperate thread
		currentFindAllWorker = new FindAllWorker(QString::fromStdString(foundFileName), QString::fromStdString(phrase), getCurrentPage(), getTotalNumberOfPages(), direction, getCurrentRotation());
		currentFindAllThread = new QThread(this);

		currentFindAllWorker->moveToThread(currentFindAllThread);

		//Set up all signals for running the worker, checking when a result is ready, and when the worker is finished
		connect(currentFindAllThread, &QThread::started, currentFindAllWorker, &FindAllWorker::run);
		connect(currentFindAllWorker, &FindAllWorker::finishedResult, this, &PDFEngine::findAllResult);
		connect(currentFindAllWorker, &FindAllWorker::finished, currentFindAllThread, &QThread::quit);
		connect(currentFindAllWorker, &FindAllWorker::finished, currentFindAllWorker, &FindAllWorker::deleteLater);
		connect(currentFindAllThread, &QThread::finished, currentFindAllThread, &QObject::deleteLater);

		currentFindAllThread->start();
		return true;
	}
	else
		return false;
}

void PDFEngine::updateParentWindow(QWidget* parent) 
{ 
	this->parentWindow = parent; 
}

bool PDFEngine::refreshEngine()
{
	//Should return file name if available
	std::string tmp = checkFileAvailable(fileName);

	if (tmp == "")
		return false;

	doc = Poppler::Document::load(QString::fromStdString(this->fileName));

	doc->setRenderHint(Poppler::Document::Antialiasing, true);
	doc->setRenderHint(Poppler::Document::TextAntialiasing, true);

	return true;
}

void PDFEngine::rerenderAllPages()
{
	rerender = true;
}

void PDFEngine::cancelFindAllWorker()
{
	//If the user closes the find all dialog or starts another search
	if (currentFindAllWorker)
		currentFindAllWorker->cancel();

	currentFindAllWorker = NULL;
	currentFindAllThread = NULL;
}

void PDFEngine::findAllResult(SearchResult result)
{
	emit sendFindAllResult(result);
}

void PDFEngine::goToPhrase(int page, QRectF rect)
{
	//Go to specific phrase the user selects regardless of whatever tab is open in the viewer
	setCurrentPage(page);
	selectedRect = rect;
	foundPageNum = page;

	emit attentionNeeded();
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

void PDFEngine::recursivelyFillModel(QVector<Poppler::OutlineItem> currentItem, QStandardItem* rootItem, NavigationBar* navBar)
{
	//Add all the children recursively to the model, in addition saving the page number in a tuple with the item pointer
	for (int i = 0; i < currentItem.size(); i++) {
		Poppler::OutlineItem newItem = currentItem.at(i);
		QStandardItem *newModelItem = new QStandardItem(newItem.name());
		newModelItem->setEditable(false);
		rootItem->appendRow(newModelItem);

		NavTuple nTuple;
		//Add either internal destination (page number) or external destination (web hyperlink)
		if (newItem.destination() && !newItem.destination().isNull()) {
			nTuple.pageNum = newItem.destination()->pageNumber();
			nTuple.url = "";
		}
		else if (newItem.uri() != "") {
			nTuple.pageNum = -1;
			nTuple.url = newItem.uri();
		}
		nTuple.sItem = newModelItem;
		navBar->navItems.push_back(nTuple);

		if (newItem.hasChildren())
			recursivelyFillModel(newItem.children(), newModelItem, navBar);
	}
}

void PDFEngine::updateHeightValues(bool total)
{
	//Either calculate the total document height with spacing or get all the page heights
	for (int i = 0; i < getTotalNumberOfPages(); i++) {
		Poppler::Page* page = doc->page(i);
		QSizeF size = page->pageSizeF();
		QRectF pt(0.0, 0.0, size.width(), size.height());

		//If page is rotated 90 or 270, use width instead of height
		int h = int((((pdfRotation == Poppler::Page::Rotate0 || pdfRotation == Poppler::Page::Rotate180) ? pt.height() : pt.width()) / 72.0f) * (72.0f * scaleValue / 75.0f));

		if (total)
			documentHeight += h + 20;
		else
			allPageHeights.push_back(h);

		delete page;
	}
}

bool PDFEngine::documentSearch(Poppler::Page* page, int pageNum, std::string phrase, QRectF* foundRect, Poppler::Page::SearchDirection direction, Poppler::Page::Rotation rotation)
{
	//Index is saved throughout runs
	static int vectorIndex = -1;
	
	bool result = false;

	//Returns list of all results found for the page
	QList<QRectF> pageResults = page->search(QString::fromStdString(phrase), Poppler::Page::IgnoreCase, rotation);
	
	//If we are on a new page or the index fell outside of the bounds reset it back to default
	if (pageNum != searchPos || vectorIndex < -1)
		vectorIndex = -1;

	//Increment index depending on direction
	vectorIndex += direction == Poppler::Page::PreviousResult ? -1 : 1;

	//Return the result at the current index
	if ((direction == Poppler::Page::PreviousResult && vectorIndex >= 0) || (direction == Poppler::Page::NextResult && vectorIndex < pageResults.length())) {
		result = true;
		*foundRect = pageResults.at(vectorIndex);
		searchPos = pageNum;
	}
	
	return result;
}

void PDFEngine::addHyperlinksToPage(Page* page, Poppler::Page* popplerPage, QImage image)
{
	QList<Poppler::Link*> links = popplerPage->links();

	//Add all necessary web hyperlink objects
	for (int i = 0; i < links.length(); i++) {
		if (links.at(i)->linkType() == Poppler::Link::Browse) {
			double linkScale = 72 * scaleValue / 75.0;
			Poppler::LinkBrowse* link = static_cast<Poppler::LinkBrowse*>(links.at(i));
			double x, y, width, height = link->linkArea().height() * -1;
			switch (pdfRotation) {
			case Poppler::Page::Rotate0:
			default:
				x = link->linkArea().x() * image.width();
				y = (link->linkArea().y() + link->linkArea().height()) * image.height();
				width = link->linkArea().width() * image.width();
				height = height * image.height();
				break;
			case Poppler::Page::Rotate90:
				x = (1.0 - link->linkArea().y()) * image.width();
				y = (link->linkArea().x()) * image.height();
				width = height * image.width();
				height = link->linkArea().width() * image.height();
				break;
			case Poppler::Page::Rotate180:
				x = (1.0 - link->linkArea().x() - link->linkArea().width()) * image.width();
				y = (1.0 - link->linkArea().y()) * image.height();
				width = link->linkArea().width() * image.width();
				height = height * image.height();
				break;
			case Poppler::Page::Rotate270:
				x = (link->linkArea().y() - height) * image.width();
				y = (1.0 - link->linkArea().x() - link->linkArea().width()) * image.height();
				width = height * image.width();
				height = link->linkArea().width() * image.height();
				break;
			}
			outputLabel->addHyperlink(new HyperlinkObject(outputLabel, QRectF(x, y, width, height), link->url()));
		}
	}
}

void PDFEngine::unlockDocument()
{
	while (doc->isLocked()) {
		PasswordBoxDialog* dialog = new PasswordBoxDialog();
		
		//If user rejects dialog, document will fail to load
		if (dialog->exec() == QDialog::Rejected) {
			failedToLoad();
			return;
		}

		//Attempt unlock, output error message if incorrect password was entered
		if (doc->unlock(dialog->getPassword().toLatin1(), dialog->getPassword().toLatin1()) != 0)
			QMessageBox::warning(NULL, "Incorrect Password", "Incorrect password!\nPlease try again.");

		delete dialog;
	}
}

void PDFEngine::failedToLoad(){	success = false;}

std::string PDFEngine::checkFileAvailable(std::string fileName)
{
	//Check if file exists
	QFileInfo checkFile(QString::fromStdString(fileName));

	if (!(checkFile.exists() && checkFile.isFile())) {
		//Loading was unsuccessful
		failedToLoad();

		//Display error msgbox
		QMessageBox msgBox(parentWindow);
		msgBox.setIcon(QMessageBox::Critical);
		msgBox.setWindowTitle(tr("File Not Found"));
		msgBox.setText(tr(("The PDF file " + fileName + " could not be found.").c_str()));
		msgBox.setInformativeText(tr("The file may have been moved, renamed, or deleted."));
		QPushButton* closeTabBtn = msgBox.addButton("Close tab", QMessageBox::RejectRole);
		QPushButton* locateFileBtn = msgBox.addButton("Locate file...", QMessageBox::AcceptRole);
		
		//Reload the file if accepted or get the viewer to close the tab
		Viewer* vwr = reinterpret_cast<Viewer*>(parentWindow);

		if (msgBox.exec() == QMessageBox::Accepted)
			vwr->reloadFile();
		else
			vwr->reloadFile(false);
	
		return {};
	}
	else
		return fileName;
}

Poppler::Page* PDFEngine::reloadDocAndPage()
{
	//Reload the current page
	if (checkFileAvailable(fileName) == "")
		return NULL;
	delete doc;
	doc = Poppler::Document::load(QString::fromStdString(this->fileName));

	doc->setRenderHint(Poppler::Document::Antialiasing, true);
	doc->setRenderHint(Poppler::Document::TextAntialiasing, true);

	return doc->page(currentPage - 1);
}

