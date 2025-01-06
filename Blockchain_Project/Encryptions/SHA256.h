#pragma once
#include <string>
#include <sstream>
#include <iostream>
#include <bitset>
#include <vector>
#include <iomanip>

//defined functions for SHA-256
//rotate word by X bits
#define ROTRIGHT(word,bits) (((word) >> (bits)) | ((word) << (32-(bits))))
//small sigma 0
#define SSIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3)) // rot 7 xor rot 18 xor shr 3
//small sigma 1
#define SSIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10)) //rot 17 xor rot 19 xor shr 10
//choice
#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z))) //basically multiplex
//majority
#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z))) //takes the most used bit
//big sigma 0
#define BSIG0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x, 22)) 
//big sigma 1
#define BSIG1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^  ROTRIGHT(x, 25))

class SHA256
{
public:
	SHA256() = default;

	static unsigned long k[64]; //first 64 prime numbers in the root of 3

	static void compute_hash(std::string value, unsigned long H[]);
	static std::string hex(unsigned long inp);
	static std::string pad512bits(std::string value, bool isFinalBlock, int ExtraLen);
	static std::string convertBinary(std::string value);
	static std::string calcHash(unsigned long H[]);
	static std::string digest(std::string value);

	static bool is1;
};

