#pragma once
#include <string>
#include <ctime>
#include <iostream>
#include "Encryptions/SHA256.h"

class HelperT
{
public:

	// Safely convert time to str
	static std::string timeToStr(const time_t& time);
	static std::string padString(std::string src, int length);

    // Helper function to split strings
	static std::vector<std::string> split(const std::string& s, char delim);
};

