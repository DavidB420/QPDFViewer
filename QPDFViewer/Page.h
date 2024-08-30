#ifndef PAGE_H
#define PAGE_H

#include <qlabel.h>
#include <qwidget.h>
#include <qpixmap.h>
#include <QMouseEvent>
#include <QPaintEvent>

class PDFEngine;

class Page : public QLabel
{
public:
	Page(QWidget* parent = 0, PDFEngine *pdfParent = 0, QImage *img = 0);
	void mousePressEvent(QMouseEvent* event);
	void mouseMoveEvent(QMouseEvent* event);
	void mouseReleaseEvent(QMouseEvent* event);
	void paintEvent(QPaintEvent* event);
	void drawSelection(QRectF rect);
private:
	bool dragging, isDragging;
	QPixmap pagePixmap;
	QPointF firstPoint, currentPoint;
	PDFEngine* parent;
};

#endif

