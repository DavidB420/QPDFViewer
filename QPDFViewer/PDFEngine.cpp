#include "PDFEngine.h"

#include <qimage.h>
#include <qlabel.h>
#include <qwidget.h>
#include <string>

PDFEngine::PDFEngine(std::string fileName, QWidget *parentWindow)
{
	this->parentWindow = parentWindow;
	
	doc = poppler::document::load_from_file(fileName);

	outputLabel = NULL;

	currentPage = 1;
	scaleValue = 100;
}

QLabel *PDFEngine::returnImage()
{
	poppler::page* testPage = doc->create_page(currentPage-1);
	poppler::page_renderer pr;
	poppler::image img = pr.render_page(testPage,
		(float)72 * scaleValue / 75, (float)72 * scaleValue / 75,
		-1, -1, -1, -1, poppler::rotate_0);

	QImage image((uchar*)img.data(), img.width(), img.height(), img.bytes_per_row(), QImage::Format_ARGB32);
	
	if (outputLabel != NULL)
		delete outputLabel;

	outputLabel = new QLabel(this->parentWindow);
	outputLabel->setPixmap(QPixmap::fromImage(image));
	outputLabel->move(100, 100);
	outputLabel->resize(img.width(), img.height());
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
