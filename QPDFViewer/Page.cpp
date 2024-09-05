#include "Page.h"
#include <qpainter.h>
#include "PDFEngine.h"

Page::Page(QWidget* parent, PDFEngine* pdfParent, QImage *img)
{
	pagePixmap = QPixmap::fromImage(*img);
	this->parent = pdfParent;
	dragging = false;
	isDragging = false;
}

QPixmap Page::getPagePixmap()
{
	return pagePixmap;
}

void Page::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		dragging = true;
		firstPoint = event->pos();
	}
}

void Page::mouseMoveEvent(QMouseEvent* event)
{
	if (dragging) {
		currentPoint = event->pos();
		isDragging = true;
		update();
	}
}

void Page::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && dragging) {
		dragging = false;
		update();
		if (isDragging) {
			QRectF selectionRect(firstPoint, currentPoint);
			parent->displayTextBox(selectionRect);
		}
		isDragging = false;
		currentPoint = QPointF(0,0);
		firstPoint = QPointF(0,0);
	}
}

void Page::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);

	painter.drawPixmap(0, 0, pagePixmap);

	if (dragging) {
		QRectF dragRect(firstPoint, currentPoint);
		painter.setBrush(QBrush(Qt::blue, Qt::Dense4Pattern));
		painter.drawRect(dragRect);
	}
}

void Page::drawSelection(QRectF rect)
{
	dragging = true;
	firstPoint.setX(rect.x());
	firstPoint.setY(rect.y());
	currentPoint.setX(firstPoint.x() + rect.width());
	currentPoint.setY(firstPoint.y() + rect.height());
	update();
}


