#pragma once
#include "Networking/ClientSocket.h"
#include "map"
#include "Client.h"
#include "Server.h"

class PeerManager
{
public:
	PeerManager(std::string keyPath) : _cli(4444, keyPath), _serv(&_cli) {
		discoverPeers(_cli.sock.sendAndRecv(_cli.pubKey));
	};

	//discoverPeers(_cli.sock.sendAndRecv(_cli.wallet->getPublicKey()));
	void addPeer(std::string ip, int port);
	void discoverPeers(std::string msg);

	void sendMessage(std::string dst, std::string msg);
	std::string recvMessage(std::string src);

	std::string getPubKey() { return _cli.pubKey; }
private:
	// Map the public key, and the boolean value for Alive to the peer's socket
	std::map<std::pair<std::string, bool>, SOCKET> peers;
	// To ignore our own personal port and ip.. Theres no use for this ;)
	std::pair<std::string, int> _personalSocketDetails;
	Client _cli;
	Server _serv;
};

