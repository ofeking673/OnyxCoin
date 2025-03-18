#pragma once
#include "KeyGenerator.h"


class ECDSASigner
{
public:
	ECDSASigner();
	Point* signMessage(cpp_int k, std::string message);
	cpp_int generateNumber();
	//the point, hashed message, and the public key to test against
	// Notice: the hex message should be hashed again when giving it to the function
	bool verifySignature(Point* rs, std::string hexMessage, Point* publicKey);

	static std::string cppIntToHexString(const cpp_int& v);
	static cpp_int hexStringToCppInt(const std::string& hex);
private:
	KeyGenerator* key;
};

