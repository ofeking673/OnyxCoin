#pragma once
#include <iostream>
#include "Networking/Socket.h"
#include "Wallet.h"
#include "Networking/ClientSocket.h"
class IClient
{
public:
	IClient(std::string keyPath, int port, int srcPort=0) : sock("127.0.0.1", port), _keyPath(keyPath)
	{
		if (keyPath != KEY_FILE_PATH) {
			wallet = new Wallet(keyPath);
		}
		else
		{
			wallet = new Wallet();
		}

		pubKey = wallet->getPublicKey();
		if (port == 4444 && srcPort != 0) {
			sock.sendMsg(pubKey + "|" + std::to_string(srcPort));
		}
		else {
			sock.sendMsg(pubKey);
		}
	}

	uint64_t displayBalance() {
		return wallet->getBalance();
	}

	~IClient() {
		wallet->saveWalletData(_keyPath);
	}

	std::string _k;
	std::string pubKey;
	ClientSocket sock;
	Wallet* wallet;
	std::string _keyPath;
};

