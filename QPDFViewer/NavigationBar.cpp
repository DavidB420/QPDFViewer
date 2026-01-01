/**
 * Copyright 2026 David Badiei
 *
 * This file is part of QPDFViewer, hereafter referred to as the program.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "NavigationBar.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qtreeview.h>
#include <qheaderview.h>
#include <qscrollbar.h>

NavigationBar::NavigationBar(QWidget* parent)
{
	//Draw nav bar with tree view
	QVBoxLayout* layout = new QVBoxLayout;

	QLabel* navLabel = new QLabel(this);
	navLabel->setText("<b>Navigation</b>");
	layout->addWidget(navLabel);

	navTree = new QTreeView(this);
	navTree->header()->setVisible(false);
	navTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	navTree->header()->setStretchLastSection(false);
	layout->addWidget(navTree);
	connect(navTree, &QTreeView::clicked, this, &NavigationBar::onItemClicked);

	this->setLayout(layout);

	this->resize(200, this->height());
	this->setMinimumWidth(200);
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
	//Emit signal that navigates to page number of the nav item
	QStandardItemModel* model = qobject_cast<QStandardItemModel*>(navTree->model());
	QStandardItem* item = model->itemFromIndex(index);

	for (int i = 0; i < navItems.size(); i++) {
		if (navItems.at(i).sItem == item) {
			emit itemClicked(navItems.at(i).pageNum);
			break;
		}
	}
}