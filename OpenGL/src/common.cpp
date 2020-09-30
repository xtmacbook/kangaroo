#include "common.h"
#include "log.h"
#include "sys.h"
#include <sstream>

float stringToFloat(const std::string &source)
{
	std::stringstream ss(source.c_str());
	float result;
	ss >> result;
	return result;
}

unsigned int stringToUint(const std::string &source)
{
	std::stringstream ss(source.c_str());
	unsigned int result;
	ss >> result;
	return result;
}

int stringToInt(const std::string &source)
{
	std::stringstream ss(source.c_str());
	int result;
	ss >> result;
	return result;
}

const char * extension(const char * str)
{
	int length, l;
	l = length = (int)strlen(str);
	while (length > 0 && str[length] != '.') {
		length--;
		if (str[length] == '\\' || str[length] == '/') {
			return &str[l]; // no extension
		}
	}
	if (length == 0) {
		return &str[l];
	}
	return &str[length];
}

int strCaseDiff(const char * s1, const char * s2)
{
#ifdef EN_OS_WIN32
	return _stricmp(s1, s2);
#else
	return strcasecmp(s1, s2);
#endif
}


