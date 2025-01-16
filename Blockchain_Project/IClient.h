#pragma once
#include <iostream>
#include "Networking/Socket.h"
#include "Wallet.h"
#include "Networking/ClientSocket.h"
class IClient
{
public:
	IClient(std::string keyPath, int port) : sock("127.0.0.1", port), _keyPath(keyPath)
	{
		if (keyPath != KEY_FILE_PATH) {
			wallet = new Wallet(keyPath);
		}
		else
		{
			wallet = new Wallet();
		}
		_k = wallet->getPrivateKey();
		pubKey = wallet->getPublicKey();
		sock.sendMsg(pubKey);
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

