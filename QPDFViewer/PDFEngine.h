#ifndef PDFENGINE_H
#define PDFENGINE_H

#include <memory>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-toc.h>
#include <string>
#include <qtreeview.h>
#include <qstandarditemmodel.h>
#include "Page.h"
#include "NavigationBar.h"

class PDFEngine
{
public:
	PDFEngine(std::string fileName,QWidget *parentWindow);
	Page* returnImage();
	int getTotalNumberOfPages();
	int getCurrentPage();
	bool setCurrentPage(int page);
	bool setCurrentScale(int scale);
	bool findPhraseInDocument(std::string phrase, poppler::page::search_direction_enum direction);
	void displayTextBox(QRectF dim);
	void displayAllText();
	void addNavOutline(NavigationBar* tView);
	void rotatePDF(bool plus90);
private:
	QWidget *parentWindow;
	Page* outputLabel;
	poppler::document* doc;
	poppler::rectf selectedRect;
	poppler::rectf foundRect;
	poppler::rotation_enum pdfRotation;
	int currentPage;
	int scaleValue;
	void recursivelyFillModel(poppler::toc_item* currentItem, QStandardItem* rootItem, NavigationBar *navBar);
};

#endif

