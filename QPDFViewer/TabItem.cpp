/**
 * Copyright 2025 David Badiei
 *
 * This file is part of QPDFViewer, hereafter referred to as the program.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "TabItem.h"
#include <qlayout.h>

TabItem::TabItem()
{
	//Stores scroll area, pdf engine, file path and other tab dependent stuff
	engine = NULL;
	filePath = "";
	useNavBar = false;
	splitterData = { 0 };

	scrollArea = new TabScrollArea(this);
	scrollArea->setBackgroundRole(QPalette::Mid);
	scrollArea->viewport()->setBackgroundRole(QPalette::Mid);
	//scrollArea->setAlignment(Qt::AlignCenter);

	QHBoxLayout* layout = new QHBoxLayout;
	layout->addWidget(scrollArea);
	layout->setContentsMargins(0, 0, 0, 0);

	/*QWidget* scrollContent = new QWidget(scrollArea->viewport());
	scrollLayout = new QVBoxLayout(scrollContent);
	scrollLayout->setAlignment(Qt::AlignCenter);*/

	/*scrollArea->setWidgetResizable(true);
	scrollArea->setWidget(scrollContent);*/

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

TabScrollArea* TabItem::getScrollArea()
{
	return scrollArea;
}

bool TabItem::getUseNavBar()
{
	return useNavBar;
}

QByteArray TabItem::getSplitterData()
{
	return splitterData;
}

void TabItem::setSplitterData(QByteArray data)
{
	splitterData = data;
}

void TabItem::setPDFEngine(std::string fileName, QWidget* parentWindow)
{
	engine = new PDFEngine(fileName, parentWindow);
	scrollArea->setDocumentHeight(engine->getDocumentHeight());
	scrollArea->setPageHeights(engine->getPageHeights());
}

void TabItem::setFilePath(QString filePath)
{
	this->filePath = filePath;
}

void TabItem::updateScrollArea()
{
	QVector <Page*> pagesVector = engine->getVisiblePages();
	scrollArea->updateScrollArea(&pagesVector);

	//scrollArea->setWidget(engine->returnImage());
}

void TabItem::setUseNavBar(bool enabled)
{
	useNavBar = enabled;
}

std::string TabItem::getFileName()
{
	//Remove file path and just get the file name
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

void TabItem::rerenderUpdateScrollArea()
{
	QVector <Page*> pagesVector = engine->getVisiblePages();
	scrollArea->setCurrentPages(&pagesVector);
	scrollArea->setBufferLock(0);
}

