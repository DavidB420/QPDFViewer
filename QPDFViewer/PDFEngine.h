#ifndef PDFENGINE_H
#define PDFENGINE_H

#include <memory>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <string>
#include "Page.h"

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
private:
	QWidget *parentWindow;
	Page* outputLabel;
	poppler::document* doc;
	poppler::rectf selectedRect;
	poppler::rectf foundRect;
	int currentPage;
	int scaleValue;
};

#endif

