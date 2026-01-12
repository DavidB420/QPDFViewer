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

#include "DetachableTabBar.h"

#include "Viewer.h"

#include <qapplication.h>
#include <qevent.h>
#include <qdrag.h>
#include <qmimedata.h>
#include <QStylePainter>

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

	slideOffset = QPoint(0, 0);

	slideTimer.setInterval(16); //16 ms = 1/60s = 60 FPS
	connect(&slideTimer, &QTimer::timeout, this, &DetachableTabBar::updateSlideAnimOffset);
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
	//Manually move tab within tab bar
	if (rect().contains(event->pos())) {
		int target = tabAt(event->pos());
		if (target >= 0 && target != detachIndex && tabText(target) != "+") {
			//Capture old position
			QRect before = tabRect(detachIndex);
			//Move tab
			moveTab(detachIndex, target);
			detachIndex = target;
			//Capture new position
			QRect after = tabRect(detachIndex);
			//Create overlap offset
			slideOffset = before.topLeft() - after.topLeft();
			slideTimer.start();
		}

		event->accept();
		return;
	}
	
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
		}
	}
}

void DetachableTabBar::paintEvent(QPaintEvent* event)
{
	//Create painter for this object
	QStylePainter painter(this);

	//Paint each tab
	for (int i = 0; i < count(); ++i) {
		QStyleOptionTab opt;
		initStyleOption(&opt, i);

		//Apply slide offset only to dragged tab
		if (i == detachIndex && slideOffset != QPoint(0, 0)) {
			opt.rect.translate(slideOffset);
		}

		//Draw tab
		painter.drawControl(QStyle::CE_TabBarTab, opt);
	}
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

void DetachableTabBar::updateSlideAnimOffset()
{
	//Decrease the offset as time goes on
	slideOffset *= 0.75;
	
	//If the end position is reached, reset everything
	if (slideOffset.manhattanLength() < 1) {
		slideOffset = QPoint(0, 0);
		slideTimer.stop();
	}
	
	//Update tab bar
	update();
}