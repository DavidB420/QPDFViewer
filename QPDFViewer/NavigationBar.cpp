#include "NavigationBar.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qtreeview.h>
#include <qheaderview.h>

NavigationBar::NavigationBar(QWidget* parent)
{
	QVBoxLayout* layout = new QVBoxLayout;
	
	QLabel* navLabel = new QLabel(this);
	navLabel->setText("<b>Navigation</b>");
	layout->addWidget(navLabel);

	navTree = new QTreeView(this);
	navTree->header()->setVisible(false);
	layout->addWidget(navTree);
	connect(navTree, &QTreeView::clicked, this, &NavigationBar::onItemClicked);

	this->setLayout(layout);

	this->setFixedWidth(200);
}

QTreeView* NavigationBar::returnTree()
{
	return navTree;
}

int NavigationBar::returnNumOfItems()
{
	return navItems.size();
}

void NavigationBar::onItemClicked(const QModelIndex& index)
{
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(navTree->model());
	QStandardItem* item = model->itemFromIndex(index);

	for (int i = 0; i < navItems.size(); i++) {
		if (navItems.at(i).sItem == item) {
			emit itemClicked(navItems.at(i).pageNum);
			break;
		}
	}
}