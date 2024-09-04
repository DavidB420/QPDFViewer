#include "TabItem.h"
#include <qlayout.h>

TabItem::TabItem()
{
	engine = NULL;
	filePath = "";

	scrollArea = new QScrollArea(this);
	scrollArea->setBackgroundRole(QPalette::Mid);
	scrollArea->setAlignment(Qt::AlignCenter);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(scrollArea);
	layout->setContentsMargins(0, 0, 0, 0);

	setLayout(layout);
}

PDFEngine* TabItem::getEngine()
{
	return engine;
}

QString TabItem::getFilePath()
{
	return filePath;
}

QScrollArea* TabItem::getScrollArea()
{
	return scrollArea;
}

void TabItem::setPDFEngine(std::string fileName, QWidget* parentWindow)
{
	engine = new PDFEngine(fileName, parentWindow);
}

void TabItem::setFilePath(QString filePath)
{
	this->filePath = filePath;
}

void TabItem::updateScrollArea()
{
	scrollArea->setWidget(engine->returnImage());
}

std::string TabItem::getFileName()
{
	int startFN = 0;

	std::string filePathStd = filePath.toStdString();

	for (int i = filePathStd.length() - 1; i >= 0; i--) {
		if (filePathStd.at(i) == '\\' || filePathStd.at(i) == '/') {
			startFN = i+1;
			break;
		}
	}

	return filePathStd.substr(startFN,filePathStd.length());
}

