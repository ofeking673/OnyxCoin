#pragma once
#include "KeyGenerator.h"


class ECDSASigner
{
public:
	ECDSASigner();
	Point* signMessage(cpp_int k, std::string message);
	cpp_int generateNumber();
	bool verifySignature(Point* rs, std::string hexMessage, Point* publicKey);
private:
	SHA256* _sha;
	KeyGenerator* key;
};

