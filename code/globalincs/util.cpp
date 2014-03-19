
#include "globalincs/util.h"

namespace util
{
	std::wstring charToWchar(const char* text)
	{
		size_t textSize = strlen(text);
		std::wstring outString;
		outString.resize(textSize);

		mbstowcs(&outString[0], text, textSize);

		return outString;
	}
}