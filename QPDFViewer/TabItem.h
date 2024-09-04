#ifndef TABITEM_H
#define TABITEM_H

#include <qwidget.h>
#include <qscrollarea.h>
#include "PDFEngine.h"

class TabItem : public QWidget
{
public:
	TabItem(QString title);
	PDFEngine* getEngine();
	QString getFilePath();
	QString getTitle();
	QScrollArea* getScrollArea();
	void setPDFEngine(std::string fileName, QWidget* parentWindow);
	void setTitle(QString title);
	void setFilePath(QString filePath);
	void updateScrollArea();
	QString getFileName();
private:
	PDFEngine* engine;
	QString title;
	QString filePath;
	QScrollArea *scrollArea;
};

#endif
