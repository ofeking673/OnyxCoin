#pragma once
#include "json.hpp"
#include "Encryptions/ECDSASigner.h"
#include "Blockchain.h"

class JsonPacketDeserializer
{
public:
	static nlohmann::json DeserializeRequest(std::string& str);

	static bool verifySignature(nlohmann::json j);
};

