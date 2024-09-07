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
