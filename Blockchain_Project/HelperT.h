#pragma once
#include <string>
#include <ctime>
#include <iostream>
#include "Encryptions/SHA256.h"
#include "Blockchain.h"

class HelperT
{
public:

	// Safely convert time to str
	static std::string timeToStr(const time_t& time);
	static double countBalance(std::string k);
};

