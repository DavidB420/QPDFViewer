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

#include "PrintDialog.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qpushbutton.h>
#include <QtPrintSupport/qprinterinfo.h>
#include <qlineedit.h>
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
	printerPullDown = new QComboBox(this);
	QHBoxLayout* printerLayout = new QHBoxLayout;
	printerLayout->addWidget(printerLabel,1);
	printerLayout->addWidget(printerPullDown,3);
	printers = QPrinterInfo::availablePrinters();
	for (const QPrinterInfo printer : printers)
		printerPullDown->addItem(printer.printerName());


	QLabel* orientationLabel = new QLabel(this);
	orientationLabel->setText("Orientation");
	orientationPullDown = new QComboBox(this);
	QHBoxLayout* orientationLayout = new QHBoxLayout;
	orientationLayout->addWidget(orientationLabel,1);
	orientationLayout->addWidget(orientationPullDown,3);
	orientationPullDown->addItem("Portrait");
	orientationPullDown->addItem("Landscape");

	QLabel* copiesLabel = new QLabel(this);
	copiesLabel->setText("Copies");
	copiesTextBox = new QSpinBox(this);
	copiesTextBox->setMinimum(1);
	copiesTextBox->setMaximum(INT_MAX);
	QHBoxLayout* copiesLayout = new QHBoxLayout;
	copiesLayout->addWidget(copiesLabel,1);
	copiesLayout->addWidget(copiesTextBox,3);

	QLabel* colorLabel = new QLabel(this);
	colorLabel->setText("Color mode");
	colorPullDown = new QComboBox(this);
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
	connect(pagesPullDown, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &PrintDialog::handlePagePullDown);
	connect(pagesPullDown, &QComboBox::currentTextChanged, this, &PrintDialog::handleIllegalChars);

	ClickableLink* osDialogLink = new ClickableLink("Print using OS dialog",this);
	osDialogLink->setMaximumHeight(20);
	osDialogLink->setCursor(Qt::PointingHandCursor);
	QHBoxLayout* osDialogLayout = new QHBoxLayout;
	osDialogLayout->addWidget(osDialogLink,0,Qt::AlignHCenter);
	connect(osDialogLink, &ClickableLink::clicked, this, &PrintDialog::openOSPrintDialog);

	QPushButton* printButton = new QPushButton(this);
	printButton->setText("Print");
	QPushButton* cancelButton = new QPushButton(this);
	cancelButton->setText("Cancel");
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(printButton);
	buttonLayout->addWidget(cancelButton);
	connect(printButton, &QPushButton::clicked, this, &PrintDialog::printPressed);
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

	showOSDialog = false;
}

PrintTaskInfo PrintDialog::returnPrinterInfo()
{
	return printerInfo;
}

bool PrintDialog::getOSDialog()
{
	return showOSDialog;
}

void PrintDialog::exitDialog()
{
	this->reject();
}

void PrintDialog::openOSPrintDialog()
{
	//Tell Viewer to show the OS's native print dialog instead
	showOSDialog = true;
	this->accept();
}

void PrintDialog::printPressed()
{
	//Save user choices in the printer info structure then return back to Viewer's print function
	printerInfo.copies = copiesTextBox->value();
	printerInfo.colorMode = colorPullDown->currentIndex() > 0 ? false : true;
	printerInfo.orientation = orientationPullDown->currentIndex();
	for (const QPrinterInfo printer : printers) {
		if (!QString::compare(printer.printerName(), printerPullDown->itemText(printerPullDown->currentIndex()), Qt::CaseSensitive))
			printerInfo.selectedPrinter = printer.printerName();
	}
	printerInfo.pageRange = pagesPullDown->currentIndex() == 0 ? pagesPullDown->itemText(pagesPullDown->currentIndex()) : pagesPullDown->lineEdit()->text();
	this->accept();
}

void PrintDialog::handleIllegalChars()
{
	//Remove any character entered that is not numerical or a comma or hyphen
	if (pagesPullDown->isEditable()) {
		QString pagesStr = pagesPullDown->lineEdit()->text();
		if (pagesStr.length() > 0) {
			QChar currentCharacter = pagesStr.at(pagesStr.length() - 1);
			if ((currentCharacter < '0' || currentCharacter > '9') && (currentCharacter < ',' || currentCharacter > '-')) {
				pagesStr.truncate(pagesStr.length() - 1);
				pagesPullDown->blockSignals(true);
				pagesPullDown->lineEdit()->setText(pagesStr);
				pagesPullDown->blockSignals(false);
			}
		}
	}
}

void PrintDialog::handlePagePullDown()
{
	//Only allow editing for the second option for page selection
	if (pagesPullDown->currentIndex() == 0)
		pagesPullDown->setEditable(false);
	else
		pagesPullDown->setEditable(true);
}