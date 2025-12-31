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

#ifndef VIEWER_H
#define VIEWER_H

#include <qmainwindow.h>
#include <qscrollarea.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>
#include <qtabwidget.h>
#include <qsplitter.h>
#include <vector>
#include <QKeyEvent>
#include "PDFEngine.h"
#include "NavigationBar.h"
#include "TabItem.h"
#include "DetachableTabBar.h"
#include "DetachableTabWidget.h"

struct MinMaxTuple {
	int min = 0, max = 0;
};

class TabItem;

class Viewer : public QMainWindow
{
	Q_OBJECT
public:
	explicit Viewer(QWidget* parent = 0);
	~Viewer();
	void keyPressEvent(QKeyEvent* event);
	void openFile(QString fileName);
	void setPageKey(int key = -1);
	void setAndUpdatePageKey(int key = -1);
	void addTab(TabItem* item);
	bool toggleDeleteTab();
	TabItem* getTab(int index);
private:
	QLabel* totalPage;
	QLineEdit* pageNumber;
	QPushButton* upButton;
	QPushButton* downButton;
	QComboBox* scaleBox;
	QPushButton* backwardsSearch;
	QPushButton* forwardsSearch;
	QLineEdit* searchBox;
	QHBoxLayout* layout;
	QSplitter* hSplitter;
	QAction* navBarShowAct;
	NavigationBar* navBar;
	QAction* rotate90CWAct, *rotate90CCWAct;
	QAction* pageTextAct;
	QAction* printAct;
	QWidget* wdgt;
	QWidget* plusWdgt;
	DetachableTabWidget* tWidget;
	DetachableTabBar* tabBar;
	std::vector <TabItem*> tabItems;
	int currentTab;
	bool deleteTab;
public slots:
	void onTabCloseRequested(int index);
private slots:
	void openFileDialog();
	void exitApp();
	void aboutApp();
	void setPage();
	void setAndUpdatePage();
	void setAndUpdateScale();
	void findPhrase();
	void getPageText();
	void showNavBar();
	void updatePageNavBar(const int pNum);
	void rotatePage();
	void onTabClicked(int index);
	void onTabMoved(int from, int to);
	void getPrintDialog();
	void checkIfPDFLoaded();
	void updatePageNumber();
	void openNewWindow(int index, const QPoint& windowPos);
	void mergeTabs(int index, QObject* srcViewer);
};

#endif
