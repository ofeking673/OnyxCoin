#include "BIP39SeedMaker.h"

std::string BIP39SeedMaker::checksum(cpp_int v)
{
    int binSize = binary(v).size();

    // Convert the entropy to raw bytes
    std::string hexValue = cppIntToHex(v);
    std::string hexBytes = convertHexStrToBytes(hexValue);

    // Hash the entropy raw bytes
    std::string hash = sha.digest(hexBytes);
    std::string hashBin = binary(cpp_int("0x"+hash));   

    // Return first 4 bits of hash
    return hashBin.substr(0, (binSize/32));
}

// Convert value to binary
std::string BIP39SeedMaker::binary(cpp_int v)
{
    std::string bin;
    std::stringstream ss;
    ss << std::hex << std::setw(32) << std::setfill('0') << v;
    for (char c : ss.str()) {
        int digit = (c >= '0' && c <= '9') ? c - '0' : (c - 'a' + 10);
        bin += std::bitset<4>(digit).to_string();
    }
    return bin;
}   

std::vector<int> BIP39SeedMaker::splitToGroups(std::string binary)
{
    std::vector<int> ans;
    for (int i = 0; i < binary.size(); i += 11) {
        ans.push_back(std::stoi(binary.substr(i, 11), nullptr, 2));
    }

    return ans;
}

std::string BIP39SeedMaker::transformToSeed(cpp_int v)
{
    std::string bin = binary(v) + checksum(v); // OK
    std::cout << bin << std::endl;
    auto ans = splitToGroups(bin); // OK
    std::string seed;
    for (int i : ans) {
        std::cout << i << std::endl;
        seed += wordlist[i] + " ";
    }

    //for (int i = ans.size(); i < 12; i++) {
    //    seed += wordlist[0] + " ";
    //}
    seed.pop_back();

    return seed;
}

std::string BIP39SeedMaker::cppIntToHex(const cpp_int& value)
{
    std::stringstream ss;
    ss << std::hex << value;
    return ss.str();
}



// Helper function to convert a single hex character to its integer value
unsigned char BIP39SeedMaker::hexCharToByte(char c)
{
    if (std::isdigit(c)) 
    {
        return c - '0';
    }
    else if (std::isxdigit(c)) 
    {
        // Convert to uppercase
        c = std::toupper(c);
        return c - 'A' + 10;
    }
    else {
        throw std::invalid_argument("Invalid hexadecimal character");
    }
}

// Function to convert a hex string to raw bytes
std::string BIP39SeedMaker::convertHexStrToBytes(const std::string& hex)
{
    // Check if the hex string has an even length
    if (hex.length() % 2 != 0)
    {
        throw std::invalid_argument("Hex string must have an even length");
    }

    std::string bytes;
    bytes.reserve(hex.length() / 2); // Reserve space for efficiency

    for (size_t i = 0; i < hex.length(); i += 2) {
        // Convert each pair of hex characters to a byte
        unsigned char highNibble = hexCharToByte(hex[i]);
        unsigned char lowNibble = hexCharToByte(hex[i + 1]);
        unsigned char byte = (highNibble << 4) | lowNibble;
        bytes += byte;
    }

    return bytes;
}

