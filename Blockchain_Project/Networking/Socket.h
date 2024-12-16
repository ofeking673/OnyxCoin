#pragma once
#include <WinSock2.h>
#include <thread>
#include <iostream>
#include "../json.hpp"
#include "../Blockchain.h"
//This is the base include for all socket related classes, So I'm defining everything here.
enum Requests
{
	MakeTransaction = 100,
	Mine = 200,
};

enum Response {
	MakeTransactionSuccess = 101,
	MakeTransactionFailed = 108,
	MineSuccess = 201,
	MineFailed = 208
};

using json = nlohmann::json;

#define READ_SIZE 2048
#define IFACE 0

class Socket
{
public:
	Socket(int destPort);
	void WaitForClients(void(*func)(SOCKET)) const;

	static std::string readFromSock(SOCKET sock);
private:
	SOCKET serverSocket_;
};

