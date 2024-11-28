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
	uint8_t hash1[Crypto::Blake2b::HASH_SIZE];

	Crypto::Blake2b::hash(reinterpret_cast<const uint8_t*>(input1), strlen(input1), hash1);

	std::string hexHash1 = Crypto::Blake2b::bytesToHex(hash1, Crypto::Blake2b::HASH_SIZE);

	assert(hexHash1 == expected1);

	// 2 - "a"
	const char* input2 = "a";
	std::string expected2 = "333fcb4ee1aa7c115355ec66ceac917c8bfd815bf7587d325aec1864edd24e34d5abe2c6b1b5ee3face62fed78dbef802f2a85cb91d455a8f5249d330853cb3c";
	uint8_t hash2[Crypto::Blake2b::HASH_SIZE];

	Crypto::Blake2b::hash(reinterpret_cast<const uint8_t*>(input2), strlen(input2), hash2);

	std::string hexHash2 = Crypto::Blake2b::bytesToHex(hash2, Crypto::Blake2b::HASH_SIZE);

	assert(hexHash2 == expected2);

	// 3 - "abc"
	const char* input3 = "abc";
	std::string expected3 = "ba80a53f981c4d0d6a2797b69f12f6e94c212f14685ac4b74b12bb6fdbffa2d17d87c5392aab792dc252d5de4533cc9518d38aa8dbf1925ab92386edd4009923";
	uint8_t hash3[Crypto::Blake2b::HASH_SIZE];

	Crypto::Blake2b::hash(reinterpret_cast<const uint8_t*>(input3), strlen(input3), hash3);

	std::string hexHash3 = Crypto::Blake2b::bytesToHex(hash3, Crypto::Blake2b::HASH_SIZE);

	assert(hexHash3 == expected3);

	// 4 - "5a db 63"
	const char* input4 = "5a db 63";
	std::string expected4 = "5a9036635ef0b5be93eb45542a3d5e1921d0fa6e88dba01919a8ea4632ac290f15b78600b6ab588f424f07395d51f9e03653c84662e0874abef0d4e999b36fe1";
	uint8_t hash4[Crypto::Blake2b::HASH_SIZE];

	Crypto::Blake2b::hash(reinterpret_cast<const uint8_t*>(input4), strlen(input4), hash4);

	std::string hexHash4 = Crypto::Blake2b::bytesToHex(hash4, Crypto::Blake2b::HASH_SIZE);

	assert(hexHash4 == expected4);

	// 5 - "The quick brown fox jumps over the lazy dog"
	const char* input5 = "The quick brown fox jumps over the lazy dog";
	std::string expected5 = "a8add4bdddfd93e4877d2746e62817b116364a1fa7bc148d95090bc7333b3673f82401cf7aa2e4cb1ecd90296e3f14cb5413f8ed77be73045b13914cdcd6a918";
	uint8_t hash5[Crypto::Blake2b::HASH_SIZE];

	Crypto::Blake2b::hash(reinterpret_cast<const uint8_t*>(input5), strlen(input5), hash5);

	std::string hexHash5 = Crypto::Blake2b::bytesToHex(hash5, Crypto::Blake2b::HASH_SIZE);

	assert(hexHash5 == expected5);


	std::cout << "Blake2b works well" << std::endl;
}
