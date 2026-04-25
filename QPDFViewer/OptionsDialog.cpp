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

#include "OptionsDialog.h"
#include <qlabel.h>
#include <qlayout.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <QIntValidator>

OptionsDialog::OptionsDialog(QWidget* parent, bool darkMode, bool sameViewer, int cacheSize, int multithreadTime, int cacheTime)
{
	//Create options dialog
	this->setWindowTitle("Options");
	this->resize(320, 240);
	this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	darkModeBox = new QCheckBox(this);
	darkModeBox->setChecked(darkMode);
	QHBoxLayout* darkModeLayout = new QHBoxLayout;
	darkModeLayout->addWidget(new QLabel("Dark Mode",this), 1);
	darkModeLayout->addWidget(darkModeBox, 3);

	sameViewerBox = new QCheckBox(this);
	sameViewerBox->setChecked(sameViewer);
	QHBoxLayout* sameViewerLayout = new QHBoxLayout;
	sameViewerLayout->addWidget(new QLabel("Add opened PDFs to existing QPDFViewer window",this), 1);
	sameViewerLayout->addWidget(sameViewerBox, 3);

	cacheEdit = new QLineEdit(this);
	cacheEdit->setText(QString::number(cacheSize));
	cacheEdit->setValidator(new QIntValidator(1, 2000, cacheEdit));
	QHBoxLayout* cacheSizeLayout = new QHBoxLayout;
	cacheSizeLayout->addWidget(new QLabel("Cache Size",this), 1);
	cacheSizeLayout->addWidget(cacheEdit, 3);
	cacheSizeLayout->addWidget(new QLabel("MB",this), 3);

	multithreadTimeEdit = new QLineEdit(this);
	multithreadTimeEdit->setText(QString::number(multithreadTime));
	multithreadTimeEdit->setValidator(new QIntValidator(1, INT_MAX, multithreadTimeEdit));
	QHBoxLayout* multithreadTimeLayout = new QHBoxLayout;
	multithreadTimeLayout->addWidget(new QLabel("Time to trigger multithreading",this), 1);
	multithreadTimeLayout->addWidget(multithreadTimeEdit, 3);
	multithreadTimeLayout->addWidget(new QLabel("ms", this), 3);

	cacheTimeEdit = new QLineEdit(this);
	cacheTimeEdit->setText(QString::number(cacheTime));
	cacheTimeEdit->setValidator(new QIntValidator(1, INT_MAX, cacheTimeEdit));
	QHBoxLayout* cacheTimeLayout = new QHBoxLayout;
	cacheTimeLayout->addWidget(new QLabel("Time to trigger cache usage", this), 1);
	cacheTimeLayout->addWidget(cacheTimeEdit, 3);
	cacheTimeLayout->addWidget(new QLabel("ms",this), 3);

	saveExitButton = new QPushButton(this);
	saveExitButton->setText("Save and exit");
	cancelButton = new QPushButton(this);
	cancelButton->setText("Cancel");
	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addWidget(saveExitButton);
	buttonLayout->addWidget(cancelButton);

	QVBoxLayout* contentLayout = new QVBoxLayout;
	contentLayout->addLayout(darkModeLayout, 4);
	contentLayout->addLayout(sameViewerLayout, 4);
	contentLayout->addLayout(cacheSizeLayout, 4);
	contentLayout->addLayout(multithreadTimeLayout, 4);
	contentLayout->addLayout(cacheTimeLayout, 4);
	contentLayout->addLayout(buttonLayout, 4);
	contentLayout->setSpacing(20);

	QHBoxLayout* mainLayout = new QHBoxLayout;
	mainLayout->addStretch();
	mainLayout->addLayout(contentLayout);
	mainLayout->addStretch();

	QVBoxLayout* wrapper = new QVBoxLayout;
	wrapper->addStretch();
	wrapper->addLayout(mainLayout);
	wrapper->addStretch();

	this->setLayout(wrapper);

	connect(cancelButton, &QPushButton::clicked, this, &OptionsDialog::exitDialog);
	connect(saveExitButton, &QPushButton::clicked, this, &OptionsDialog::acceptDialog);

	result = { false,false,200,400,800 };
}

OptionsResult OptionsDialog::getResult()
{
	return result;
}

void OptionsDialog::acceptDialog()
{
	result.darkMode = darkModeBox->isChecked();
	result.sameViewer = sameViewerBox->isChecked();
	result.cacheSize = cacheEdit->text().toInt();
	result.multithreadTime = multithreadTimeEdit->text().toInt();
	result.cacheTime = cacheTimeEdit->text().toInt();
	this->accept();
}

void OptionsDialog::exitDialog()
{
	this->reject();
}