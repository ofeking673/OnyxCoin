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
	static std::string generateAddressFromPublicKey(std::string publicKey);
	static std::string generateAddressFromPrivateKey(std::string privateKey);
private:
	static std::string padHexTo64Bytes(std::string hexStr);
};

