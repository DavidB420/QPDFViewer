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

#ifndef PDFENGINE_H
#define PDFENGINE_H

#include <memory>
#include <poppler-qt5.h>
#include <string>
#include <qtreeview.h>
#include <qstandarditemmodel.h>
#include <qcache.h>
#include "Page.h"
#include "NavigationBar.h"
#include "FindAllWorker.h"
#include "PageRendererWorker.h"

struct PageRenderThread {
	QThread* renderThread;
	PageRendererWorker* worker;
};
struct PageCacheObject {
	QImage image;
	int scaleValue;
	Poppler::Page::Rotation pdfRotation;
};
class PDFEngine: public QObject
{
	Q_OBJECT
public:
	PDFEngine(std::string fileName,QWidget *parentWindow);
	~PDFEngine();
	static QImage returnImage(QString fileName, QString password, bool hasPassword, int pageNum, int scaleValue, Poppler::Page::Rotation pdfRotation, Poppler::Document* document=NULL, void (*check1)(void*)=NULL, void (*check2)(void*)=NULL,void*ctx=NULL);
	Page* returnDefaultImage(int h);
	QString getPassword();
	bool getHasPassword();
	int getTotalNumberOfPages();
	int getCurrentPage();
	int getScaleValue();
	bool setCurrentPage(int page);
	bool setCurrentPageSignal(int page);
	bool setCurrentScale(int scale);
	bool findPhraseInDocument(std::string phrase, Poppler::Page::SearchDirection direction);
	void displayTextBox(QRectF dim);
	void displayAllText();
	void addNavOutline(NavigationBar* tView);
	void rotatePDF(bool plus90);
	unsigned long getDocumentHeight();
	QVector <Page*> getVisiblePages();
	QVector <int> getPageHeights();
	Poppler::Page::Rotation getCurrentRotation();
	bool getSuccess();
	bool getAllSearchResults(int direction, std::string phrase);
	void updateParentWindow(QWidget* parent);
	bool refreshEngine();
	void rerenderAllPages();
	Poppler::Page* reloadDocAndPage();
	std::string checkFileAvailable(std::string fileName);
private:
	QWidget *parentWindow;
	Page* outputLabel;
	Poppler::Document* doc;
	QRectF selectedRect;
	QRectF foundRect;
	Poppler::Page::Rotation pdfRotation;
	int currentPage;
	int scaleValue;
	int foundPageNum;
	int searchPos;
	bool success;
	unsigned long documentHeight;
	void recursivelyFillModel(QVector<Poppler::OutlineItem> currentItem, QStandardItem* rootItem, NavigationBar *navBar);
	QVector <Page*> previousPages;
	QVector <int> allPageHeights;
	QVector <qint64> renderTimes;
	QCache<int, PageCacheObject> pageCache;
	QMap <int, struct PageRenderThread> renderThreadList;
	std::string fileName;
	FindAllWorker* currentFindAllWorker;
	QThread* currentFindAllThread;
	QString password;
	bool hasPassword;
	bool rerender;
	bool useMultithreading;
	void updateRenderTimeAvgs(qint64 elapsed);
	void updateHeightValues(bool total);
	bool documentSearch(Poppler::Page *page, int pageNum, std::string phrase, QRectF* foundRect, Poppler::Page::SearchDirection direction, Poppler::Page::Rotation rotation);
	void addHyperlinksToPage(Page* page, Poppler::Page* popplerPage, QImage image);
	void killThread(PageRenderThread thread);
	void unlockDocument();
	void failedToLoad();
signals:
	void pageChanged();
	void sendFindAllResult(SearchResult result);
	void attentionNeeded();
	void pageFinished();
public slots:
	void goToPhrase(int page, QRectF rect);
	void cancelFindAllWorker();
	void findAllResult(SearchResult result);
	void onPageRendered(int pageNum, QImage renderedImg, int elapsedTime);
};

#endif

