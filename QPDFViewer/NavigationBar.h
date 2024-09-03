#ifndef NAVIGATIONBAR_H
#define NAVIGATIONBAR_H

#include <qwidget.h>
#include <qtreeview.h>
#include <vector>
#include <qstandarditemmodel.h>

struct NavTuple {
	QStandardItem* sItem;
	int pageNum;
};

class NavigationBar : public QWidget
{
	Q_OBJECT
public:
	NavigationBar(QWidget* parent = 0);
	QTreeView* returnTree();
	std::vector<NavTuple> navItems;
private:
	QTreeView* navTree;
signals:
	void itemClicked(const int pNum);
private slots:
	void onItemClicked(const QModelIndex& index);
};

#endif