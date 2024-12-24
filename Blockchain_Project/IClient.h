#pragma once
#include <iostream>
#include "Networking/Socket.h"
#include "Networking/ClientSocket.h"
class IClient
{
public:
	IClient(std::string key, int port) : _k(key), sock(port) {}
	std::string _k;
	ClientSocket sock;
};

