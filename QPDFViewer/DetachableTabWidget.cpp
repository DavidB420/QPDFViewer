#include "DetachableTabWidget.h"

#include <qevent.h>
#include <qmimedata.h>

DetachableTabWidget::DetachableTabWidget(QWidget* parent, DetachableTabBar* tBar)
{
	setTabBar(tBar);

	this->tBar = tBar;

	this->setAcceptDrops(true);
	setAttribute(Qt::WA_AcceptDrops, true);

	viewerPtr = parent;

	TAB_MIME = tBar->getTabMime();
}

void DetachableTabWidget::dropEvent(QDropEvent* event)
{
	QByteArray data = event->mimeData()->data(TAB_MIME);
	QDataStream in(&data, QIODevice::ReadOnly);

	quintptr srcPtr;
	int index;
	in >> srcPtr >> index;

	QObject* srcViewer = reinterpret_cast<QObject*>(srcPtr);

	if (srcViewer != viewerPtr)
		emit tabMerged(index, srcViewer);
	else
		emit tBar->detachTab(index, pos());

	event->acceptProposedAction();
	event->setDropAction(Qt::MoveAction);
}

void DetachableTabWidget::dragMoveEvent(QDragMoveEvent* event)
{
	if (event->mimeData()->hasFormat(TAB_MIME)) {
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}

void DetachableTabWidget::dragEnterEvent(QDragEnterEvent* event)
{
	if (event->mimeData()->hasFormat(TAB_MIME)) {
		event->setDropAction(Qt::MoveAction);
		event->accept();
	}
}