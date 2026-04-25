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

#ifndef OPTIONSPARSER_H
#define OPTIONSPARSER_H

#include <qstring.h>
#include <qpalette.h>

class OptionsParser
{
public:
	OptionsParser();
	void loadFromFile();
	void saveToFile();
	bool returnDarkMode();
	bool returnSameViewer();
	int returnCacheSize();
	int returnMultithreadTime();
	int returnCacheTime();
	void setValues(bool darkMode, bool sameViewer, int cacheSize, int multithreadTime, int cacheTime);
private:
	bool darkMode;
	bool sameViewer;
	int cacheSize;
	int multithreadTime;
	int cacheTime;
	QString defaultStyle;
	QPalette defaultPalette;
	void changeTheme();
};

#endif