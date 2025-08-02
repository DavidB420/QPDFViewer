/**
 * Copyright 2025 David Badiei
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
	int returnNumOfItems();
	std::vector<NavTuple> navItems;
private:
	QTreeView* navTree;
signals:
	void itemClicked(const int pNum);
private slots:
	void onItemClicked(const QModelIndex& index);
};

#endif