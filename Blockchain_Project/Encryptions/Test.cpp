#include "Test.h"

void Test::testSha()
{
	//3 tests: "abc", "", "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" (because 55 letter count limit, that we changed)
	const std::string abcHash = "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad";
	const std::string emptyHash = "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855";
	const std::string a56Hash = "b35439a4ac6f0948b6d6f9e3c6af0f5f590ce20f1bde7090ef7970686ec6738a";

	SHA256 hash;

	assert(abcHash == hash.digest("abc"));
	assert(emptyHash == hash.digest(""));
	assert(a56Hash == hash.digest("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"));

	//If it ran, Congratulations!
	std::cout << "Hash runs as needed!\n";
}

void Test::testEcc()
{
	//Test different cases: k = 5, k = 115792089237316195423570985008687907852837564279074904382605163141518161494322, 1
	//Expected results: 
	// k=1: G point 
	// k=5: x=2F8BDE4D1A07209355B4A7250A5C5128E88B84BDDC619AB7CBA8D569B240EFE4, y=D8AC222636E5E3D6D4DBA9DDA6C9C426F788271BAB0D6840DCA87D3AA6AC62D6
	// k=115792089237316195423570985008687907852837564279074904382605163141518161494322: 
	// x: D7924D4F7D43EA965A465AE3095FF41131E5946F3C85F79E44ADBCF8E27E080E
	// y: A7E1D78D57938D597C7BD13DD733921015BF50D427692C5A3AFB235F095D90D7

	const Point pnt2(cpp_int("0x2F8BDE4D1A07209355B4A7250A5C5128E88B84BDDC619AB7CBA8D569B240EFE4"), cpp_int("0xD8AC222636E5E3D6D4DBA9DDA6C9C426F788271BAB0D6840DCA87D3AA6AC62D6"));
	const Point pnt3(cpp_int("0xD7924D4F7D43EA965A465AE3095FF41131E5946F3C85F79E44ADBCF8E27E080E"), cpp_int("0xA7E1D78D57938D597C7BD13DD733921015BF50D427692C5A3AFB235F095D90D7"));

	KeyGenerator k = KeyGenerator();
	auto point1 = k.ECMul(1, k.GPoint);
	assert(point1->_x == k.GPoint->_x && point1->_y == k.GPoint->_y);

	auto point2 = k.ECMul(5, k.GPoint);
	assert(point2->_x == pnt2._x && point2->_y == pnt2._y);

	auto point3 = k.ECMul(cpp_int("115792089237316195423570985008687907852837564279074904382605163141518161494322"), k.GPoint);
	assert(point3->_x == pnt3._x && point3->_y == pnt3._y);

	std::cout << "ECC Key pair generation works perfectly!\n";
}

void Test::testBip39()
{
	// Test values:
	// 1. a18f8a54168f32465f844c69e2948fd1 
	// pave labor never code veteran case lawn maze hazard believe catalog pepper
	// 2. 7444ee91624354a1567ddb8e1485e60c
	// inmate cherry picture setup crystal expose fly rocket mix picnic kangaroo bonus
	// 3. 708e4b58dbf0a65f2a217076e4393c26
	// identify inch stomach retire apology convince possible come ivory canyon excuse erode
	const std::string answer1 = "pave labor never code veteran case lawn maze hazard believe catalog pepper";
	const std::string answer2 = "inmate cherry picture setup crystal expose fly rocket mix picnic kangaroo bonus";
	const std::string answer3 = "identify inch stomach retire apology convince possible come ivory canyon excuse erode";

	BIP39SeedMaker bip = BIP39SeedMaker();

	std::string ans1 = bip.transformToSeed(cpp_int("0xa18f8a54168f32465f844c69e2948fd1"));
	std::string ans2 = bip.transformToSeed(cpp_int("0x7444ee91624354a1567ddb8e1485e60c"));
	std::string ans3 = bip.transformToSeed(cpp_int("0x708e4b58dbf0a65f2a217076e4393c26"));

	assert(ans1 == answer1);
	assert(ans2 == answer2);
	assert(ans3 == answer3);
	
	std::cout << "BIP39 works perfectly fine!\n";
}

void Test::testECDSA()
{
	//Verify message signature,
	//Using a random private key!
	KeyGenerator k;
	ECDSASigner e;
	SHA256 hash;

	cpp_int priv = k.generatePrivate();
	Point* pub = k.ECMul(priv, k.GPoint);
	Point* rs = e.signMessage(priv, "abc");

	assert(e.verifySignature(rs, hash.digest("abc"), pub));

	std::cout << "ECDSA Message signing and verification works perfectly!\n";
}

void Test::testBlake2b()
{
	// 1 - ""
	const char* input1 = "";
	std::string expected1 = "786a02f742015903c6c6fd852552d272912f4740e15847618a86e217f71f5419d25e1031afee585313896444934eb04b903a685b1448b755d56f701afe9be2ce";
	uint8_t hash1[Blake2b::HASH_SIZE];

	Blake2b::hash(reinterpret_cast<const uint8_t*>(input1), strlen(input1), hash1);

	std::string hexHash1 = Blake2b::bytesToHex(hash1, Blake2b::HASH_SIZE);

	assert(hexHash1 == expected1);

	// 2 - "a"
	const char* input2 = "a";
	std::string expected2 = "333fcb4ee1aa7c115355ec66ceac917c8bfd815bf7587d325aec1864edd24e34d5abe2c6b1b5ee3face62fed78dbef802f2a85cb91d455a8f5249d330853cb3c";
	uint8_t hash2[Blake2b::HASH_SIZE];

	Blake2b::hash(reinterpret_cast<const uint8_t*>(input2), strlen(input2), hash2);

	std::string hexHash2 = Blake2b::bytesToHex(hash2, Blake2b::HASH_SIZE);

	assert(hexHash2 == expected2);

	// 3 - "abc"
	const char* input3 = "abc";
	std::string expected3 = "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6fdbffa2d17d87c5392aab792dc252d5de4533cc9518d38aa8dbf1925ab92386edd4009923";
	uint8_t hash3[Blake2b::HASH_SIZE];

	Blake2b::hash(reinterpret_cast<const uint8_t*>(input3), strlen(input3), hash3);

	std::string hexHash3 = Blake2b::bytesToHex(hash3, Blake2b::HASH_SIZE);

	assert(hexHash3 == expected3);

	// 4 - "5a db 63"
	const char* input4 = "5a db 63";
	std::string expected4 = "5a9036635ef0b5be93eb45542a3d5e1921d0fa6e88dba01919a8ea4632ac290f15b78600b6ab588f424f07395d51f9e03653c84662e0874abef0d4e999b36fe1";
	uint8_t hash4[Blake2b::HASH_SIZE];

	Blake2b::hash(reinterpret_cast<const uint8_t*>(input4), strlen(input4), hash4);

	std::string hexHash4 = Blake2b::bytesToHex(hash4, Blake2b::HASH_SIZE);

	assert(hexHash4 == expected4);

	// 5 - "The quick brown fox jumps over the lazy dog"
	const char* input5 = "The quick brown fox jumps over the lazy dog";
	std::string expected5 = "a8add4bdddfd93e4877d2746e62817b116364a1fa7bc148d95090bc7333b3673f82401cf7aa2e4cb1ecd90296e3f14cb5413f8ed77be73045b13914cdcd6a918";
	uint8_t hash5[Blake2b::HASH_SIZE];

	Blake2b::hash(reinterpret_cast<const uint8_t*>(input5), strlen(input5), hash5);

	std::string hexHash5 = Blake2b::bytesToHex(hash5, Blake2b::HASH_SIZE);

	assert(hexHash5 == expected5);

	{ // Hash output 32 bytes
			// 1 - ""
		const char* input1 = "";
		std::string expected1 = "0e5751c026e543b2e8ab2eb06099daa1d1e5df47778f7787faab45cdf12fe3a8";
		uint8_t hash1[32];

		Blake2b::hash(reinterpret_cast<const uint8_t*>(input1), strlen(input1), hash1, 32);

		std::string hexHash1 = Blake2b::bytesToHex(hash1, 32);

		assert(hexHash1 == expected1);

		// 2 - "a"
		const char* input2 = "a";
		std::string expected2 = "8928aae63c84d87ea098564d1e03ad813f107add474e56aedd286349c0c03ea4";
		uint8_t hash2[32];

		Blake2b::hash(reinterpret_cast<const uint8_t*>(input2), strlen(input2), hash2, 32);

		std::string hexHash2 = Blake2b::bytesToHex(hash2, 32);

		assert(hexHash2 == expected2);

		// 3 - "abc"
		const char* input3 = "abc";
		std::string expected3 = "bddd813c634239723171ef3fee98579b94964e3bb1cb3e427262c8c068d52319";
		uint8_t hash3[32];

		Blake2b::hash(reinterpret_cast<const uint8_t*>(input3), strlen(input3), hash3, 32);

		std::string hexHash3 = Blake2b::bytesToHex(hash3, 32);

		assert(hexHash3 == expected3);

		// 4 - "5a db 63"
		const char* input4 = "5a db 63";
		std::string expected4 = "684dcf65dcb29274778bfc42771c635c0bb00eb701e98af735202ecc840b07f8";
		uint8_t hash4[32];

		Blake2b::hash(reinterpret_cast<const uint8_t*>(input4), strlen(input4), hash4, 32);

		std::string hexHash4 = Blake2b::bytesToHex(hash4, 32);

		assert(hexHash4 == expected4);

		// 5 - "The quick brown fox jumps over the lazy dog"
		const char* input5 = "The quick brown fox jumps over the lazy dog";
		std::string expected5 = "01718cec35cd3d796dd00020e0bfecb473ad23457d063b75eff29c0ffa2e58a9";
		uint8_t hash5[32];

		Blake2b::hash(reinterpret_cast<const uint8_t*>(input5), strlen(input5), hash5, 32);

		std::string hexHash5 = Blake2b::bytesToHex(hash5, 32);

		assert(hexHash5 == expected5);
	}
	std::cout << "Blake2b works well" << std::endl;
}

// Helper function to convert byte vector to hex string
std::string Test::bytesToHexString(const std::vector<uint8_t>& bytes, size_t length)
{
	std::ostringstream oss;
	for (size_t i = 0; i < length; ++i)
		oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
	return oss.str();
}

void Test::testArgon2()
{
	// Test Vector 1
	{
		std::cout << "Running Test Vector 1..." << std::endl;

		std::string password = "password";
		std::string salt = "somesalt";

		// Expected output (hex string)
		std::string expectedHex = "c1bc4d00af5ae21bebc081ad618694850511146225d8d3a070ed95b983b8d474";

		// Initialize Argon2 instance
		//Argon2(Type type = Argon2id, uint32_t timeCost = 3, uint32_t memoryCost = 8192, uint32_t parallelism = 1, uint32_t hashLength = 32);
		Argon2 argon2;

		// Derive the key
		std::vector<uint8_t> saltBytes(salt.begin(), salt.end());
		std::vector<uint8_t> derivedKey = argon2.deriveKey(password, saltBytes);

		// Convert derived key to hex string
		std::string derivedHex = bytesToHexString(derivedKey, 32);

		// Compare derived key with expected output
		if (derivedHex == expectedHex) {
			std::cout << "Test Vector 1 passed." << std::endl;
		}
		else {
			std::cout << "Test Vector 1 failed." << std::endl;
			std::cout << "Expected: " << expectedHex << std::endl;
			std::cout << "Got:      " << derivedHex << std::endl;
		}

		std::cout << std::endl;
	}

	// Test Vector 2
	{
		std::cout << "Running Test Vector 2..." << std::endl;

		std::string password = "this is it";
		std::string salt = "nothing";
		uint32_t parallelism = 4;
		uint32_t memoryCost = 32; // Number of 1KB blocks (32 KiB total)
		uint32_t timeCost = 3;
		uint32_t hashLength = 32;
		Argon2::Type type = Argon2::Argon2id;

		// Expected output (hex string)
		std::string expectedHex =
			"89 e9 42 ff 1f 77 25 4c "
			"86 7a f3 6c 15 e3 6d 8e "
			"c8 2a 4e e4 8f 70 88 4f "
			"5d c3 54 c7 57 37 90 25";

		// Initialize Argon2 instance
		Argon2 argon2(type, timeCost, memoryCost, parallelism, hashLength);

		// Derive the key
		std::vector<uint8_t> saltBytes(salt.begin(), salt.end());
		std::vector<uint8_t> derivedKey = argon2.deriveKey(password, saltBytes);

		// Convert derived key to hex string
		std::string derivedHex = bytesToHexString(derivedKey, 32);

		// Compare derived key with expected output
		if (derivedHex == expectedHex) {
			std::cout << "Test Vector 2 passed." << std::endl;
		}
		else {
			std::cout << "Test Vector 2 failed." << std::endl;
			std::cout << "Expected: " << expectedHex << std::endl;
			std::cout << "Got:      " << derivedHex << std::endl;
		}

		std::cout << std::endl;
	}
}

void Test::testRIPEMD160()
{
	{
		std::string input = "";
		std::string expected = "9c1185a5c5e9fc54612808977ee8f548b2258d31";
		std::string res = RIPEMD_160::hash(input);

		assert(res == expected, "1 Incorrect ripemd160");
	}

	{
		std::string input = "a";
		std::string expected = "0bdc9d2d256b3ee9daae347be6f4dc835a467ffe";
		std::string res = RIPEMD_160::hash(input);

		assert(res == expected, "2 Incorrect ripemd160");
	}

	{
		std::string input = "abc";
		std::string expected = "8eb208f7e05d987a9b044a8e98c6b087f15a0bfc";
		std::string res = RIPEMD_160::hash(input);

		assert(res == expected, "3 Incorrect ripemd160");
	}

	{
		std::string input = "The quick brown fox jumps over the lazy dog";
		std::string expected = "37f332f68db77bd9d7edd4969571ad671cf9dd3b";
		std::string res = RIPEMD_160::hash(input);

		assert(res == expected, "4 Incorrect ripemd160");
	}

	std::cout << "RIPEMD-160 works well!" << std::endl;
}

void Test::testAddressGenerator()
{
	{
		std::string output = AddressGenerator::generateAddressFromPublicKey("d8ac222636e5e3d6d4dba9dda6c9c426f788271bab0d6840dca87d3aa6ac62d6");
		std::string expected = "6GzqttWiuPEFeq9CKBnExR2dWfeFPWkA";
		assert(expected == output);
	}

	{
		std::string output = AddressGenerator::generateAddressFromPublicKey("00ac222636e5e3d6d4dba9dda6c9c426f788271bab0d6840dca87d3aa6ac62d6");
		std::string expected = "6GtoDZ575yj77RcUsd6S7gdxg9fz6Zbd";
		assert(expected == output);
	}

	std::cout << "Address generator works good!" << std::endl;
}

void Test::testWalletCreation()
{
	for (int i = 0; i < 3; i++) 
	{
		Wallet wallet1 = Wallet();
		Wallet wallet2 = Wallet(wallet1._seed, true);
		Wallet wallet3 = Wallet(wallet1.getPrivateKey());

		assert(wallet1.getAddress() == wallet2.getAddress());
		assert(wallet1.getAddress() == wallet3.getAddress());
		assert(wallet2.getAddress() == wallet3.getAddress());

		assert(wallet1.getPrivateKey() == wallet2.getPrivateKey());
		assert(wallet1.getPrivateKey() == wallet3.getPrivateKey());
		assert(wallet2.getPrivateKey() == wallet3.getPrivateKey());

		assert(wallet1.getPublicKey() == wallet2.getPublicKey());
		assert(wallet1.getPublicKey() == wallet3.getPublicKey());
		assert(wallet2.getPublicKey() == wallet3.getPublicKey());
	}

	std::cout << "Wallet creation works good!" << std::endl;
}
