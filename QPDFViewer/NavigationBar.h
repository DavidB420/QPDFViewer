#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <qwidget.h>
#include <qtreeview.h>

class NavigationBar : public QWidget
{
public:
	NavigationBar(QWidget* parent = 0);
	QTreeView* returnTree();
private:
	QTreeView* navTree;
};

#endif