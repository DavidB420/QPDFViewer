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

	loadDocument();

	cancelled = false;
	foundPageNum = 0;
	reloadResult = 0;
}

void PageRendererWorker::cancel() { cancelled = true; }

void PageRendererWorker::loadDocument()
{
	doc = Poppler::Document::load(fileName);
	
	if (hasPassword)
		doc->unlock(password.toLatin1(), password.toLatin1());
	
	doc->setRenderHint(Poppler::Document::Antialiasing, true);
	doc->setRenderHint(Poppler::Document::TextAntialiasing, true);
}

void PageRendererWorker::run()
{
	//Create poppler image based on current page, rotation and scale values
	Poppler::Page* page = doc->page(pageNum - 1);

	//Reload if page is unavailable
	if (page == NULL) {
		emit finished(-1, QImage());
		return;
	}

	//Convert the poppler into QImage
	QImage image = page->renderToImage((float)72 * scale / 75, (float)72 * scale / 75,
		-1, -1, -1, -1, rotation);

	//Reload page if image is null
	if (image.isNull()) {
		emit finished(-1, QImage());
		return;
	}

	//Delete poppler page and documentr
	delete page;
	delete doc;

	if (!cancelled)
		emit finished(pageNum, image);
}
