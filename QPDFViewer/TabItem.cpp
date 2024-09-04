#include "TabItem.h"
#include <qlayout.h>

TabItem::TabItem(QString title)
{
	engine = NULL;
	filePath = "";
	this->title = title;

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

QString TabItem::getTitle()
{
	return title;
}

QScrollArea* TabItem::getScrollArea()
{
	return scrollArea;
}

void TabItem::setPDFEngine(std::string fileName, QWidget* parentWindow)
{
	engine = new PDFEngine(fileName, parentWindow);
}

void TabItem::setTitle(QString title)
{
	this->title = title;
}

void TabItem::setFilePath(QString filePath)
{
	this->filePath = filePath;
}

void TabItem::updateScrollArea()
{
	scrollArea->setWidget(engine->returnImage());
}

QString TabItem::getFileName()
{
	return QString();
}

