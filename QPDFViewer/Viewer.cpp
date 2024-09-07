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
#include "TabItem.h"

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
	connect(openAct, &QAction::triggered, this, &Viewer::openFile);
	fileMenu->addAction(openAct);
	pageTextAct = new QAction(tr("&Page Text"), this);
	connect(pageTextAct, &QAction::triggered, this, &Viewer::getPageText);
	fileMenu->addAction(pageTextAct);
	printAct = new QAction(tr("&Print"), this);
	fileMenu->addAction(printAct);
	connect(printAct, &QAction::triggered, this, &Viewer::getPrintDialog);
	fileMenu->addSeparator();
	QAction* exitAct = new QAction(tr("&Exit"), this);
	fileMenu->addAction(exitAct);
	connect(exitAct, &QAction::triggered, this, &Viewer::exitApp);
	rotate90CWAct = new QAction(QString::fromUtf8(u8"Rotate 90° CW"), this);
	pageMenu->addAction(rotate90CWAct);
	connect(rotate90CWAct, &QAction::triggered, this, &Viewer::rotatePage);
	rotate90CCWAct = new QAction(QString::fromUtf8(u8"Rotate 90° CCW"), this);
	pageMenu->addAction(rotate90CCWAct);
	connect(rotate90CCWAct, &QAction::triggered, this, &Viewer::rotatePage);
	navBarShowAct = new QAction(tr("&Show Navigation Bar"), this);
	navBarShowAct->setCheckable(true);
	navMenu->addAction(navBarShowAct);
	connect(navBarShowAct, &QAction::triggered, this, &Viewer::showNavBar);
	QAction* aboutAct = new QAction(tr("&About"), this);
	aboutmenu->addAction(aboutAct);
	connect(aboutAct, &QAction::triggered, this, &Viewer::aboutApp);

	//Add toolbar
	QToolBar* toolBar = new QToolBar(this);
	this->addToolBar(toolBar);
	toolBar->setMovable(false);
	toolBar->setFixedHeight(30);
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
	upButton->setText(QString::fromUtf8(u8"▲"));
	upButton->setFixedWidth(45);
	toolBar->addWidget(upButton);
	connect(upButton, &QPushButton::clicked, this, & Viewer::setAndUpdatePage);
	downButton = new QPushButton(this);
	downButton->setText(QString::fromUtf8(u8"▼"));
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
	backwardsSearch->setText("Look Backwards");
	toolBar->addWidget(backwardsSearch);
	connect(backwardsSearch, &QPushButton::pressed, this, &Viewer::findPhrase);
	forwardsSearch = new QPushButton(this);
	forwardsSearch->setText("Look Forwards");
	toolBar->addWidget(forwardsSearch);
	connect(forwardsSearch, &QPushButton::pressed, this, &Viewer::findPhrase);

	//Initialize scroll area and hbox layout
	layout = new QHBoxLayout;
	tWidget = new QTabWidget(this);
	tWidget->setMovable(true);
	tWidget->setTabsClosable(true);
	layout->addWidget(tWidget);
	layout->setContentsMargins(0, 0, 0, 0);
	currentTab = 0;
	tabItems.push_back(new TabItem());
	tWidget->addTab(tabItems.at(currentTab), "No PDF loaded");
	tWidget->addTab(new QWidget(), tr("+"));
	tWidget->tabBar()->setTabButton(tWidget->count() - 1, QTabBar::RightSide, nullptr);
	connect(tWidget, &QTabWidget::tabBarClicked, this, &Viewer::onTabClicked);
	connect(tWidget->tabBar(), &QTabBar::tabMoved, this, &Viewer::onTabMoved);
	connect(tWidget, &QTabWidget::tabCloseRequested, this, &Viewer::onTabCloseRequested);
	QWidget* layoutWidget = new QWidget(this);
	layoutWidget->setLayout(layout);
	setCentralWidget(layoutWidget);

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

void Viewer::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open PDF file"), NULL, tr("PDF Files (*.pdf)"));

	if (fileName != NULL) {
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
		checkIfPDFLoaded();

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

void Viewer::exitApp()
{
	QApplication::exit();
}

void Viewer::aboutApp()
{
	//Display about box
	QMessageBox::about(this, tr("About QPDFViewer"),
		tr("<b>QPDFViewer 1.0</b><br>Written by David Badiei, 2024<br>Licensed under GNU General Public License v3 (GPL-3)"));
}

void Viewer::setAndUpdatePage() { setAndUpdatePageKey(); }

void Viewer::setAndUpdatePageKey(int key)
{
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

			if (!result)
				return;
			else
				pageNumber->setText(QString::number(tabItems.at(currentTab)->getEngine()->getCurrentPage()));
		}
		
		tabItems.at(currentTab)->updateScrollArea();
	}
}

void Viewer::setAndUpdateScale()
{
	if (scaleBox->currentText().endsWith("%"))
		scaleBox->setCurrentText(scaleBox->currentText().mid(0, scaleBox->currentText().length() - 1));
	
	if (!tabItems.at(currentTab)->getEngine()->setCurrentScale(scaleBox->currentText().toInt())) {
		QMessageBox::critical(this, "Out of bounds", "Entered value out of bounds!");
		return;
	}

	tabItems.at(currentTab)->updateScrollArea();

	scaleBox->setCurrentText(scaleBox->currentText() + "%");
}

void Viewer::findPhrase()
{
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
	tabItems.at(currentTab)->getEngine()->displayAllText();
}

void Viewer::showNavBar()
{
	if (navBarShowAct->isChecked()) {
		if (navBar != NULL) {
			delete navBar;
			navBar = NULL;
		}
		navBar = new NavigationBar(this);
		layout->insertWidget(0,navBar);
		tabItems.at(currentTab)->getEngine()->addNavOutline(navBar);
		connect(navBar, &NavigationBar::itemClicked, this, &Viewer::updatePageNavBar);
		tabItems.at(currentTab)->setUseNavBar(true);
	}
	else {
		delete navBar;
		navBar = NULL;
		tabItems.at(currentTab)->setUseNavBar(false);
	}
}

void Viewer::updatePageNavBar(const int pNum)
{
	tabItems.at(currentTab)->getEngine()->setCurrentPage(pNum);
	tabItems.at(currentTab)->updateScrollArea();
}

void Viewer::rotatePage()
{
	if (rotate90CWAct == sender())
		tabItems.at(currentTab)->getEngine()->rotatePDF(true);
	else if (rotate90CCWAct == sender())
		tabItems.at(currentTab)->getEngine()->rotatePDF(false);

	tabItems.at(currentTab)->updateScrollArea();
}

void Viewer::onTabClicked(int index)
{
	if (index == tWidget->count() - 1) {
		if (tWidget->count() == 2 && tWidget->widget(0)->isEnabled() == false) {
			tWidget->setTabVisible(0, true);
			tWidget->removeTab(0);
		}
		tabItems.push_back(new TabItem());
		currentTab = tWidget->count() - 1;
		int currentIndex = tWidget->insertTab(currentTab, tabItems.at(currentTab), "No PDF loaded");
		tWidget->setCurrentIndex(currentIndex);
	}

	if (index != tWidget->count() - 1)
		currentTab = index;

	if (tabItems.at(currentTab)->getFilePath().length() > 0)
		this->setWindowTitle("QPDFViewer - " + tabItems.at(currentTab)->getFilePath());
	else
		this->setWindowTitle("QPDFViewer");

	if (tabItems.at(currentTab)->getUseNavBar())
		navBarShowAct->setChecked(true);
	else
		navBarShowAct->setChecked(false);
	showNavBar();
	
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

	checkIfPDFLoaded();
}

void Viewer::onTabMoved(int from, int to)
{
	if (from == tWidget->count() - 1 || to == tWidget->count() - 1) {
		tWidget->tabBar()->blockSignals(true);
		tWidget->tabBar()->moveTab(to, from);
		tWidget->tabBar()->blockSignals(false);
		tWidget->setCurrentIndex(currentTab);
	}
	else {
		std::swap(tabItems[from], tabItems[to]);
		currentTab = from;
	}
}

void Viewer::onTabCloseRequested(int index)
{
	if (index < tWidget->count() - 1) {
		tWidget->removeTab(index);
		tabItems.erase(tabItems.begin()+index);
		if (currentTab > 0)
			currentTab--;
		tWidget->setCurrentIndex(currentTab);
	}

	if (tWidget->count() == 1) {
		QWidget* wdgt = new QWidget();
		wdgt->setEnabled(false);
		tWidget->insertTab(0, wdgt, "");
		tWidget->setTabVisible(0,false);
		tWidget->setCurrentIndex(0);
	}
}

void Viewer::getPrintDialog()
{
	QPrinter printer;
	QPrintDialog dialog(&printer, this);
	if (dialog.exec()) {
		QPainter painter(&printer);
		QRect rect = painter.viewport();
		int min = 0, max = 0;
		if (printer.printRange() == QPrinter::AllPages) {
			min = 1;
			max = tabItems.at(currentTab)->getEngine()->getTotalNumberOfPages();
		}
		else if (printer.printRange() == QPrinter::PageRange){
			min = dialog.fromPage();
			max = dialog.toPage();
		}
		if (min >= 1 && max <= tabItems.at(currentTab)->getEngine()->getTotalNumberOfPages()) {
			int tmp = tabItems.at(currentTab)->getEngine()->getCurrentPage();
			for (int i = 0; i < printer.copyCount(); i++) {
				for (int j = min; j <= max; j++) {
					tabItems.at(currentTab)->getEngine()->setCurrentPage(j);
					tabItems.at(currentTab)->updateScrollArea();
					QPixmap pMap = tabItems.at(currentTab)->getEngine()->returnImage()->getPagePixmap();
					QSize size = pMap.size();
					size.scale(rect.size(), Qt::KeepAspectRatio);
					painter.setViewport(rect.x(), rect.y(), size.width(), size.height());
					painter.setWindow(pMap.rect());
					painter.drawPixmap(0, 0, pMap);

					if (j < max)
						printer.newPage();
				}
			}
			tabItems.at(currentTab)->getEngine()->setCurrentPage(tmp);
			tabItems.at(currentTab)->updateScrollArea();
		}
		else
			QMessageBox::critical(this, "Page range out of bounds", "Entered page range out of bounds!");
	}
}

void Viewer::checkIfPDFLoaded()
{
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


