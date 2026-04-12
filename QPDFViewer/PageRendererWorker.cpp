/**
 * Copyright 2026 David Badiei
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

#include "PageRendererWorker.h"
#include <poppler-qt5.h>

PageRendererWorker::PageRendererWorker(PageRenderTask renderTask)
{
	pageNum = renderTask.pageNum;
	scale = renderTask.scale;
	rotation = renderTask.rotation;
	fileName = renderTask.fileName;
	password = renderTask.password;
	hasPassword = renderTask.hasPassword;
	selectedRect = renderTask.selectedRect;

	doc = Poppler::Document::load(fileName);

	if (hasPassword)
		doc->unlock(password.toLatin1(), password.toLatin1());

	cancelled = false;
	foundPageNum = 0;
}

void PageRendererWorker::cancel() { cancelled = true; }

void PageRendererWorker::run()
{
	//Create poppler image based on current page, rotation and scale values
	Poppler::Page* page = doc->page(pageNum - 1);

	doc->setRenderHint(Poppler::Document::Antialiasing, true);
	doc->setRenderHint(Poppler::Document::TextAntialiasing, true);

	//Reload if page is unavailable
	/*if (page == NULL) {
		page = reloadDocAndPage();
		if (page == NULL)
			return NULL;
	}*/

	//Convert the poppler into QImage
	QImage image = page->renderToImage((float)72 * scale / 75, (float)72 * scale / 75,
		-1, -1, -1, -1, rotation);

	//Reload page if image is null
	/*if (image.isNull()) {
		page = reloadDocAndPage();
		image = page->renderToImage((float)72 * scaleValue / 75, (float)72 * scaleValue / 75,
			-1, -1, -1, -1, pdfRotation);
		if (image.isNull())
			return NULL;
	}*/

	//If there has been a selection from searching in the past, be sure to show it
	/*if (selectedRect.x() != 0 && selectedRect.y() != 0 && selectedRect.width() != 0 && selectedRect.height() != 0 && getCurrentPage() == foundPageNum) {
		QRectF rect(selectedRect.x() * scaleValue / 75, selectedRect.y() * scaleValue / 75, selectedRect.width() * scaleValue / 75, selectedRect.height() * scaleValue / 75);
		outputLabel->drawSelection(rect);
		selectedRect = QRectF(0, 0, 0, 0);
		foundPageNum = -1;
	}*/

	//Delete poppler page and document
	delete page;
	delete doc;

	if (!cancelled)
		emit finished(pageNum, image);
}
