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

#include "Viewer.h"
#include <qmenubar.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qtoolbar.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <qcombobox.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qtabbar.h>
#include <qtoolbutton.h>
#include <qtabbar.h>
#include <qpainter.h>
#include <QtPrintSupport/qprinter.h>
#include <QtPrintSupport/qprintdialog.h>
#include <vector>
#include <QDebug>
#include "TabItem.h"
#include "PrintDialog.h"

Viewer::Viewer(QWidget* parent)
{
	//Set basic window settings
	this->setWindowTitle("QPDFViewer");
	this->resize(800, 600);

	//Add menu bar options
	QMenuBar* mBar = new QMenuBar(this);
	setMenuBar(mBar);
	QMenu* fileMenu = new QMenu(this);
	fileMenu = mBar->addMenu(tr("&File"));
	QMenu* pageMenu = new QMenu(this);
	pageMenu = mBar->addMenu(tr("&Page"));
	QMenu* navMenu = new QMenu(this);
	navMenu = mBar->addMenu(tr("&Navigation"));
	QMenu* aboutmenu = new QMenu(this);
	aboutmenu = mBar->addMenu(tr("&About"));
	QAction* openAct = new QAction(tr("&Open..."), this);
	connect(openAct, &QAction::triggered, this, &Viewer::openFileDialog);
	openAct->setIcon(QIcon(":/images/assets/openIcon.png"));
	fileMenu->addAction(openAct);
	pageTextAct = new QAction(tr("&Page Text"), this);
	connect(pageTextAct, &QAction::triggered, this, &Viewer::getPageText);
	pageTextAct->setIcon(QIcon(":/images/assets/pageTextIcon.png"));
	fileMenu->addAction(pageTextAct);
	printAct = new QAction(tr("&Print"), this);
	fileMenu->addAction(printAct);
	connect(printAct, &QAction::triggered, this, &Viewer::getPrintDialog);
	printAct->setIcon(QIcon(":/images/assets/printIcon.png"));
	fileMenu->addSeparator();
	QAction* exitAct = new QAction(tr("&Exit"), this);
	fileMenu->addAction(exitAct);
	exitAct->setIcon(QIcon(":/images/assets/closeIcon.png"));
	connect(exitAct, &QAction::triggered, this, &Viewer::exitApp);
	rotate90CWAct = new QAction(QString::fromUtf8(u8"Rotate 90° CW"), this);
	pageMenu->addAction(rotate90CWAct);
	rotate90CWAct->setIcon(QIcon(":/images/assets/rotate90CWIcon.png"));
	connect(rotate90CWAct, &QAction::triggered, this, &Viewer::rotatePage);
	rotate90CCWAct = new QAction(QString::fromUtf8(u8"Rotate 90° CCW"), this);
	rotate90CCWAct->setIcon(QIcon(":/images/assets/rotate90CCWIcon.png"));
	pageMenu->addAction(rotate90CCWAct);
	connect(rotate90CCWAct, &QAction::triggered, this, &Viewer::rotatePage);
	navBarShowAct = new QAction(tr("&Show Navigation Bar"), this);
	navBarShowAct->setCheckable(true);
	navMenu->addAction(navBarShowAct);
	connect(navBarShowAct, &QAction::triggered, this, &Viewer::showNavBar);
	QAction* aboutAct = new QAction(tr("&About"), this);
	aboutmenu->addAction(aboutAct);
	connect(aboutAct, &QAction::triggered, this, &Viewer::aboutApp);
	aboutAct->setIcon(QIcon(":/images/assets/aboutIcon.png"));

	//Add toolbar
	QToolBar* toolBar = new QToolBar(this);
	this->addToolBar(toolBar);
	toolBar->setMovable(false);
	toolBar->setFixedHeight(30);
	toolBar->toggleViewAction()->setEnabled(false);
	toolBar->toggleViewAction()->setVisible(false);
	QLabel* pageLbl = new QLabel(this);
	pageLbl->setText("Page: ");
	toolBar->addWidget(pageLbl);
	pageNumber = new QLineEdit(this);
	pageNumber->setMaximumWidth(50);
	pageNumber->setFixedHeight(20);
	pageNumber->setAlignment(Qt::AlignCenter);
	pageNumber->setValidator(new QIntValidator(this));
	connect(pageNumber, &QLineEdit::returnPressed, this, & Viewer::setAndUpdatePage);
	toolBar->addWidget(pageNumber);
	upButton = new QPushButton(this);
	upButton->setIcon(QIcon(":/images/assets/upIcon.png"));
	upButton->setFixedWidth(45);
	toolBar->addWidget(upButton);
	connect(upButton, &QPushButton::clicked, this, & Viewer::setAndUpdatePage);
	downButton = new QPushButton(this);
	downButton->setIcon(QIcon(":/images/assets/downIcon.png"));
	downButton->setFixedWidth(45);
	connect(downButton, &QPushButton::clicked, this, & Viewer::setAndUpdatePage);
	toolBar->addWidget(downButton);
	totalPage = new QLabel(this);
	totalPage->setText(" of ");
	toolBar->addWidget(totalPage);
	QLabel* scaleLbl = new QLabel(this);
	scaleLbl->setText("Scale Document:");
	toolBar->addWidget(scaleLbl);
	scaleBox = new QComboBox(this);
	scaleBox->setEditable(true);
	scaleBox->setValidator(new QRegExpValidator(QRegExp("[0-9]*%?"),this));
	scaleBox->setInsertPolicy(QComboBox::NoInsert);
	for (int i = 1; i <= 16; i++)
		scaleBox->insertItem(i-1, QString::number(i * 25) + "%");
	connect(scaleBox->lineEdit(), &QLineEdit::returnPressed, this, &Viewer::setAndUpdateScale);
	connect(scaleBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &Viewer::setAndUpdateScale);
	toolBar->addWidget(scaleBox);
	searchBox = new QLineEdit(this);
	searchBox->setFixedHeight(20);
	toolBar->addWidget(searchBox);
	backwardsSearch = new QPushButton(this);
	backwardsSearch->setFixedWidth(85);
	backwardsSearch->setIcon(QIcon(":/images/assets/backwardsIcon.png"));
	toolBar->addWidget(backwardsSearch);
	connect(backwardsSearch, &QPushButton::pressed, this, &Viewer::findPhrase);
	forwardsSearch = new QPushButton(this);
	forwardsSearch->setFixedWidth(85);
	forwardsSearch->setIcon(QIcon(":/images/assets/forwardsIcon.png"));
	toolBar->addWidget(forwardsSearch);
	connect(forwardsSearch, &QPushButton::pressed, this, &Viewer::findPhrase);

	//Initialize tabs and layout
	layout = new QHBoxLayout;
	hSplitter = new QSplitter(Qt::Horizontal);
	tWidget = new QTabWidget(this);
	tWidget->setMovable(true);
	tWidget->setTabsClosable(true);
	hSplitter->addWidget(tWidget);
	layout->addWidget(hSplitter);
	layout->setContentsMargins(0, 0, 0, 0);
	currentTab = 0;
	tabItems.push_back(new TabItem());
	tWidget->addTab(tabItems.at(currentTab), "No PDF loaded");
	tWidget->addTab(new QWidget(), tr("+"));
	tWidget->tabBar()->setTabButton(tWidget->count() - 1, QTabBar::RightSide, nullptr);
	connect(tabItems.at(currentTab)->getScrollArea(), &TabScrollArea::hitExtremity, this, &Viewer::setPage);
	connect(tWidget, &QTabWidget::tabBarClicked, this, &Viewer::onTabClicked);
	connect(tWidget->tabBar(), &QTabBar::tabMoved, this, &Viewer::onTabMoved);
	connect(tWidget, &QTabWidget::tabCloseRequested, this, &Viewer::onTabCloseRequested);
	QWidget* layoutWidget = new QWidget(this);
	layoutWidget->setLayout(layout);
	setCentralWidget(layoutWidget);

	//Disable pdf control buttons
	checkIfPDFLoaded();
}

Viewer::~Viewer()
{
}

void Viewer::keyPressEvent(QKeyEvent* event)
{
	switch (event->key())
	{
	case Qt::Key_F1:
	case Qt::Key_F2:
		setAndUpdatePageKey(event->key());

		break;
	}
}

void Viewer::openFile(QString fileName)
{
	if (fileName != NULL) {
		//Create and setup pdf engine and other controls based on file
		if (tabItems.at(currentTab)->getEngine() != NULL)
			delete tabItems.at(currentTab)->getEngine();
		tabItems.at(currentTab)->setPDFEngine(fileName.toStdString(), this);
		tabItems.at(currentTab)->setFilePath(fileName);
		tabItems.at(currentTab)->updateScrollArea();
		tWidget->setTabText(currentTab, QString::fromStdString(tabItems.at(currentTab)->getFileName()));
		totalPage->setText(" of " + QString::number(tabItems.at(currentTab)->getEngine()->getTotalNumberOfPages()) + " ");
		pageNumber->setText(QString::number(tabItems.at(currentTab)->getEngine()->getCurrentPage()));
		this->setWindowTitle("QPDFViewer - " + fileName);
		scaleBox->setCurrentIndex(3);

		//Enable all buttons now that pdf engine object exits
		checkIfPDFLoaded();

		//Check if we have data in the nav bar, if so make sure to show it when the file is loaded
		if (navBar != NULL) {
			delete navBar;
			navBar = NULL;
		}
		navBar = new NavigationBar(this);
		tabItems.at(currentTab)->getEngine()->addNavOutline(navBar);
		int tmp = navBar->returnNumOfItems();
		delete navBar;
		navBar = NULL;
		if (tmp > 0) {
			navBarShowAct->setChecked(true);
			showNavBar();
		}
	}
}

void Viewer::openFileDialog()
{
	//Create open file dialog then open the selected file
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open PDF file"), NULL, tr("PDF Files (*.pdf)"));

	openFile(fileName);
}

void Viewer::exitApp()
{
	QApplication::exit();
}

void Viewer::aboutApp()
{
	//Display about box
	QMessageBox::about(this, tr("About QPDFViewer"),
		tr("<b>QPDFViewer 2.0 BETA</b><br>Written by David Badiei, 2025<br>Licensed under GNU General Public License v3 (GPL-3)"));
}

void Viewer::setPage() { setPageKey(); tabItems.at(currentTab)->rerenderUpdateScrollArea();}

void Viewer::setAndUpdatePage() { setPage(); tabItems.at(currentTab)->updateScrollArea();}

void Viewer::setAndUpdatePageKey(int key) { setPageKey(key); tabItems.at(currentTab)->updateScrollArea(); }

void Viewer::setPageKey(int key)
{
	//Updates page number either when function keys or arrow buttons are pressed
	if (tabItems.at(currentTab)->getEngine() != NULL) {
		if (pageNumber == sender()) {
			if (!tabItems.at(currentTab)->getEngine()->setCurrentPage(pageNumber->text().toInt())) {
				QMessageBox::critical(this, "Out of bounds", "Entered value out of bounds!");
				return;
			}
		}
		else {
			bool result = false;
			if (upButton == sender() || key == Qt::Key_F2)
				result = tabItems.at(currentTab)->getEngine()->setCurrentPage(tabItems.at(currentTab)->getEngine()->getCurrentPage() + 1);
			else if (downButton == sender() || key == Qt::Key_F1)
				result = tabItems.at(currentTab)->getEngine()->setCurrentPage(tabItems.at(currentTab)->getEngine()->getCurrentPage() - 1);

			if (sender() == tabItems.at(currentTab)->getScrollArea())
				result = tabItems.at(currentTab)->getEngine()->setCurrentPage(tabItems.at(currentTab)->getScrollArea()->getPageToLoad());

			if (!result)
				return;
			else
				pageNumber->setText(QString::number(tabItems.at(currentTab)->getEngine()->getCurrentPage()));
		}
		
	}
}

void Viewer::setAndUpdateScale()
{
	//Update scale if valid
	if (tabItems.at(currentTab)->getEngine() != NULL) {
		if (scaleBox->currentText().endsWith("%"))
			scaleBox->setCurrentText(scaleBox->currentText().mid(0, scaleBox->currentText().length() - 1));

		if (!tabItems.at(currentTab)->getEngine()->setCurrentScale(scaleBox->currentText().toInt())) {
			QMessageBox::critical(this, "Out of bounds", "Entered value out of bounds!");
			return;
		}

		//Refresh page then update scale box
		tabItems.at(currentTab)->updateScrollArea(true);

		scaleBox->setCurrentText(scaleBox->currentText() + "%");
	}
}

void Viewer::findPhrase()
{
	//Search for phrase either forwards or backwards
	bool result = false;

	if (forwardsSearch == sender())
		result = tabItems.at(currentTab)->getEngine()->findPhraseInDocument(searchBox->text().toStdString(), poppler::page::search_next_result);
	else if (backwardsSearch == sender())
		result = tabItems.at(currentTab)->getEngine()->findPhraseInDocument(searchBox->text().toStdString(),poppler::page::search_previous_result);

	if (result) {
		tabItems.at(currentTab)->updateScrollArea();
		pageNumber->setText(QString::number(tabItems.at(currentTab)->getEngine()->getCurrentPage()));
	}
	else
		QMessageBox::warning(this, "Could not find phrase", "Could not find phrase: " + searchBox->text());
}

void Viewer::getPageText()
{
	//Display all text in the page in seperate window
	tabItems.at(currentTab)->getEngine()->displayAllText();
}

void Viewer::showNavBar()
{
	//Show nav bar if the action is checked
	if (navBarShowAct->isChecked()) {
		if (navBar != NULL) {
			delete navBar;
			navBar = NULL;
		}
		navBar = new NavigationBar(this);

		//Update splitter with nav bar
		hSplitter->insertWidget(0,navBar);
		hSplitter->setSizes({ 200, INT_MAX-500 });
		for (int i = 0; i < hSplitter->count(); i++)
			hSplitter->setCollapsible(i, false);

		tabItems.at(currentTab)->getEngine()->addNavOutline(navBar);

		//Save the state if its uninitialized, then restore the state for all runs
		if (tabItems.at(currentTab)->getSplitterData().size() == 1 && tabItems.at(currentTab)->getSplitterData()[0] == '\0')
			tabItems.at(currentTab)->setSplitterData(hSplitter->saveState());
		hSplitter->restoreState(tabItems.at(currentTab)->getSplitterData());

		connect(navBar, &NavigationBar::itemClicked, this, &Viewer::updatePageNavBar);
		tabItems.at(currentTab)->setUseNavBar(true);
		
		//If the splitter is moved save the new state
		QObject::connect(hSplitter, &QSplitter::splitterMoved,
			[=](int pos, int index) {
				Q_UNUSED(index);
				tabItems.at(currentTab)->setSplitterData(hSplitter->saveState());
			});
	}
	else {
		delete navBar;
		navBar = NULL;
		tabItems.at(currentTab)->setUseNavBar(false);
	}
}

void Viewer::updatePageNavBar(const int pNum)
{
	//Update the page based on button clicked in nav bar
	tabItems.at(currentTab)->getEngine()->setCurrentPage(pNum);
	tabItems.at(currentTab)->updateScrollArea(true);
	pageNumber->setText(QString::number(pNum));
}

void Viewer::rotatePage()
{
	//Rotate and update page either 90 CW or CCWs
	if (rotate90CWAct == sender())
		tabItems.at(currentTab)->getEngine()->rotatePDF(true);
	else if (rotate90CCWAct == sender())
		tabItems.at(currentTab)->getEngine()->rotatePDF(false);

	tabItems.at(currentTab)->updateScrollArea(true);
}

void Viewer::onTabClicked(int index)
{
	//If plus button has been pressed
	if (index == tWidget->count() - 1) {
		//Delete invisible tab if there are now pdf tabs open beforehand
		if (tWidget->count() == 2 && tWidget->widget(0)->isEnabled() == false) {
			tWidget->setTabVisible(0, true);
			tWidget->removeTab(0);
		}
		//Create new tab and set it as the current tab
		tabItems.push_back(new TabItem());
		currentTab = tWidget->count() - 1;
		int currentIndex = tWidget->insertTab(currentTab, tabItems.at(currentTab), "No PDF loaded");
		tWidget->setCurrentIndex(currentIndex);
		connect(tabItems.at(currentTab)->getScrollArea(), &TabScrollArea::hitExtremity, this, &Viewer::setAndUpdatePage);
	}

	//Update current tab if we are not pressing the the plus buttton
	if (index != tWidget->count() - 1)
		currentTab = index;

	//Change window title depending on if a pdf has been loaded or not
	if (tabItems.at(currentTab)->getFilePath().length() > 0)
		this->setWindowTitle("QPDFViewer - " + tabItems.at(currentTab)->getFilePath());
	else
		this->setWindowTitle("QPDFViewer");

	//Check if clicked tab has been using the nav bar
	if (tabItems.at(currentTab)->getUseNavBar())
		navBarShowAct->setChecked(true);
	else
		navBarShowAct->setChecked(false);
	showNavBar();
	
	//Update some of the controls depending on if a pdf has been loaded or not
	if (tabItems.at(currentTab)->getEngine() != NULL) {
		pageNumber->setText(QString::number(tabItems.at(currentTab)->getEngine()->getCurrentPage()));
		scaleBox->setCurrentText(QString::number(tabItems.at(currentTab)->getEngine()->getScaleValue()) + "%");
		totalPage->setText(" of " + QString::number(tabItems.at(currentTab)->getEngine()->getTotalNumberOfPages()) + " ");
	}
	else {
		pageNumber->setText("");
		scaleBox->setCurrentText("25%");
		totalPage->setText(" of ");
	}

	//Enable or disable some of the controls depending on if a pdf has been loaded or not
	checkIfPDFLoaded();
}

void Viewer::onTabMoved(int from, int to)
{
	//If user moved a tab onto the plus tab
	if (from == tWidget->count() - 1 || to == tWidget->count() - 1) {
		//Move it back
		tWidget->tabBar()->blockSignals(true);
		tWidget->tabBar()->moveTab(to, from);
		tWidget->tabBar()->blockSignals(false);
		tWidget->setCurrentIndex(currentTab);
	}
	else {
		//Otherwise do the move
		std::swap(tabItems[from], tabItems[to]);
		currentTab = from;
	}
}

void Viewer::onTabCloseRequested(int index)
{
	//Check if this isnt the plus tab, otherwise do the remove
	if (index < tWidget->count() - 1) {
		tWidget->removeTab(index);
		delete tabItems.at(index);
		tabItems.erase(tabItems.begin()+index);
		if (currentTab > 0)
			currentTab--;
		tWidget->setCurrentIndex(currentTab);
	}

	//Delete navbar if only plus button is left
	if (tWidget->count() == 1) {
		delete navBar;
		navBar = NULL;
	}

	//If we have no tabs left, create an invisible one so the plus button isnt in focus, otherwise update pdf controls
	if (tWidget->count() == 1) {
		QWidget* wdgt = new QWidget();
		wdgt->setEnabled(false);
		tWidget->insertTab(0, wdgt, "");
		tWidget->setTabVisible(0,false);
		tWidget->setCurrentIndex(0);
	}
	else {
		onTabClicked(currentTab);
	}
}

void Viewer::getPrintDialog()
{
	PrintDialog* pDialog = new PrintDialog(this);
	pDialog->show();
	QPrinter printer;
	
	//Handle QPDFViewer's native print dialog, or give OS dialog
	if (pDialog->exec() && pDialog->result() == QDialog::Accepted) {
		QPrintDialog dialog(&printer, this);
		int dialogResult = 0;
		std::vector<MinMaxTuple> minMaxList;
		if (pDialog->getOSDialog())
			dialogResult = dialog.exec();
		else {
			//Save results in QPrinter
			printer.setPrinterName(pDialog->returnPrinterInfo().selectedPrinter);
			printer.setCopyCount(pDialog->returnPrinterInfo().copies);
			printer.setColorMode(pDialog->returnPrinterInfo().colorMode ? QPrinter::ColorMode::Color : QPrinter::ColorMode::GrayScale);
			printer.setPageOrientation(pDialog->returnPrinterInfo().orientation == 0 ? QPageLayout::Portrait : QPageLayout::Landscape);
			printer.setPrintRange(!QString::compare(pDialog->returnPrinterInfo().pageRange, QString("All pages")) ? QPrinter::AllPages : QPrinter::PageRange);

			//If a specific range is given, save it
			if (printer.printRange() == QPrinter::PageRange) {
				QStringList ranges = pDialog->returnPrinterInfo().pageRange.split(",",Qt::SkipEmptyParts);
				for (const QString& part : ranges) {
					QStringList numbers = part.split("-", Qt::SkipEmptyParts);
					if (numbers.size() == 2) {
						MinMaxTuple newTuple;
						newTuple.min = numbers.at(0).toInt();
						newTuple.max = numbers.at(1).toInt();
						if (newTuple.min <= newTuple.max)
							minMaxList.push_back(newTuple);
					}
					else if (numbers.size() == 1) {
						MinMaxTuple newTuple;
						newTuple.min = numbers.at(0).toInt();
						newTuple.max = numbers.at(0).toInt();
						minMaxList.push_back(newTuple);
					}
				}
			}

			//Dialog completed successfully
			dialogResult = 1;
		}

		if (dialogResult) {
			int copies = printer.copyCount(); //we handle multiple copies, not the printer driver, so qprinter's copy count must be set to 1
			printer.setCopyCount(1);
			QPainter painter(&printer);
			QRect rect = painter.viewport();

			//Check if we are printing all or just a selection
			if (printer.printRange() == QPrinter::AllPages) {
				MinMaxTuple newTuple;
				newTuple.min = 1;
				newTuple.max = tabItems.at(currentTab)->getEngine()->getTotalNumberOfPages();
				minMaxList.push_back(newTuple);
			}
			else if (printer.printRange() == QPrinter::PageRange && pDialog->getOSDialog()) {
				MinMaxTuple newTuple;
				newTuple.min = dialog.fromPage();
				newTuple.max = dialog.toPage();
				minMaxList.push_back(newTuple);
			}

			//If range is valid print the range * any copies the user wants
			for (int numOfTuples = 0; numOfTuples < minMaxList.size(); numOfTuples++) {
				int min = minMaxList.at(numOfTuples).min;
				int max = minMaxList.at(numOfTuples).max;
				if (min >= 1 && max <= tabItems.at(currentTab)->getEngine()->getTotalNumberOfPages()) {
					int tmp = tabItems.at(currentTab)->getEngine()->getCurrentPage();
					for (int i = 0; i < copies; i++) {
						for (int j = min; j <= max; j++) {
							tabItems.at(currentTab)->getEngine()->setCurrentPage(j);
							tabItems.at(currentTab)->updateScrollArea();
							QPixmap pMap = tabItems.at(currentTab)->getEngine()->returnImage()->getPagePixmap();
							QSize size = pMap.size();
							size.scale(rect.size(), Qt::KeepAspectRatio);
							painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
							painter.setWindow(pMap.rect());
							painter.drawPixmap(0, 0, pMap);

							if (j < max || numOfTuples < minMaxList.size() - 1) //create new page if we are on a new page or a new section
								printer.newPage();
						}

						if (i < copies - 1 && numOfTuples == minMaxList.size()-1) //only create a new page here if we are on a new copy and last section
							printer.newPage();
					}

					//Reset page as it was taken over by the print operation
					tabItems.at(currentTab)->getEngine()->setCurrentPage(tmp);
					tabItems.at(currentTab)->updateScrollArea();
				}
				else
					QMessageBox::critical(this, "Page range out of bounds", "Entered page range ( " + QString::number(min) + " , " + QString::number(max) + " ) out of bounds!");
			}
		}
	}
}

void Viewer::checkIfPDFLoaded()
{
	//Turn controls on or off depending if we have the engine loaded
	bool toggle;

	if (tabItems.at(currentTab)->getEngine() == NULL)
		toggle = false;
	else
		toggle = true;

	pageTextAct->setEnabled(toggle);
	printAct->setEnabled(toggle);
	rotate90CCWAct->setEnabled(toggle);
	rotate90CWAct->setEnabled(toggle);
	pageNumber->setEnabled(toggle);
	upButton->setEnabled(toggle);
	downButton->setEnabled(toggle);
	scaleBox->setEnabled(toggle);
	navBarShowAct->setEnabled(toggle);
	upButton->setEnabled(toggle);
	downButton->setEnabled(toggle);
	backwardsSearch->setEnabled(toggle);
	forwardsSearch->setEnabled(toggle);
}


