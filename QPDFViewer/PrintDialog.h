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

#ifndef PRINTDIALOG_H
#define PRINTDIALOG_H

#include <qdialog.h>
#include <qlist.h>
#include <QtPrintSupport/qprinterinfo.h>
#include <qcombobox.h>
#include <qspinbox.h>
#include <qlabel.h>

struct PrintTaskInfo {
	QString selectedPrinter;
	QString pageRange;
	int copies;
	bool orientation;
	bool colorMode;
};

class PrintDialog : public QDialog
{
	Q_OBJECT
public:
	PrintDialog(QWidget* parent = 0);
	PrintTaskInfo returnPrinterInfo();
	bool getOSDialog();
private:
	QList<QPrinterInfo> printers;
	QComboBox *pagesPullDown;
	PrintTaskInfo printerInfo;
	QSpinBox* copiesTextBox;
	QComboBox* colorPullDown;
	QComboBox* orientationPullDown;
	QComboBox* printerPullDown;
	bool showOSDialog;
private slots:
	void handlePagePullDown();
	void exitDialog();
	void openOSPrintDialog();
	void printPressed();
	void handleIllegalChars();
};

class ClickableLink : public QLabel {
	Q_OBJECT
public:
	explicit ClickableLink(const QString& text, QWidget* parent = nullptr) {
		setCursor(Qt::PointingHandCursor);
		setText(text);
		setStyleSheet("QLabel { color : blue; text-decoration: underline;}");
	}
signals:
	void clicked();
protected:
	void mousePressEvent(QMouseEvent* event) override {
		emit clicked();
	}

};

#endif
