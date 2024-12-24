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

    Point* pnt = Point::parseString(j["signature"]);
    std::string pubKey = j["src"];
    std::string hashedMessage = Blockchain::sha->digest(jsonDump);

    ECDSASigner ecd;
    return ecd.verifySignature(pnt, hashedMessage, Point::parseString(pubKey));
}
