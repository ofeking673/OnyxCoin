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
