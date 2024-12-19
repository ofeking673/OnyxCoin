#include "JsonPacketSerializer.h"

std::string JsonPacketSerializer::serializeMiningRequest(std::string srcAddr, std::string hash, int nonce)
{
    json json;
    json["hash"] = hash;
    json["src"] = srcAddr;
    json["nonce"] = nonce;
     //self explanatory
    json["signature"] = signMessage(srcAddr, json.dump())->ToString();
    return json.dump();
}

std::string JsonPacketSerializer::serializeTransactionRequest(std::string srcAddr, std::string dstAddr, double amt)
{
    json json;
    KeyGenerator key;
    
    Point* pnt = key.ECMul(cpp_int(srcAddr), key.GPoint);
    json["src"] = pnt->ToString(); //source address
    delete pnt;

    json["dst"] = dstAddr; //destination address
    json["amt"] = amt; //amount

    //sign and add signature to the message
    json["signature"] = signMessage(srcAddr, json.dump())->ToString();

    return json.dump();
}
