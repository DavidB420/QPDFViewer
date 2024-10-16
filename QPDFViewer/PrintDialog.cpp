#include "PrintDialog.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <qprinterinfo.h>
#include <qlist.h>
#include <QResizeEvent>

PrintDialog::PrintDialog(QWidget* parent)
{
	//Create print dialog
	this->setWindowTitle("Print");
	this->resize(640, 480);
	this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	QLabel* printerLabel = new QLabel(this);
	printerLabel->setText("Printer");
	QComboBox* printerPullDown = new QComboBox(this);
	QHBoxLayout* printerLayout = new QHBoxLayout;
	printerLayout->addWidget(printerLabel,1);
	printerLayout->addWidget(printerPullDown,3);
	printers = QPrinterInfo::availablePrinters();
	for (const QPrinterInfo printer : printers)
		printerPullDown->addItem(printer.printerName());


	QLabel* orientationLabel = new QLabel(this);
	orientationLabel->setText("Orientation");
	QComboBox* orientationPullDown = new QComboBox(this);
	QHBoxLayout* orientationLayout = new QHBoxLayout;
	orientationLayout->addWidget(orientationLabel,1);
	orientationLayout->addWidget(orientationPullDown,3);
	orientationPullDown->addItem("Landscape");
	orientationPullDown->addItem("Vertical");
	orientationPullDown->addItem("Landscape (reversed)");
	orientationPullDown->addItem("Vertical (reversed)");

	QLabel* copiesLabel = new QLabel(this);
	copiesLabel->setText("Copies");
	QSpinBox* copiesTextBox = new QSpinBox(this);
	copiesTextBox->setMinimum(1);
	copiesTextBox->setMaximum(INT_MAX);
	QHBoxLayout* copiesLayout = new QHBoxLayout;
	copiesLayout->addWidget(copiesLabel,1);
	copiesLayout->addWidget(copiesTextBox,3);

	QLabel* colorLabel = new QLabel(this);
	colorLabel->setText("Color mode");
	QComboBox* colorPullDown = new QComboBox(this);
	QHBoxLayout* colorLayout = new QHBoxLayout;
	colorLayout->addWidget(colorLabel,1);
	colorLayout->addWidget(colorPullDown,3);
	colorPullDown->addItem("Color");
	colorPullDown->addItem("Black and white");

	QLabel* pagesLabel = new QLabel(this);
	pagesLabel->setText("Pages");
	pagesPullDown = new QComboBox(this);
	QHBoxLayout* pagesLayout = new QHBoxLayout;
	pagesLayout->addWidget(pagesLabel,1);
	pagesLayout->addWidget(pagesPullDown,3);
	pagesPullDown->addItem("All pages");
	pagesPullDown->addItem("");
	connect(pagesPullDown, &QComboBox::currentTextChanged, this, &PrintDialog::handlePagePullDown);

	ClickableLink* osDialogLink = new ClickableLink("Print using OS dialog",this);
	osDialogLink->setMaximumHeight(20);
	osDialogLink->setCursor(Qt::PointingHandCursor);
	QHBoxLayout* osDialogLayout = new QHBoxLayout;
	osDialogLayout->addWidget(osDialogLink,0,Qt::AlignHCenter);
	connect(osDialogLink, &ClickableLink::clicked, this, &PrintDialog::exitDialog);

	QPushButton* printButton = new QPushButton(this);
	printButton->setText("Print");
	QPushButton* cancelButton = new QPushButton(this);
	cancelButton->setText("Cancel");
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(printButton);
	buttonLayout->addWidget(cancelButton);
	connect(cancelButton, &QPushButton::clicked, this, &PrintDialog::exitDialog);


	QVBoxLayout* layout = new QVBoxLayout;
	layout->addLayout(printerLayout,4);
	layout->addLayout(orientationLayout,4);
	layout->addLayout(copiesLayout,4);
	layout->addLayout(colorLayout,4);
	layout->addLayout(pagesLayout,4);
	layout->addLayout(osDialogLayout);
	layout->addLayout(buttonLayout,4);
	layout->setSpacing(20);


	this->setLayout(layout);
}

void PrintDialog::exitDialog()
{
	this->reject();
}

void PrintDialog::handlePagePullDown()
{
	if (pagesPullDown->currentIndex() == 0)
		pagesPullDown->setEditable(false);
	else
		pagesPullDown->setEditable(true);
}