#include "StringConv.h"

std::string fromPopplerStringStdString(poppler::ustring popplerStr)
{
	poppler::byte_array utf8Arr = popplerStr.to_utf8();

	std::string returnedString = "";
	
	for (int i = 0; i < utf8Arr.size(); i++)
		returnedString.insert(i,1, utf8Arr.at(i));

	return returnedString;
}

poppler::ustring fromStdStringToPopplerString(std::string stdString)
{
	return poppler::ustring::from_utf8(stdString.c_str(), stdString.length());
}
