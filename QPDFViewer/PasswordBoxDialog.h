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

#ifndef PASSWORDBOXDIALOG_H
#define PASSWORDBOXDIALOG_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>

class PasswordBoxDialog : public QDialog
{
public:
	PasswordBoxDialog(QWidget* parent = 0);
	QString getPassword();
private:
	QLineEdit* editBox;
	QPushButton* okButton;
	QPushButton* cancelButton;
private slots:
	void passwordEntered();
	void cancelled();
};

#endif

