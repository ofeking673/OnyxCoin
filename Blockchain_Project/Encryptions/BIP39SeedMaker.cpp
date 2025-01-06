#include "BIP39SeedMaker.h"


std::string BIP39SeedMaker::checksum(cpp_int v)
{
    int binSize = binary(v).size();

    // Convert the entropy to raw bytes
    std::string hexValue = ECDSASigner::cppIntToHexString(v);
    std::string hexBytes = convertHexStrToBytes(hexValue);

    // Hash the entropy raw bytes
    std::string hash = SHA256::digest(hexBytes);
    std::string hashBin = binary(cpp_int("0x"+hash));   

    // Return first 4 bits of hash
    return hashBin.substr(0, binSize/32);
}

// Convert value to binary
std::string BIP39SeedMaker::binary(cpp_int v)
{
    std::string bin;
    std::stringstream ss;
    ss << std::hex << v;
    for (char c : ss.str()) {
        int digit = (c >= '0' && c <= '9') ? c - '0' : (c - 'a' + 10);
        bin += std::bitset<4>(digit).to_string();
    }
    return PadBinary(bin, 128);
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
    auto ans = splitToGroups(bin); // OK
    std::string seed;
    for (int i : ans) {
        seed += wordlist[i] + " ";
    }

    //for (int i = ans.size(); i < 12; i++) {
    //    seed += wordlist[0] + " ";
    //}
    seed.pop_back();

    return seed;
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
std::string BIP39SeedMaker::PadBinary(std::string& bin, int len) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(len) << bin;
    return ss.str();
}
std::string BIP39SeedMaker::PadToNearestHex(std::string& hex, int amount)
{
    std::stringstream ss;
    if(amount == 1) 
    {
        ss << std::setfill('0') << std::setw(hex.size() + amount) << hex;
    }
    else
    {
        ss << std::setfill('0') << std::setw(amount) << hex;
    }
    return ss.str();
}


// Function to convert a hex string to raw bytes
std::string BIP39SeedMaker::convertHexStrToBytes(std::string& hex)
{
    // Check if the hex string has an even length
    if (hex.length() % 2 != 0)
    {
        hex = PadToNearestHex(hex);
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

cpp_int BIP39SeedMaker::binToInt(std::string bin)
{
    cpp_int num = 0;
    for (char c : bin) {
        num = (num * 2) + (c - '0');
    }
    return num;
}

std::string BIP39SeedMaker::reverseSeed(std::string seed)
{
    std::istringstream ss(seed);
    std::stringstream s;
    std::string word;
    std::string bin;
    int i = 0;
    while(ss >> word) {
        int index = std::find(wordlist.begin(), wordlist.end(), word) - wordlist.begin();
        std::string binIndex = binary(index);
        s << std::setw(11) << std::setfill('0') << binIndex.substr(binIndex.size()-11); //binary returns a 12 bit number, 4 * 3
        bin += s.str();
        s.str("");
    }

    std::string checksum = bin.substr(128);
    bin = bin.substr(0, 128);
    return ECDSASigner::cppIntToHexString(binToInt(bin));
}