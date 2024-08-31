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

	this->setLayout(layout);

	this->setFixedWidth(200);
}

QTreeView* NavigationBar::returnTree()
{
	return navTree;
}
