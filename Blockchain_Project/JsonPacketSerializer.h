#pragma once
#include "JsonPacketDeserializer.h"
#include "Blockchain.h"
#include "Encryptions/ECDSASigner.h"

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

class JsonPacketSerializer
{
public:
	static std::string serializeMiningRequest(std::string srcAddr, std::string hash, int nonce);
	static std::string serializeTransactionRequest(std::string srcAddr, std::string dstAddr, double amt);


	static std::string serializeMiningResponse(bool success, int diff);
private:
	static Point* signMessage(std::string key, std::string message) {
		ECDSASigner ecd;
		return ecd.signMessage(cpp_int(key), message);
	}
};

