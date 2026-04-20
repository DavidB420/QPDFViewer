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

#ifndef PAGERENDERERWORKER_H
#define PAGERENDERERWORKER_H

#include <qobject.h>
#include <qwidget.h>
#include <poppler-qt5.h>

struct PageRenderTask {
    int pageNum;
    int foundPageNum;
    int scale;
    Poppler::Page::Rotation rotation;
    QString fileName;
    QString password;
    bool hasPassword;
    QRectF selectedRect;
};

class PageRendererWorker: public QObject
{
    Q_OBJECT
public:
    PageRendererWorker(PageRenderTask renderTask);
    void cancel();
public slots:
    void run();
private:
    int pageNum, scale;
    Poppler::Page::Rotation rotation;
    QString fileName;
    QString password;
    bool hasPassword, cancelled;
    Poppler::Document* doc;
    QRectF selectedRect;
    int foundPageNum;
    volatile int reloadResult;
    void check1();
    static void check1Static(void *ctx);
signals:
    void finished(int pageNum, QImage renderedImg);
    void needReloadCheck(PageRendererWorker *worker);
};

#endif
