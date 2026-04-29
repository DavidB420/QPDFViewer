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

#include "OptionsParser.h"
#include <QTextStream>
#include <QFile>
#include <qcoreapplication.h>
#include <qapplication.h>
#include <qpalette.h>
#include <qstyle.h>
#include <qstandardpaths.h>
#include <qdir.h>
#include "VersionNumber.h"

OptionsParser::OptionsParser()
{
	//Initialize with default values 
	darkMode = false;
	sameViewer = false;
	cacheSize = 200;
	multithreadTime = 400;
	cacheTime = 800;

	defaultStyle = qApp->style()->objectName();
	defaultPalette = qApp->palette();
}

void OptionsParser::loadFromFile()
{
	//Load from the config file if it exists, otherwise intiialize the file with default values
	QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
	QDir().mkpath(dir);
	QString path = dir + "/config.cfg";
	QFile file(path);

	if (file.exists()) {
		file.open(QIODevice::ReadOnly);
		QTextStream in(&file);

		while (!in.atEnd()) {
			QString line = in.readLine();

			if (line.isEmpty() || line.startsWith("#"))
				continue;

			QStringList parts = line.split("=");

			if (parts.size() != 2)
				continue;

			QString key = parts[0].trimmed();
			QString value = parts[1].trimmed();

			if ((key == "QPDFViewer_MajorVersion" && value.toInt() != MAJOR_VERSION) || (key == "QPDFViewer_MinorVersion" && value.toInt() != MINOR_VERSION)) {
				file.close();
				saveToFile();
				return;
			}
			else if (key == "darkMode") {
				darkMode = value.toInt();
				changeTheme();
			}
			else if (key == "sameViewer")
				sameViewer = value.toInt();
			else if (key == "cacheSize")
				cacheSize = value.toInt();
			else if (key == "multithreadTime")
				multithreadTime = value.toInt();
			else if (key == "cacheTime")
				cacheTime = value.toInt();
		}
		file.close();
	}
	else
		saveToFile();
}

void OptionsParser::saveToFile()
{
	//Save config file with update values and update theme if necessary 
	QString dir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
	QDir().mkpath(dir);
	QString path = dir + "/config.cfg";
	QFile file(path);

	if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
		changeTheme();
		QTextStream out(&file);
		out << "QPDFViewer_MajorVersion=" << MAJOR_VERSION << "\n";
		out << "QPDFViewer_MinorVersion=" << MINOR_VERSION << "\n";
		out << "darkMode=" << darkMode << "\n";
		out << "sameViewer=" << sameViewer << "\n";
		out << "cacheSize=" << cacheSize << "\n";
		out << "multithreadTime=" << multithreadTime << "\n";
		out << "cacheTime=" << cacheTime << "\n";
		file.close();
	}
}

bool OptionsParser::returnDarkMode()
{
	return darkMode;
}

bool OptionsParser::returnSameViewer()
{
	return sameViewer;
}

int OptionsParser::returnCacheSize()
{
	return cacheSize;
}

int OptionsParser::returnMultithreadTime()
{
	return multithreadTime;
}

int OptionsParser::returnCacheTime()
{
	return cacheTime;
}

void OptionsParser::setValues(bool darkMode, bool sameViewer, int cacheSize, int multithreadTime, int cacheTime)
{
	this->darkMode = darkMode;
	this->sameViewer = sameViewer;
	this->cacheSize = cacheSize;
	this->multithreadTime = multithreadTime;
	this->cacheTime = cacheTime;
}

void OptionsParser::changeTheme()
{
	//Apply custom dark theme, or restore default light theme
	QApplication* a = static_cast<QApplication*>(QApplication::instance());

	if (darkMode) {
		a->setStyle("Fusion");
		NoMnemonicStyle *style = new NoMnemonicStyle(a->style());
		a->setStyle(style);
		QPalette dark;
		dark.setColor(QPalette::Window, QColor(37, 37, 38));
		dark.setColor(QPalette::WindowText, QColor(220, 220, 220));
		dark.setColor(QPalette::Base, QColor(30, 30, 30));
		dark.setColor(QPalette::AlternateBase, QColor(45, 45, 45));
		dark.setColor(QPalette::Text, QColor(220, 220, 220));
		dark.setColor(QPalette::Button, QColor(45, 45, 45));
		dark.setColor(QPalette::ButtonText, QColor(220, 220, 220));
		dark.setColor(QPalette::Highlight, QColor(0, 120, 215));
		dark.setColor(QPalette::HighlightedText, Qt::white);
		QColor disabledText(120, 120, 120);
		dark.setColor(QPalette::Disabled, QPalette::Text, disabledText);
		dark.setColor(QPalette::Disabled, QPalette::ButtonText, disabledText);
		dark.setColor(QPalette::Disabled, QPalette::WindowText, disabledText);
		a->setPalette(dark);
	}
	else {
		a->setStyle(defaultStyle);
		a->setPalette(defaultPalette);
		a->setStyleSheet("");
	}
}
