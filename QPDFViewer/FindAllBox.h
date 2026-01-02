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

#ifndef FINDALLBOX_H
#define FINDALLBOX_H

#include <qdialog.h>
#include <qtreewidget.h>
#include <qstyleditemdelegate.h>
#include <qobject.h>
#include "FindAllWorker.h"

class FindAllBox : public QDialog
{
	Q_OBJECT
public:
	FindAllBox(QWidget* parent = 0, QString phrase="", int direction = 0); //0 - bidirectional, 1 - forward, 2 - backward
	~FindAllBox();
private:
	QTreeWidget* results;
signals:
	void itemClicked(int page, QRectF rect);
public slots:
	void addItemToBox(SearchResult result);
private slots:
	void selectResult(QTreeWidgetItem* item, int column);
};


class HtmlItemDelegate : public QStyledItemDelegate
{
public:
	using QStyledItemDelegate::QStyledItemDelegate;
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

#endif
