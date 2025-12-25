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

#ifndef PDFENGINE_H
#define PDFENGINE_H

#include <memory>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <poppler/cpp/poppler-toc.h>
#include <string>
#include <qtreeview.h>
#include <qstandarditemmodel.h>
#include "Page.h"
#include "NavigationBar.h"

class PDFEngine
{
public:
	PDFEngine(std::string fileName,QWidget *parentWindow);
	Page* returnImage();
	int getTotalNumberOfPages();
	int getCurrentPage();
	int getScaleValue();
	bool setCurrentPage(int page);
	bool setCurrentScale(int scale);
	bool findPhraseInDocument(std::string phrase, poppler::page::search_direction_enum direction);
	void displayTextBox(QRectF dim);
	void displayAllText();
	void addNavOutline(NavigationBar* tView);
	void rotatePDF(bool plus90);
	unsigned long getDocumentHeight();
	QVector <Page*> getVisiblePages();
	QVector <int> getPageHeights();
	poppler::rotation_enum getCurrentRotation();
private:
	QWidget *parentWindow;
	Page* outputLabel;
	poppler::document* doc;
	poppler::rectf selectedRect;
	poppler::rectf foundRect;
	poppler::rotation_enum pdfRotation;
	int currentPage;
	int scaleValue;
	unsigned long documentHeight;
	void recursivelyFillModel(poppler::toc_item* currentItem, QStandardItem* rootItem, NavigationBar *navBar);
	QVector <Page*> previousPages;
	QVector <int> allPageHeights;
	void updateHeightValues(bool total);

};

#endif

