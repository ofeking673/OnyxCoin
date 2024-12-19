#include "AddressGenerator.h"

std::string AddressGenerator::generateAddress(cpp_int publicKey)
{

	SHA256 sha256;
	BIP39SeedMaker bip39;
	Base58 base58;

	std::string versionByte = "ff";
	std::string hexPublicKey = bip39.cppIntToHex(publicKey);
	hexPublicKey = padHexTo32Bytes(hexPublicKey);

	std::string publicKeyHash = RIPEMD_160::hash(sha256.digest(hexPublicKey));
	std::string versionedPayload = versionByte + publicKeyHash;

	// Checksum is first 4 bytes
	std::string checksum = sha256.digest(sha256.digest(versionedPayload)); 
	checksum = checksum.substr(0, 4);

	std::string hexToEncode = "0x" + versionedPayload + checksum;
	cpp_int toEncode = cpp_int(hexToEncode);
	std::string address = base58.base58(toEncode);
	return address;
}

std::string AddressGenerator::padHexTo32Bytes(std::string hexStr)
{
	// Calculate the number of leading zeros needed
	size_t padding_length = 64 - hexStr.size();

	// Create a string with the required number of leading zeros
	std::string padding(padding_length, '0');

	// Concatenate the padding and the hex string
	std::string padded_hex = padding + hexStr;

	return padded_hex;
}
