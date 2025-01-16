#include "AddressGenerator.h"

std::string AddressGenerator::generateAddressFromPublicKey(std::string publicKey)
{
	//BIP39SeedMaker bip39;
	//Base58 base58;

	//std::string versionByte = "ff";
	//publicKey = padHexTo64Bytes(publicKey);

	//std::string publicKeyHash = RIPEMD_160::hash(SHA256::digest(publicKey));
	//std::string versionedPayload = versionByte + publicKeyHash;

	//// Checksum is first 4 bytes
	//std::string checksum = SHA256::digest(SHA256::digest(versionedPayload)); 
	//checksum = checksum.substr(0, 4);

	//std::string hexToEncode = "0x" + versionedPayload + checksum;
	//cpp_int toEncode = cpp_int(hexToEncode);
	//std::string address = base58.base58(toEncode);
	//return address;
	return RIPEMD_160::hash(SHA256::digest(publicKey));
}

std::string AddressGenerator::generateAddressFromPrivateKey(std::string privateKey)
{
	KeyGenerator ecc;
	cpp_int privateK = ECDSASigner::hexStringToCppInt(privateKey);
	Point* publicK = ecc.ECMul(privateK, ecc.GPoint);
	std::string publicKey = publicK->ToStringNoSeperator();
	std::string address = AddressGenerator::generateAddressFromPublicKey(publicKey);
}

std::string AddressGenerator::padHexTo64Bytes(std::string hexStr)
{
	// Calculate the number of leading zeros needed
	size_t padding_length = 128 - hexStr.size();

	// Create a string with the required number of leading zeros
	std::string padding(padding_length, '0');

	// Concatenate the padding and the hex string
	std::string padded_hex = padding + hexStr;

	return padded_hex;
}
