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

double HelperT::countBalance(std::string k)
{
	double bal = 0;
	Blockchain* chain = Blockchain::getInstance();
	for (auto& block : chain->getChain())
	{
		for (auto& trans : block._transactions) {
			double amt = trans.getAmount();
			if (trans.getSender() == k) {
				bal -= amt;
			}
			if (trans.getReceiver() == k) {
				bal += amt;
			}
		}
	}
	return bal;
}
