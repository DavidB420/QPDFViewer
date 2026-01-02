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

#include "FindAllBox.h"

#include <qlayout.h>
#include <qpainter.h>
#include <qtextdocument.h>

FindAllBox::FindAllBox(QWidget* parent, QString phrase, int direction)
{
	this->setWindowTitle("Find all '" + phrase + "'");
	switch (direction) {
	case 1:
		this->setWindowTitle(this->windowTitle() + " forwards");
		break;
	case 2:
		this->setWindowTitle(this->windowTitle() + " backwards");
		break;
	}

	this->resize(640, 480);
	this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	results = new QTreeWidget(this);
	results->setColumnCount(2);
	results->setHeaderLabels({ "Page","Snippet" });
	results->setRootIsDecorated(false);
	results->setUniformRowHeights(true);
	results->setSelectionBehavior(QAbstractItemView::SelectRows);
	results->setSelectionMode(QAbstractItemView::SingleSelection);
	results->setTextElideMode(Qt::ElideNone);
	results->setWordWrap(true);
	HtmlItemDelegate* delegate = new HtmlItemDelegate(this);
	results->setItemDelegateForColumn(1, delegate);


	QVBoxLayout* layout = new QVBoxLayout;

	layout->addWidget(results);

	this->setLayout(layout);

	connect(results, &QTreeWidget::itemActivated, this, &FindAllBox::selectResult);

}

FindAllBox::~FindAllBox()
{
	results->clear();
}

void FindAllBox::addItemToBox(SearchResult result)
{
	QTreeWidgetItem* newItem = new QTreeWidgetItem(results);
	newItem->setText(0, QString::number(result.page));
	newItem->setText(1, result.snippet);
	newItem->setData(0, Qt::UserRole, result.page);
	newItem->setData(1, Qt::UserRole, result.foundRect);
}

void FindAllBox::selectResult(QTreeWidgetItem* item, int column)
{
	emit itemClicked(item->data(0, Qt::UserRole).toInt(), item->data(1, Qt::UserRole).toRectF());
}

void HtmlItemDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	//Save painter state
	painter->save();
	
	//Create a local copy of the style options for this item This contains text, colors, selection state, fonts, etc.
	QStyleOptionViewItem opt(option);
	//Initialize the option with data from the model index (sets opt.text, opt.rect, selection state, font, etc.)
	initStyleOption(&opt, index);

	//Create a QTextDocument to render rich text (HTML)
	QTextDocument doc;
	doc.setHtml(opt.text);
	
	//Clear the text in the style option so Qt does NOT draw it as plain text (otherwise it would be drawn twice)
	opt.text.clear();

	//Ask the current style to draw the item background, selection highlight, focus rectangle, etc. — but without any text
	opt.widget->style()->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

	//Move the painter's origin to the top-left corner of the item rectangle so text doc draws in the correct position
	painter->translate(opt.rect.topLeft());
	QRect clip(0, 0, opt.rect.width(), opt.rect.height());
	doc.drawContents(painter, clip);

	//Restore the painter to its original state
	painter->restore();
}

QSize HtmlItemDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	//Create a QTextDocument to render rich text (HTML)
	QTextDocument doc;
	//Set text
	doc.setHtml(index.data().toString());
	//Get exact width from QStyleOptionViewItem
	doc.setTextWidth(option.rect.width());

	//Return updated size
	return QSize(doc.idealWidth(), doc.size().height());
}
