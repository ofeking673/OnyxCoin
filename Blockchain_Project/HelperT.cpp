#include "HelperT.h"

std::string HelperT::timeToStr(const time_t& time)
{
	char timeBuffer[26];
	auto err = ctime_s(timeBuffer, sizeof(timeBuffer), &time);

	if (err == 0)
	{
		int len = strlen(timeBuffer);
		if (len > 0 && timeBuffer[len - 1] == '\n')
		{
			timeBuffer[len - 1] = '\0';
		}
		return timeBuffer;
	}
	else
	{
		std::cerr << "Failed to convert timestamp to string." << std::endl;
		return "0";
	}
}
