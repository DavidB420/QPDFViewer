/**
 * Copyright 2025 David Badiei
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

#include "StringConv.h"

std::string fromPopplerStringStdString(poppler::ustring popplerStr)
{
	//Convert poppler::ustring to std::string
	poppler::byte_array utf8Arr = popplerStr.to_utf8();

	std::string returnedString = "";
	
	for (int i = 0; i < utf8Arr.size(); i++)
		returnedString.insert(i,1, utf8Arr.at(i));

	return returnedString;
}

poppler::ustring fromStdStringToPopplerString(std::string stdString)
{
	//Convert std::string to poppler::ustring
	return poppler::ustring::from_utf8(stdString.c_str(), stdString.length());
}
