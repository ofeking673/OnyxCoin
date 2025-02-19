#pragma once

#include "BIP39SeedMaker.h"
#include "Base58.h"
#include "ECDSASigner.h"
#include "KeyGenerator.h"
#include "RIPEMD_160.h"
#include "SHA256.h"

class AddressGenerator
{
public:
	static std::string generateAddressFromPublicKey(std::string publicKey);
	static std::string generateAddressFromPrivateKey(std::string privateKey);
private:
	static std::string padHexTo64Bytes(std::string hexStr);
};

