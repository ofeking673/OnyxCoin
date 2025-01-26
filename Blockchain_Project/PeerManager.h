#pragma once
#include "map"
#include "Client.h"
#include "Server.h"

class PeerManager
{
public:
	PeerManager(IMessageHandler* handler, std::string keyPath, int port) : _cli(4444, keyPath, port), _serv(&_cli, port, handler) {
		discoverPeers(_cli.sock.recvMsg());
	};

	//discoverPeers(_cli.sock.sendAndRecv(_cli.wallet->getPublicKey()));
	void addPeer(std::string ip, int port);
	void discoverPeers(std::string msg);

	/// Add peer if this client connects to your server, so connection is already alive, and should be saved.
	void addPeer(const std::string& pubkey, SOCKET sock);

	void handleSocket(SOCKET sock);

	void sendMessage(std::string dst, std::string msg);
	std::string recvMessage(std::string src);

	// Utility
	std::string getPubKey() { return _cli.pubKey; }
	std::vector<SOCKET> getAllClients();
	std::vector<std::string> getAllPublicKeys();
	std::string signMessage(std::string msg);
private:
	// Map the public key, and the boolean value for Alive to the peer's socket
	std::map<std::string, std::pair<SOCKET, bool>> peers;
	// To ignore our own personal port and ip.. Theres no use for this ;)
	std::pair<std::string, int> _personalSocketDetails;
	Client _cli;
	Server _serv;
};

