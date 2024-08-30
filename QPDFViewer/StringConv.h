#ifndef STRINGCONV_H
#define STRINGCONV_H

#include <poppler/cpp/poppler-global.h>

std::string fromPopplerStringStdString(poppler::ustring popplerStr);
poppler::ustring fromStdStringToPopplerString(std::string stdString);

#endif
