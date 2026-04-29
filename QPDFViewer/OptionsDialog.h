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

#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <qdialog.h>
#include <qcheckbox.h>
#include <qlineedit.h>

struct OptionsResult {
	bool darkMode;
	bool sameViewer;
	int cacheSize;
	int multithreadTime;
	int cacheTime;
};
class OptionsDialog : public QDialog
{
	Q_OBJECT
public:
	OptionsDialog(QWidget* parent = 0, bool darkMode=false, bool sameViewer=false, int cacheSize=200, int multithreadTime=400, int cacheTime=800);
	OptionsResult getResult();
private:
	QCheckBox* darkModeBox;
	QCheckBox* sameViewerBox;
	QLineEdit* cacheEdit;
	QLineEdit* multithreadTimeEdit;
	QLineEdit* cacheTimeEdit;
	QPushButton* saveExitButton;
	QPushButton* cancelButton;
	OptionsResult result;
private slots:
	void exitDialog();
	void acceptDialog();

};

#endif
