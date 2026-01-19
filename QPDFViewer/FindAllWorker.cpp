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

#include "FindAllWorker.h"

#include <qthread.h>

FindAllWorker::FindAllWorker(QString fn, QString phrase, QString password, bool hasPassword, int currentPage, int totalNumberOfPages, int direction, Poppler::Page::Rotation pdfRotation)
{
	//Save parameters and reload document seperately for the worker, as poppler is not thread safe
	this->fn = fn;
	this->phrase = phrase;
	this->currentPage = currentPage;
	this->totalNumberOfPages = totalNumberOfPages;
	this->direction = direction;
	this->pdfRotation = pdfRotation;

	cancelled = false;

	doc = Poppler::Document::load(fn);

	if (hasPassword)
		doc->unlock(password.toLatin1(), password.toLatin1());
}

FindAllWorker::~FindAllWorker()
{
	delete doc;
}

void FindAllWorker::cancel()
{
	this->blockSignals(true);
	cancelled = true;
}

void FindAllWorker::run()
{
	//Save start, stop, step of for loop
	int start = direction == 0 ? 1 : currentPage;
	int stop = direction == 2 ? 0 : totalNumberOfPages;
	int step = direction == 2 ? -1 : 1;

	//Make loop more optimized by precalculating these values outside of the loop
	QString highlightHTMLHeader = "< span style = \"color:red; font-weight:bold;\">";
	QString highlightHTMLFooter = "</span>";
	int headerSize = highlightHTMLHeader.size();
	int footerSize = highlightHTMLFooter.size();
	QString qPhrase = phrase;
	int qPhraseLength = qPhrase.length();

	//Run bidirectional capable loop until stop or cancelled
	for (int i = start; (step > 0 ? i <= stop : i > stop) && !cancelled; i += step) {
		if (QThread::currentThread()->isInterruptionRequested())
			break;
		Poppler::Page* page = doc->page(i-1);

		QList<QRectF> rects = page->search(qPhrase, Poppler::Page::IgnoreCase, pdfRotation);

		//Check if there any actual results before continuing with the rest of search
		if (!rects.isEmpty()) {
			//Save page text for snippet
			QString text = page->text(QRectF());
			QString textTmp = text;

			//Get all results in the page
			int index = direction == 2 ? text.length()-1 : 0, j = direction == 2 ? rects.length() - 1 : 0;
			while ((direction == 2 && (index = text.lastIndexOf(qPhrase, index, Qt::CaseInsensitive)) != -1) || (direction != 2 && (index = text.indexOf(qPhrase, index, Qt::CaseInsensitive)) != -1)) {	
				//Get new copy of text and save rect and page number in result
				textTmp = text;
				SearchResult newResult;
				newResult.page = i;
				newResult.foundRect = rects.at(j);
				newResult.done = cancelled;

				//Insert HTML header and footer around found phrase
				textTmp.insert(index, highlightHTMLHeader);
				index += highlightHTMLHeader.size();
				textTmp.insert(index + qPhraseLength, highlightHTMLFooter);

				//Snippet radius is 40 characters
				QString snippet = textTmp.mid(qMax(0, index - 40 - headerSize), qMin(textTmp.length(), index + qPhraseLength + footerSize + 40));
				newResult.snippet = snippet;
				
				//We had to skip the HTML header before
				index -= highlightHTMLHeader.size();

				index += direction == 2 ? -1 : qPhraseLength;
				j+=direction == 2 ? -1 : 1;

				//Finished one result, send it to engine so it can send it to dialog box
				emit finishedResult(newResult);
			}
		}

		delete page;
	}

	emit finished();
}
