#include "MessageManager.h"
#include "AddressGenerator.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

MessageP2P MessageManager::createPingMessage(const std::string& publicKey)
{
    MessageP2P message;

    std::string address = AddressGenerator::generateAddressFromPrivateKey(privateKey);

    message.setType(MessageType::PING);
    message.setAuthor(address);

    // Time of message
    std::string time = getCurrentDateTime();
    message.setLength(time.length());
    message.setPayload(time);

    signMessage(message, privateKey);

    return message;
}

MessageP2P MessageManager::createPongMessage(const std::string& privateKey, const std::string& time)
{
    MessageP2P message;

    std::string address = AddressGenerator::generateAddressFromPrivateKey(privateKey);

    message.setType(MessageType::PING);
    message.setAuthor(address);

    // Time of PING message
    message.setLength(time.length());
    message.setPayload(time);

    signMessage(message, privateKey);

    return message;
}


MessageP2P MessageManager::createGetPeersMessage(const std::string& publicKey)
{
    MessageP2P message;
    message.setType(MessageType::GET_PEERS);
    message.setAuthor(publicKey);


    return message;
}

std::string MessageManager::getCurrentDateTime()
{
    // Get the current time using the system clock.
    auto now = std::chrono::system_clock::now();

    // Convert to time_t which represents calendar time
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Convert to local time 
    std::tm tm_struct;
    #if defined(_MSC_VER)
        localtime_s(&tm_struct, &now_time);
    #else
        tm_struct = *std::localtime(&now_time);
    #endif

    // Use string stream and put_time to format the date and time
    std::ostringstream oss;
    oss << std::put_time(&tm_struct, "%Y-%m-%d %H:%M:%S");

    return oss.str();
}
