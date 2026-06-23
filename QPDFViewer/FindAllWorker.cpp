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

QList <SearchResult> FindAllWorker::wordBoxSearch(QList<Poppler::TextBox*> words, int direction, int pageNum, QString qPhrase, QString highlightHTMLHeader, QString highlightHTMLFooter, bool (*functioning)(void*), void (*emitter)(void*,SearchResult), void* ctx)
{	
	QList<SearchResult> results;

	//Check if there any actual results before continuing with the rest of search
	if (!words.isEmpty()) {
		int headerSize = highlightHTMLHeader.length();
		int footerSize = highlightHTMLFooter.length();
		int qPhraseLength = qPhrase.length();
		//Save page text for snippet
		QString text = "";
		QList<int> wordLengthLookup;
		int pos = 0;
		for (int j = 0; j < words.length(); j++) {
			QString wordStr = words.at(j)->text();
			for (int k = 0; k < wordStr.length(); k++) {
				text.append(wordStr.at(k));
			}
			wordLengthLookup.append(pos);
			pos += wordStr.length();
			if (words.at(j)->hasSpaceAfter()) {
				text.append(' ');
				pos++;
			}
			else if (j + 1 < words.length()) {
				//Check if next word is on a different line by comparing Y position
				if (!qFuzzyCompare(words.at(j)->boundingBox().y(), words.at(j + 1)->boundingBox().y())) {
					text.append(' ');
					pos++;
				}
			}
		}
		QString textTmp = text;

		//Get all results in the page
		int index = direction == 2 ? text.length() - 1 : 0, j = direction == 2 ? words.length() - 1 : 0;
		while ((direction == 2 && index >= 0 && (index = text.lastIndexOf(qPhrase, index, Qt::CaseInsensitive)) != -1) || (direction != 2 && index < text.length() && (index = text.indexOf(qPhrase, index, Qt::CaseInsensitive)) != -1)) {
			//Get new copy of text and save rect and page number in result
			textTmp = text;
			SearchResult newResult;
			newResult.page = pageNum;
			QRectF currentLineRect = QRectF(0, 0, 0, 0);
			//Create found rect, use multiple words if necessary
			for (int k = j, rectIndex = index, m = qPhrase.count(' '); direction == 2 ? k >= 0 : k < words.length(); direction == 2 ? k-- : k++) {
				int wordStart = wordLengthLookup.at(k);
				int wordEnd = wordStart + words.at(k)->text().length();
				if (rectIndex >= wordStart && rectIndex < wordEnd) {
					if (currentLineRect == QRectF(0, 0, 0, 0)) currentLineRect = words.at(k)->charBoundingBox(rectIndex - wordStart);
					for (int l = (rectIndex - wordStart) + 1; l < (rectIndex - wordStart) + qPhrase.length(); l++)
						currentLineRect = currentLineRect.united(words.at(k)->charBoundingBox(l));
					j = k;
					m += words.at(k)->text().length();
					if (m < qPhrase.length()) {
						rectIndex += words.at(k)->text().length() + 1;
						if (!qFuzzyCompare(words.at(k)->boundingBox().y(), words.at(k + 1)->boundingBox().y())) {
							newResult.foundRect.append(currentLineRect);
							currentLineRect = QRectF(0, 0, 0, 0);
						}
					}
				}
			}
			newResult.foundRect.append(currentLineRect);

			newResult.done = functioning != NULL ? functioning(ctx) : true;

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

			//Finished one result, send it to engine so it can send it to dialog box
			if (emitter != NULL)
				emitter(ctx, newResult);
			else
				results.append(newResult);
		}
	}

	return results;
}

void FindAllWorker::emitterItem(SearchResult newResult)
{
	emit finishedResult(newResult);
}

void FindAllWorker::emitterItemStatic(void* ctx, SearchResult newResult) {
	auto* self = static_cast<FindAllWorker*>(ctx);
	return self->emitterItem(newResult);
}

bool FindAllWorker::isCancelled()
{
	return cancelled;
}

bool FindAllWorker::isCancelledStatic(void* ctx) {
	auto* self = static_cast<FindAllWorker*>(ctx);
	return self->isCancelled();
}

void FindAllWorker::cancel()
{
	this->blockSignals(true);
	cancelled = true;
}

void FindAllWorker::run()
{
	if (phrase.isEmpty()) {
		emit finished();
		return;
	}
	
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

		QList<Poppler::TextBox*> words = page->textList(pdfRotation);

		FindAllWorker::wordBoxSearch(words, direction, i, qPhrase, highlightHTMLHeader, highlightHTMLFooter, &FindAllWorker::isCancelledStatic, &FindAllWorker::emitterItemStatic, this);
		
		qDeleteAll(words);
		delete page;
	}

	emit finished();
}
