#pragma once
#include <iostream>
#include "Networking/Socket.h"
class IClient
{
public:
	IClient(std::string key, int port) : _k(key), sock(port) {}
	std::string _k;
	Socket sock;
};

