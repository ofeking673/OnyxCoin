#pragma once
#include "SHA256.h"
#include "KeyGenerator.h"
#include "BIP39SeedMaker.h"
#include "ECDSASigner.h"
#include "Blake2b.h"
#include "Argon2.h"

class Test
{
public:
	void testSha();
	void testEcc();
	void testBip39();
	void testECDSA();
	void testBlake2b();
	void testArgon2();


	//std::vector<uint8_t> hexStringToBytes(const std::string& hex);
	std::string bytesToHexString(const std::vector<uint8_t>& bytes);
};

