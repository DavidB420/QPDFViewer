#include "PDFEngine.h"

#include <qimage.h>
#include <qlabel.h>
#include <qwidget.h>
#include <qpainter.h>
#include <string>
#include <poppler/cpp/poppler-global.h>

PDFEngine::PDFEngine(std::string fileName, QWidget *parentWindow)
{
	this->parentWindow = parentWindow;
	
	doc = poppler::document::load_from_file(fileName);

	outputLabel = NULL;

	currentPage = 1;
	scaleValue = 100;

	selectedRect = poppler::rectf(0, 0, 0, 0);
}

QLabel *PDFEngine::returnImage()
{
	poppler::page* page = doc->create_page(currentPage-1);
	poppler::page_renderer pr;
	poppler::image img = pr.render_page(page,
		(float)72 * scaleValue / 75, (float)72 * scaleValue / 75,
		-1, -1, -1, -1, poppler::rotate_0);

	QImage image((uchar*)img.data(), img.width(), img.height(), img.bytes_per_row(), QImage::Format_ARGB32);
	
	if (selectedRect.x() != 0 && selectedRect.y() != 0 && selectedRect.width() != 0 && selectedRect.height() != 0) {
		QPainter painter(&image);
		painter.setBrush(Qt::NoBrush);
		painter.setPen(Qt::red);
		painter.drawRect(selectedRect.x() * scaleValue / 75, selectedRect.y() * scaleValue / 75, selectedRect.width() * scaleValue / 75, selectedRect.height() * scaleValue / 75);
		selectedRect = poppler::rectf(0, 0, 0, 0);
	}
	
	if (outputLabel != NULL)
		delete outputLabel;

	outputLabel = new QLabel(this->parentWindow);
	outputLabel->setPixmap(QPixmap::fromImage(image));
	outputLabel->resize(img.width(), img.height());

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
	int currentSearch = currentPage;
	bool result = false;
	selectedRect = poppler::rectf(0, 0, 0, 0);

	while (currentSearch >= 1 && currentSearch <= getTotalNumberOfPages()) {
		poppler::page* page = doc->create_page(currentSearch - 1);

		if (currentPage != currentSearch)
			foundRect = page->page_rect();

		if (direction == poppler::page::search_previous_result && currentPage != currentSearch) {
			result = true;
			while (result)
				result = page->search(poppler::ustring::from_latin1(phrase), foundRect, poppler::page::search_next_result, poppler::case_insensitive, poppler::rotate_0);
		}
		
		result = page->search(poppler::ustring::from_latin1(phrase), foundRect, direction, poppler::case_insensitive, poppler::rotate_0);

		if (result) {
			selectedRect = poppler::rectf(foundRect.x(), foundRect.y(), foundRect.width(), foundRect.height());
			currentPage = currentSearch;
			currentSearch = -1;
		}

		if (direction == poppler::page::search_next_result)
			currentSearch++;
		else if (direction == poppler::page::search_previous_result)
			currentSearch--;

		delete page;
	}

	return result;
}