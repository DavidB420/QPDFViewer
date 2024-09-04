#ifndef TABITEM_H
#define TABITEM_H

#include <qwidget.h>
#include <qscrollarea.h>
#include "PDFEngine.h"

class TabItem : public QWidget
{
public:
	TabItem();
	PDFEngine* getEngine();
	QString getFilePath();
	QScrollArea* getScrollArea();
	void setPDFEngine(std::string fileName, QWidget* parentWindow);
	void setFilePath(QString filePath);
	void updateScrollArea();
	std::string getFileName();
private:
	PDFEngine* engine;
	QString title;
	QString filePath;
	QScrollArea *scrollArea;
};

#endif
