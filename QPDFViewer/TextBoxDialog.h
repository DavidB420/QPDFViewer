#ifndef TEXTBOXDIALOG_H
#define TEXTBOXDIALOG_H

#include <qdialog.h>
#include <qplaintextedit.h>
#include <string>

class TextBoxDialog : public QDialog
{
public:
	TextBoxDialog(QWidget* parent = 0, std::string *txt = 0);
private:
	QPlainTextEdit* tBox;
private slots:
	void exitDialog();
	void copyAllClipboard();
};

#endif
