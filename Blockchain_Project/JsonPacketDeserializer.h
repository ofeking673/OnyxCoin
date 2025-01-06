#pragma once
#include "json.hpp"
#include "Encryptions/ECDSASigner.h"
#include "Blockchain.h"

using json = nlohmann::json;

class JsonPacketDeserializer
{
public:
	static json DeserializeRequest(std::string& str);

	static bool verifySignature(json j);
};

