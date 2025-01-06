#include "JsonPacketDeserializer.h"

json JsonPacketDeserializer::DeserializeRequest(std::string& str)
{
    json j = json::parse(str);
    if (verifySignature(j)) 
        return j;
    std::cout << __FUNCTION__ ": MESSAGE IS SIGNED INCORRECTLY!\n" << str << std::endl;
    throw new std::runtime_error("Message is signed incorrectly, Message was : " + str);
}

bool JsonPacketDeserializer::verifySignature(json j)
{
    json filter = j;
    filter.erase("signature");
    std::string jsonDump = filter.dump();
    std::cout << jsonDump << std::endl;

    Point* pnt = Point::parseHexString(j["signature"]);
    Point* pubKey = Point::parseHexString(j["src"]);
    std::string hashedMessage = SHA256::digest(jsonDump);
    std::cout << pubKey->ToString() << std::endl;

    ECDSASigner ecd;
    return ecd.verifySignature(pnt, hashedMessage, pubKey);
}
