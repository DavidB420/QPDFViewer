#include "Viewer.h"
#include <qmenubar.h>

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
	QAction* exitAct = new QAction(tr("&Exit"), this);
	fileMenu->addAction(exitAct);
	connect(exitAct, &QAction::triggered, this, &Viewer::exitApp);
	QAction* aboutAct = new QAction(tr("&About"), this);
	aboutmenu->addAction(aboutAct);
	connect(aboutAct, &QAction::triggered, this, &Viewer::aboutApp);
}

Viewer::~Viewer()
{
}

void Viewer::openFile()
{

}

void Viewer::exitApp()
{
}

void Viewer::aboutApp()
{
}


