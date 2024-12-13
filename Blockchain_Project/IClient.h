#pragma once
class IClient
{
public:
	virtual void Action() = 0; //For client it will be transaction, for miner it will be submit hash
	std::string k;
};

