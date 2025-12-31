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
	//Set default values
	this->setMovable(true);
	this->setTabsClosable(true);
	this->setAcceptDrops(true);
	setAttribute(Qt::WA_AcceptDrops, true);

	this->setParent(parent);

	viewerPtr = parent;

	detachIndex = -1;

	TAB_MIME = "qpdfviewer/x-detachable-tab";
}

QString DetachableTabBar::getTabMime()
{
	return TAB_MIME;
}

void DetachableTabBar::mousePressEvent(QMouseEvent* event)
{
	//Save detach index (except for the plus button)
	detachIndex = tabAt(event->pos());

	if (detachIndex == count() - 1)
		detachIndex = -1;

	detachStartPos = event->pos();

	QTabBar::mousePressEvent(event);
}

void DetachableTabBar::mouseMoveEvent(QMouseEvent* event)
{
	//Check if user started dragging a tab outside the the tab bar
	if ((event->pos() - detachStartPos).manhattanLength() > QApplication::startDragDistance()) {
		if (!rect().contains(event->pos()) && detachIndex >= 0) {

			//Start a drag operation, saving both the pointer of its viewer and the index of the tab
			QDrag* drag = new QDrag(this);
			QMimeData* mimeData = new QMimeData;
			QByteArray data;
			QDataStream out(&data, QIODevice::WriteOnly);
			out << quintptr(viewerPtr);
			out << detachIndex;
			mimeData->setData(TAB_MIME, data);
			drag->setMimeData(mimeData);
			Qt::DropAction result = drag->exec(Qt::MoveAction);

			//If the user doesnt drag it to another window, create a new one
			if (result == Qt::IgnoreAction)
				emit detachTab(detachIndex, event->globalPos());
			detachIndex = -1;
			return;
		}
	}
	QTabBar::mouseMoveEvent(event);
}

void DetachableTabBar::mouseReleaseEvent(QMouseEvent* event)
{
	//If mouse is released, reset the index
	detachIndex = -1;
	QTabBar::mouseReleaseEvent(event);
}

void DetachableTabBar::dragEnterEvent(QDragEnterEvent* event)
{
	//Check if mime matches before accepting
	if (event->mimeData()->hasFormat(TAB_MIME)) {
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}

void DetachableTabBar::dropEvent(QDropEvent* event)
{
	//Read data returned from drop
	QByteArray data = event->mimeData()->data(TAB_MIME);
	QDataStream in(&data, QIODevice::ReadOnly);

	quintptr srcPtr;
	int index;
	in >> srcPtr >> index;

	QObject* srcViewer = reinterpret_cast<QObject*>(srcPtr);

	//If this tab came from another window, merge it
	if (srcViewer != viewerPtr)
		emit tabMerged(index, srcViewer);

	//Drop was successful
	event->acceptProposedAction();
	event->setDropAction(Qt::MoveAction);
}

void DetachableTabBar::dragMoveEvent(QDragMoveEvent* event)
{
	//Check if mime matches
	if (event->mimeData()->hasFormat(TAB_MIME)){
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}

void DetachableTabBar::tabLayoutChange()
{
	int plusIndex = count() - 1;

	//If Qt moved a real tab past the plus tab, snap it back
	for (int i = 0; i < count() - 1; ++i) {
		if (tabText(i) == "+") {
			moveTab(i, plusIndex);
			break;
		}
	}

	QTabBar::tabLayoutChange();
}