#pragma once

#include "Encryptions/BIP39SeedMaker.h"
#include "Encryptions/Base58.h"
#include "Encryptions/ECDSASigner.h"
#include "Encryptions/KeyGenerator.h"
#include "Encryptions/RIPEMD_160.h"
#include "Encryptions/SHA256.h"

class AddressGenerator
{
public:
	static std::string generateAddress(cpp_int publicKey);
private:
	static std::string padHexTo32Bytes(std::string hexStr);
};

