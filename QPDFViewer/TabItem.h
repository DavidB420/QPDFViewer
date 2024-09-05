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
	bool getUseNavBar();
	void setPDFEngine(std::string fileName, QWidget* parentWindow);
	void setFilePath(QString filePath);
	void updateScrollArea();
	void setUseNavBar(bool enabled);
	std::string getFileName();
private:
	PDFEngine* engine;
	QString title;
	QString filePath;
	QScrollArea *scrollArea;
	bool useNavBar;
};

#endif
