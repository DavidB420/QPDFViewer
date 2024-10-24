#ifndef TABSCROLLAREA_H
#define TABSCROLLAREA_H

#include "qscrollarea.h"
#include <QScrollBar>

class TabScrollArea : public QScrollArea
{
	Q_OBJECT
public:
	TabScrollArea(QWidget* parent = nullptr);
	bool returnTopOrBottom();
protected:
	void wheelEvent(QWheelEvent* event) override;
private:
	bool topOrBottom;
signals:
	void hitExtremity();
};

#endif