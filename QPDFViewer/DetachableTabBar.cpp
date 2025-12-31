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

#include "DetachableTabBar.h"

#include "Viewer.h"

#include <qapplication.h>
#include <qevent.h>
#include <qdrag.h>
#include <qmimedata.h>

DetachableTabBar::DetachableTabBar(QWidget* parent)
{
	this->setMovable(true);
	this->setTabsClosable(true);
	this->setAcceptDrops(true);
	setAttribute(Qt::WA_AcceptDrops, true);

	this->setParent(parent);

	viewerPtr = parent;

	detachIndex = -1;

	TAB_MIME = "application/x-detachable-tab";
}

QString DetachableTabBar::getTabMime()
{
	return TAB_MIME;
}

void DetachableTabBar::mousePressEvent(QMouseEvent* event)
{
	detachIndex = tabAt(event->pos());

	if (detachIndex == count() - 1)
		detachIndex = -1;

	detachStartPos = event->pos();

	QTabBar::mousePressEvent(event);
}

void DetachableTabBar::mouseMoveEvent(QMouseEvent* event)
{
	if ((event->pos() - detachStartPos).manhattanLength() > QApplication::startDragDistance()) {
		if (!rect().contains(event->pos()) && detachIndex >= 0) {  // Remove the rect().contains() check
			QDrag* drag = new QDrag(this);
			QMimeData* mimeData = new QMimeData;
			QByteArray data;
			QDataStream out(&data, QIODevice::WriteOnly);
			out << quintptr(viewerPtr);
			out << detachIndex;
			mimeData->setData(TAB_MIME, data);
			drag->setMimeData(mimeData);
			Qt::DropAction result = drag->exec(Qt::MoveAction);

			if (result == Qt::IgnoreAction)
				emit detachTab(detachIndex, event->globalPos());  // Use globalPos for consistency
			detachIndex = -1;
			return;
		}
	}
	QTabBar::mouseMoveEvent(event);
}

void DetachableTabBar::mouseReleaseEvent(QMouseEvent* event)
{
	detachIndex = -1;
	QTabBar::mouseReleaseEvent(event);
}

void DetachableTabBar::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat(TAB_MIME)) {
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}

void DetachableTabBar::dropEvent(QDropEvent* event)
{
	QByteArray data = event->mimeData()->data(TAB_MIME);
	QDataStream in(&data, QIODevice::ReadOnly);

	quintptr srcPtr;
	int index;
	in >> srcPtr >> index;

	QObject* srcViewer = reinterpret_cast<QObject*>(srcPtr);

	if (srcViewer != viewerPtr)
		emit tabMerged(index, srcViewer);

	event->acceptProposedAction();
	event->setDropAction(Qt::MoveAction);
}

void DetachableTabBar::dragMoveEvent(QDragMoveEvent* event)
{
	if (event->mimeData()->hasFormat(TAB_MIME)){
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}
