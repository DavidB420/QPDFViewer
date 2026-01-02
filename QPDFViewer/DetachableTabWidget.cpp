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

#include "DetachableTabWidget.h"

#include <qevent.h>
#include <qmimedata.h>

DetachableTabWidget::DetachableTabWidget(QWidget* parent, DetachableTabBar* tBar)
{
	//Set default values and get mime value from tab bar
	setTabBar(tBar);

	this->tBar = tBar;

	this->setAcceptDrops(true);
	setAttribute(Qt::WA_AcceptDrops, true);

	viewerPtr = parent;

	TAB_MIME = tBar->getTabMime();
}

void DetachableTabWidget::dropEvent(QDropEvent* event)
{
	//Read data returned from drop
	QByteArray data = event->mimeData()->data(TAB_MIME);
	QDataStream in(&data, QIODevice::ReadOnly);

	quintptr srcPtr;
	int index;
	in >> srcPtr >> index;

	QObject* srcViewer = reinterpret_cast<QObject*>(srcPtr);

	//If this tab came from another window, merge it, otherwise detach it into another window
	if (srcViewer != viewerPtr)
		emit tabMerged(index, srcViewer);
	else
		emit tBar->detachTab(index, pos());

	//Drop was successful
	event->acceptProposedAction();
	event->setDropAction(Qt::MoveAction);
}

void DetachableTabWidget::dragMoveEvent(QDragMoveEvent* event)
{
	//Check if mime matches
	if (event->mimeData()->hasFormat(TAB_MIME)) {
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}

void DetachableTabWidget::dragEnterEvent(QDragEnterEvent* event)
{
	//Check if mime matches
	if (event->mimeData()->hasFormat(TAB_MIME)) {
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}