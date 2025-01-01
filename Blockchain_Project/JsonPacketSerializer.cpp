#include "JsonPacketSerializer.h"

std::string JsonPacketSerializer::serializeMiningRequest(std::string srcAddr, std::string hash, int nonce)
{
    json json;
    json["status"] = Mine;
    json["hash"] = hash;
    json["src"] = getPublic(cpp_int("0x" + srcAddr).convert_to<std::string>());
    json["nonce"] = nonce;
     //self explanatory
    json["signature"] = signMessage(srcAddr, json.dump())->ToString();
    return json.dump();
}

std::string JsonPacketSerializer::serializeTransactionRequest(const std::string& src, Transaction tx)
{
    json json = tx.toJson();
    json["status"] = MakeTransaction;
    json["src"] = getPublic(cpp_int("0x" + src).convert_to<std::string>());
    ECDSASigner ecd;
    json["signature"] = signMessage(src, json.dump())->ToString();
    return json.dump();
}

std::string JsonPacketSerializer::serializeMiningResponse(bool success, int diff)
{
    json j;
    j["status"] = (success) ? MineSuccess : MineFailed;
    j["difficulty"] = diff;
    j["reward"] = (diff < 2) ? diff * 2 : (diff * 1.5); //random reward i just made up (i think its pretty good)
    //if diff is low, return a diff*2 , else give 1.5*diff
    
    return j.dump();
}

std::string JsonPacketSerializer::serializeTransactionResponse(bool success)
{
    json j;
    j["success"] = success;
    j["status"] = MakeTransactionSuccess;

    return j.dump();
}
