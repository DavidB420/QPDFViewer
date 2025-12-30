#ifndef DETACHABLETABWIDGET_H
#define DETACHABLETABWIDGET_H

#include <qtabwidget.h>

#include "DetachableTabBar.h"

class DetachableTabWidget : public QTabWidget
{
	Q_OBJECT
public:
	DetachableTabWidget(QWidget* parent = 0, DetachableTabBar* tBar = 0);
signals:
	void tabMerged(int index, QObject* srcViewer);
protected:
	void dropEvent(QDropEvent* event) override;
	void dragMoveEvent(QDragMoveEvent* event) override;
	void dragEnterEvent(QDragEnterEvent* event) override;
private:
	void* viewerPtr;
	QString TAB_MIME;
	DetachableTabBar* tBar;
};

#endif