#include "BIP39SeedMaker.h"

std::string BIP39SeedMaker::checksum(cpp_int v)
{
    int binSize = binary(v).size();
    std::string hash = sha.digest(cppIntToHex(v));
    std::string hashBin = binary(cpp_int("0x"+hash));
    std::cout << hashBin.substr(0, (binSize / 32)) << std::endl;
    return hashBin.substr(0, (binSize/32));
}

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
    std::string bin = binary(v) + checksum(v);
    std::cout << bin << std::endl;
    auto ans = splitToGroups(bin);
    std::string seed;
    for (int i : ans) {
        std::cout << i << std::endl;
        seed += wordlist[i] + " ";
    }

    for (int i = ans.size(); i < 12; i++) {
        seed += wordlist[0] + " ";
    }
    seed.pop_back();

    return seed;
}

std::string BIP39SeedMaker::cppIntToHex(const cpp_int& value)
{
    std::stringstream ss;
    ss << std::hex << value;
    return ss.str();
}
