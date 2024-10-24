#include "TabScrollArea.h"

TabScrollArea::TabScrollArea(QWidget* parent)
{
	topOrBottom = false;
}

bool TabScrollArea::returnTopOrBottom()
{
	return topOrBottom;
}

void TabScrollArea::wheelEvent(QWheelEvent* event)
{
	QScrollArea::wheelEvent(event);

	if (verticalScrollBar()->value() == verticalScrollBar()->maximum()) {
		topOrBottom = false;
		emit hitExtremity();
	}
	else if (verticalScrollBar()->value() == verticalScrollBar()->minimum()) {
		topOrBottom = true;
		emit hitExtremity();
	}
}