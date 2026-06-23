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

#include "TextBoxDialog.h"

#include <qplaintextedit.h>
#include <qpushbutton.h>
#include <qlayout.h>

TextBoxDialog::TextBoxDialog(QWidget* parent, bool unwrappedCopy, std::string *txt)
{
	//Create window that shows the selected text with an ok and copy all button
	this->setWindowTitle("Page Text");
	this->resize(640, 480);
	this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	QVBoxLayout* layout = new QVBoxLayout;
	
	tBox = new QPlainTextEdit(this);
	tBox->setPlainText(QString::fromStdString(*txt));
	tBox->setReadOnly(true);
	tBox->installEventFilter(this);
	
	QPushButton* okButton = new QPushButton(this);
	okButton->setText("Ok");
	connect(okButton, &QPushButton::clicked, this, &TextBoxDialog::exitDialog);
	unwrappedChkBx = new QCheckBox(this);
	unwrappedChkBx->setText("Unwrapped view");
	connect(unwrappedChkBx, &QCheckBox::clicked, this, &TextBoxDialog::toggleUnwrapped);
	QPushButton* copyAllButton = new QPushButton(this);
	copyAllButton->setText("Copy All");
	connect(copyAllButton, &QPushButton::clicked, this, &TextBoxDialog::copyAllClipboard);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(okButton);
	buttonLayout->addStretch();
	buttonLayout->addWidget(unwrappedChkBx);
	buttonLayout->addStretch();
	buttonLayout->addWidget(copyAllButton);
	buttonLayout->addStretch();

	layout->addWidget(tBox);
	layout->addLayout(buttonLayout);

	this->setLayout(layout);

	this->txt = tBox->toPlainText();
	this->unwrappedCopyEnable = unwrappedCopy;
}

QString TextBoxDialog::generateUnwrapped()
{	
	return txt.replace('\n',' ');
}

void TextBoxDialog::wrappedCopy()
{
	unwrappedChkBx->setChecked(false);
	toggleUnwrapped();
	copyAllClipboard();
}

void TextBoxDialog::unwrappedCopy()
{
	unwrappedChkBx->setChecked(true);
	toggleUnwrapped();
	copyAllClipboard();
}

bool TextBoxDialog::eventFilter(QObject* obj, QEvent* event)
{
	if (obj == tBox && event->type() == QEvent::KeyPress) {
		QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
		if (keyEvent->matches(QKeySequence::Copy)) {
			if (unwrappedCopyEnable) unwrappedCopy();
			else wrappedCopy();
			return true;
		}
		else if (keyEvent->key() == Qt::Key_C && keyEvent->modifiers() == (Qt::ControlModifier | Qt::ShiftModifier)) {
			if (unwrappedCopyEnable) wrappedCopy();
			else unwrappedCopy();
			return true;
		}
	}
	return QDialog::eventFilter(obj, event);
}

void TextBoxDialog::copyAllClipboard()
{
	//Select and copy everything to clipboard
	tBox->selectAll();
	tBox->copy();
}

void TextBoxDialog::toggleUnwrapped()
{
	if (unwrappedChkBx->isChecked())
		tBox->setPlainText(generateUnwrapped());
	else
		tBox->setPlainText(txt);
}

void TextBoxDialog::exitDialog()
{
	//Close this dialog
	this->accept();
}

