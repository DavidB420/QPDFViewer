﻿#include "Viewer.h"
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
	QMenu* aboutmenu = new QMenu(this);
	aboutmenu = mBar->addMenu(tr("&About"));
	QAction* openAct = new QAction(tr("&Open..."), this);
	connect(openAct, &QAction::triggered, this, &Viewer::openFile);
	fileMenu->addAction(openAct);
	QAction* printAct = new QAction(tr("&Print"), this);
	fileMenu->addAction(printAct);
	fileMenu->addSeparator();
	QAction* exitAct = new QAction(tr("&Exit"), this);
	fileMenu->addAction(exitAct);
	connect(exitAct, &QAction::triggered, this, &Viewer::exitApp);
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
	connect(pageNumber, &QLineEdit::returnPressed, this, &Viewer::setAndUpdatePage);
	toolBar->addWidget(pageNumber);
	upButton = new QPushButton(this);
	upButton->setText(QString::fromUtf8(u8"▲"));
	upButton->setFixedWidth(45);
	toolBar->addWidget(upButton);
	connect(upButton, &QPushButton::clicked, this, &Viewer::setAndUpdatePage);
	downButton = new QPushButton(this);
	downButton->setText(QString::fromUtf8(u8"▼"));
	downButton->setFixedWidth(45);
	connect(downButton, &QPushButton::clicked, this, &Viewer::setAndUpdatePage);
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
	QLineEdit* searchBox = new QLineEdit(this);
	searchBox->setFixedHeight(20);
	toolBar->addWidget(searchBox);
	QPushButton* backwardsSearch = new QPushButton(this);
	backwardsSearch->setText("Look Backwards");
	toolBar->addWidget(backwardsSearch);
	QPushButton* forwardsSearch = new QPushButton(this);
	forwardsSearch->setText("Look Forwards");
	toolBar->addWidget(forwardsSearch);

	//Initialize scroll area
	scrollArea = new QScrollArea(this);
	scrollArea->setBackgroundRole(QPalette::Mid);
	scrollArea->setAlignment(Qt::AlignCenter);
	setCentralWidget(scrollArea);
}

Viewer::~Viewer()
{
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
		this->setWindowTitle(this->windowTitle() + " - " + fileName);
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
		tr("<b>QPDFViewer 1.0</b><br>Written by David Badiei, 2024\nLicensed under GNU General Public License v3 (GPL-3)"));
}

void Viewer::setAndUpdatePage()
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
			if (upButton == sender())
				result = engine->setCurrentPage(engine->getCurrentPage() + 1);
			else if (downButton == sender())
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


