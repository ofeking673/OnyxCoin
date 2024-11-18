#include <iostream>
#include <thread>
#include <chrono>
#include "Blockchain.h"
#include "KeyGenerator.h"
#include "ECDSASigner.h"
#include "Base58.h"

int main()
{
	KeyGenerator* keyGen = new KeyGenerator();
    SHA256* sha = new SHA256();
    ECDSASigner* sign = new ECDSASigner();
   
    cpp_int privateKey(keyGen->generatePrivate()); //Generate a new private key

    std::cout << "New private key: " << privateKey << std::endl;


    Point* publicKey = keyGen->ECMul(privateKey, keyGen->GPoint);
    std::cout << "Public key for that private key:\n  X: " << publicKey->_x << "\n  Y: " << publicKey->_y << std::endl << std::endl;
 //   std::cout << "Extracting signature for message 'abc'...\n";
 //   std::string str = "abc";
 //   Point* rs = sign->signMessage(privateKey, str);
 //   std::cout << "Signature values: R = " << rs->_x << " S = " << rs->_y <<std::endl << std::endl;
 //   std::cout << "Is the message signed correctly? " << ((sign->verifySignature(rs, sha->digest(str), publicKey)) ? "Yes" : "No") << std::endl;

    cpp_int pub = KeyGenerator::cat(publicKey->_x, publicKey->_y);
    std::cout << pub << std::endl;
	Base58 b;
	std::cout << b.base58(pub);
    return 0;
}