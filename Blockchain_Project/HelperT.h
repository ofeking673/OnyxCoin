#pragma once
#include <string>
#include <ctime>
#include <iostream>

class HelperT
{
public:

	// Safely convert time to str
	static std::string timeToStr(const time_t& time);
};

