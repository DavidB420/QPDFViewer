#include "TextBoxDialog.h"

#include <qplaintextedit.h>
#include <qpushbutton.h>
#include <qlayout.h>

TextBoxDialog::TextBoxDialog(QWidget* parent, std::string *txt)
{
	this->setWindowTitle("Page Text");
	this->resize(640, 480);
	this->setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	QVBoxLayout* layout = new QVBoxLayout;
	
	tBox = new QPlainTextEdit(this);
	tBox->setPlainText(QString::fromStdString(*txt));
	tBox->setReadOnly(true);
	
	QPushButton* okButton = new QPushButton(this);
	okButton->setText("Ok");
	connect(okButton, &QPushButton::clicked, this, &TextBoxDialog::exitDialog);
	QPushButton* copyAllButton = new QPushButton(this);
	copyAllButton->setText("Copy All");
	connect(copyAllButton, &QPushButton::clicked, this, &TextBoxDialog::copyAllClipboard);

	QHBoxLayout* buttonLayout = new QHBoxLayout;
	buttonLayout->addStretch();
	buttonLayout->addWidget(okButton);
	buttonLayout->addStretch();
	buttonLayout->addWidget(copyAllButton);
	buttonLayout->addStretch();

	layout->addWidget(tBox);
	layout->addLayout(buttonLayout);

	this->setLayout(layout);
}

void TextBoxDialog::copyAllClipboard()
{
	tBox->selectAll();
	tBox->copy();
}

void TextBoxDialog::exitDialog()
{
	this->accept();
}

