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
#include "PDFEngine.h"
#include <poppler-qt5.h>
#include <qthread.h>

PageRendererWorker::PageRendererWorker(PageRenderTask renderTask)
{
	pageNum = renderTask.pageNum;
	scale = renderTask.scale;
	rotation = renderTask.rotation;
	fileName = renderTask.fileName;
	password = renderTask.password;
	hasPassword = renderTask.hasPassword;
	selectedRect = renderTask.selectedRect;

	cancelled = false;
	foundPageNum = 0;
	reloadResult = 0;
}

void PageRendererWorker::cancel() { cancelled = true; }

void PageRendererWorker::run()
{
	QElapsedTimer timer;
	timer.start();

	QImage image = PDFEngine::returnImage(fileName, password, hasPassword, pageNum, scale, rotation, &PageRendererWorker::check1Static, &PageRendererWorker::check1Static, this);

	if (!cancelled)
		emit finished(pageNum, image,timer.elapsed());
}

void PageRendererWorker::check1() 
{
	emit finished(-1, QImage(), -1);
	return;
}

void PageRendererWorker::check1Static(void* ctx) {
	auto* self = static_cast<PageRendererWorker*>(ctx);
	self->check1(); 
}