/**
 * Copyright 2024 David Badiei
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

bool TabItem::getUseNavBar()
{
	return useNavBar;
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

