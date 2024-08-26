#ifndef PDFENGINE_H
#define PDFENGINE_H

#include <memory>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <string>
#include <qlabel.h>

class PDFEngine
{
public:
	PDFEngine(std::string fileName,QWidget *parentWindow);
	QLabel* returnImage();
	int getTotalNumberOfPages();
	int getCurrentPage();
	bool setCurrentPage(int page);
	bool setCurrentScale(int scale);
private:
	QWidget *parentWindow;
	QLabel* outputLabel;
	poppler::document* doc;
	int currentPage;
	int scaleValue;
};

#endif

