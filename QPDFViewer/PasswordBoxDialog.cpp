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

#include "PasswordBoxDialog.h"

#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>

PasswordBoxDialog::PasswordBoxDialog(QWidget* parent)
{
	//Dialog for users to enter password
	this->resize(500, 150);

	this->setWindowTitle("Protected document");
	this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	QVBoxLayout* layout = new QVBoxLayout(this);

	QHBoxLayout *infoLayout = new QHBoxLayout(this);
	layout->addLayout(infoLayout);
	QLabel* lbl = new QLabel(this);
	lbl->setPixmap(QPixmap(":/images/assets/encrypted.png"));
	infoLayout->addWidget(lbl);
	QLabel* txt = new QLabel(this);
	txt->setText("This document is encrypted.\nPlease enter the necessary password to continue loading the document or close this dialog to cancel loading.");
	infoLayout->addWidget(txt);
	infoLayout->setAlignment(Qt::AlignLeft);


	QHBoxLayout* passwordLayout = new QHBoxLayout(this);
	layout->addLayout(passwordLayout);
	QLabel* passwordTxt = new QLabel(this);
	passwordTxt->setText("Password:");
	passwordLayout->addWidget(passwordTxt);
	editBox = new QLineEdit(this);
	editBox->setEchoMode(QLineEdit::Password);
	passwordLayout->addWidget(editBox);

	QHBoxLayout* buttonLayout = new QHBoxLayout(this);
	layout->addLayout(buttonLayout);
	okButton = new QPushButton(this);
	okButton->setText("Ok");
	connect(okButton, &QPushButton::clicked, this, &PasswordBoxDialog::passwordEntered);
	buttonLayout->addWidget(okButton);
	cancelButton = new QPushButton(this);
	cancelButton->setText("Cancel");
	buttonLayout->addWidget(cancelButton);
	connect(cancelButton, &QPushButton::clicked, this, &PasswordBoxDialog::cancelled);


	this->setLayout(layout);
}

QString PasswordBoxDialog::getPassword()
{
	return editBox->text();
}

void PasswordBoxDialog::passwordEntered()
{
	this->accept();
}

void PasswordBoxDialog::cancelled()
{
	this->reject();
}
