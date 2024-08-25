#include "Viewer.h"
#include <qmenubar.h>
#include <qapplication.h>
#include <qmessagebox.h>
#include <qtoolbar.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qfiledialog.h>
#include <memory>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-image.h>
#include <poppler/cpp/poppler-page-renderer.h>
#include <cstdio>

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
	QLineEdit* pageNumber = new QLineEdit(this);
	pageNumber->setMaximumWidth(50);
	pageNumber->setFixedHeight(20);
	pageNumber->setAlignment(Qt::AlignCenter);
	toolBar->addWidget(pageNumber);
	QPushButton* upButton = new QPushButton(this);
	upButton->setText(QString::fromUtf8(u8"▲"));
	upButton->setFixedWidth(45);
	toolBar->addWidget(upButton);
	QPushButton* downButton = new QPushButton(this);
	downButton->setText(QString::fromUtf8(u8"▼"));
	downButton->setFixedWidth(45);
	toolBar->addWidget(downButton);
	QLabel* totalPage = new QLabel(this);
	totalPage->setText(" of ");
	toolBar->addWidget(totalPage);
	QLabel* scaleLbl = new QLabel(this);
	scaleLbl->setText("Scale Document:");
	toolBar->addWidget(scaleLbl);
	QLineEdit* searchBox = new QLineEdit(this);
	searchBox->setFixedHeight(20);
	toolBar->addWidget(searchBox);
	QPushButton* backwardsSearch = new QPushButton(this);
	backwardsSearch->setText("Look Backwards");
	toolBar->addWidget(backwardsSearch);
	QPushButton* forwardsSearch = new QPushButton(this);
	forwardsSearch->setText("Look Forwards");
	toolBar->addWidget(forwardsSearch);
}

Viewer::~Viewer()
{
}

void Viewer::openFile()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Open PDF file"), NULL, tr("PDF Files (*.pdf)"));

	if (fileName != NULL) {
		poppler::document* doc = poppler::document::load_from_file(fileName.toStdString());
		poppler::page *testPage = doc->create_page(0);
		poppler::page_renderer pr;

		poppler::image img = pr.render_page(testPage,
			(float)72 * 100 / 100, (float)72 * 100 / 100,
			-1, -1, -1, -1, poppler::rotate_0);

		unsigned char* test = (unsigned char*)img.data();
		for (int i = 0; i < img.height() * img.width(); i++)
			printf(" %02x ", test[i]);
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


