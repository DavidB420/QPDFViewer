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

#ifndef FINDALLWORKER_H
#define FINDALLWORKER_H

#include <qobject.h>
#include <poppler-qt5.h>

struct SearchResult {
	QString snippet;
	QRectF foundRect;
	int page;
	bool done;
};

Q_DECLARE_METATYPE(SearchResult)

class FindAllWorker: public QObject
{
	Q_OBJECT
public:
	FindAllWorker(QString fn="", QString phrase="", QString password="", bool hasPassword=false, int currentPage = 0, int totalNumberOfPages = 0, int direction = 0, Poppler::Page::Rotation pdfRotation = Poppler::Page::Rotate0);
	~FindAllWorker();
	bool isDone();
private:
	QString fn;
	QString phrase;
	int currentPage;
	int totalNumberOfPages;
	int direction;
	bool cancelled;
	bool done;
	Poppler::Document* doc;
	Poppler::Page::Rotation pdfRotation;
signals:
	void finishedResult(SearchResult result);
	void finished();
public slots:
	void run();
	void cancel();

};

#endif