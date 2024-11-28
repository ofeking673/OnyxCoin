#pragma once
#include "SHA256.h"
#include "KeyGenerator.h"
#include "BIP39SeedMaker.h"
#include "ECDSASigner.h"
#include "Blake2b.h"

class Test
{
public:
	void testSha();
	void testEcc();
	void testBip39();
	void testECDSA();
	void testBlake2b();
};

