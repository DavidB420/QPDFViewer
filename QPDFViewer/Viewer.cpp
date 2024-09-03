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
	QAction* pageTextAct = new QAction(tr("&Page Text"), this);
	connect(pageTextAct, &QAction::triggered, this, &Viewer::getPageText);
	fileMenu->addAction(pageTextAct);
	QAction* printAct = new QAction(tr("&Print"), this);
	fileMenu->addAction(printAct);
	fileMenu->addSeparator();
	QAction* exitAct = new QAction(tr("&Exit"), this);
	fileMenu->addAction(exitAct);
	connect(exitAct, &QAction::triggered, this, &Viewer::exitApp);
	QAction* rotate90CWAct = new QAction(QString::fromUtf8(u8"Rotate 90° CW"), this);
	pageMenu->addAction(rotate90CWAct);
	QAction* rotate90CCWAct = new QAction(QString::fromUtf8(u8"Rotate 90° CCW"), this);
	pageMenu->addAction(rotate90CCWAct);
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
	scrollArea = new QScrollArea(this);
	scrollArea->setBackgroundRole(QPalette::Mid);
	scrollArea->setAlignment(Qt::AlignCenter);
	layout->addWidget(scrollArea);
	layout->setContentsMargins(0, 0, 0, 0);
	QWidget* layoutWidget = new QWidget(this);
	layoutWidget->setLayout(layout);
	setCentralWidget(layoutWidget);
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
		if (engine != NULL)
			delete engine;
		engine = new PDFEngine(fileName.toStdString(), this);
		scrollArea->setWidget(engine->returnImage());
		totalPage->setText(" of " + QString::number(engine->getTotalNumberOfPages()) + " ");
		pageNumber->setText(QString::number(engine->getCurrentPage()));
		this->setWindowTitle("QPDFViewer - " + fileName);
		scaleBox->setCurrentIndex(3);
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
	if (engine != NULL) {
		if (pageNumber == sender()) {
			if (!engine->setCurrentPage(pageNumber->text().toInt())) {
				QMessageBox::critical(this, "Out of bounds", "Entered value out of bounds!");
				return;
			}
		}
		else {
			bool result = false;
			if (upButton == sender() || key == Qt::Key_F2)
				result = engine->setCurrentPage(engine->getCurrentPage() + 1);
			else if (downButton == sender() || key == Qt::Key_F1)
				result = engine->setCurrentPage(engine->getCurrentPage() - 1);

			if (!result)
				return;
			else
				pageNumber->setText(QString::number(engine->getCurrentPage()));
		}
		
		scrollArea->setWidget(engine->returnImage());
	}
}

void Viewer::setAndUpdateScale()
{
	if (scaleBox->currentText().endsWith("%"))
		scaleBox->setCurrentText(scaleBox->currentText().mid(0, scaleBox->currentText().length() - 1));
	
	if (!engine->setCurrentScale(scaleBox->currentText().toInt())) {
		QMessageBox::critical(this, "Out of bounds", "Entered value out of bounds!");
		return;
	}

	scrollArea->setWidget(engine->returnImage());

	scaleBox->setCurrentText(scaleBox->currentText() + "%");
}

void Viewer::findPhrase()
{
	bool result = false;

	if (forwardsSearch == sender())
		result = engine->findPhraseInDocument(searchBox->text().toStdString(),poppler::page::search_next_result);
	else if (backwardsSearch == sender())
		result = engine->findPhraseInDocument(searchBox->text().toStdString(),poppler::page::search_previous_result);

	if (result)
		scrollArea->setWidget(engine->returnImage());
	else
		QMessageBox::warning(this, "Could not find phrase", "Could not find phrase: " + searchBox->text());
}

void Viewer::getPageText()
{
	engine->displayAllText();
}

void Viewer::showNavBar()
{
	if (navBarShowAct->isChecked()) {
		navBar = new NavigationBar(this);
		layout->insertWidget(0,navBar);
		engine->addNavOutline(navBar);
		connect(navBar, &NavigationBar::itemClicked, this, &Viewer::updatePageNavBar);
	}
	else {
		delete navBar;
	}
}

void Viewer::updatePageNavBar(const int pNum)
{
	engine->setCurrentPage(pNum);
	scrollArea->setWidget(engine->returnImage());
}


