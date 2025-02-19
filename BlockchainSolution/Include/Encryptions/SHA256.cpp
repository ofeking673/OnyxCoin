#include "pch.h"
#include "SHA256.h"
bool SHA256::is1 = false;
unsigned long SHA256::k[64] = {
		0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,
		0x923f82a4,0xab1c5ed5,0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,
		0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,0xe49b69c1,0xefbe4786,
		0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
		0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,
		0x06ca6351,0x14292967,0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,
		0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,0xa2bfe8a1,0xa81a664b,
		0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
		0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,
		0x5b9cca4f,0x682e6ff3,0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,
		0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

void SHA256::compute_hash(std::string value, unsigned long H[])
{
	std::vector<unsigned long> Block;
	for (int i = 0; i < 512; i += 32)
	{
		Block.push_back(std::strtoul(value.substr(i, 32).c_str(), NULL, 2));
	} //initialize the first 16 words as a unsigned long instead of the string it is

	unsigned long W[64];

	for (int t = 0; t < 16; t++)
	{
		W[t] = Block[t] & 0xFFFFFFFF; //input 32 bit long words
	}

	for (int t = 16; t < 64; t++)
	{
		//calculate the next 48 words with this formula
		//σ1(t-2) + (t-7) + σ0(t-15) + (t-16) -> 32 bits
		W[t] = (SSIG1(W[t - 2]) + W[t - 7] + SSIG0(W[t - 15]) + W[t - 16]) & 0xFFFFFFFF; 
	}

	unsigned long temp1;
	unsigned long temp2;
	unsigned long a = H[0];
	unsigned long b = H[1];
	unsigned long c = H[2];
	unsigned long d = H[3];
	unsigned long e = H[4];
	unsigned long f = H[5];
	unsigned long g = H[6];
	unsigned long h = H[7];

	for (int i = 0; i < 64; i++) // cycle 64 times
	{
		//calculate the temporary values for compression
		temp1 = h + BSIG1(e) + CH(e, f, g) + k[i] + W[i];
		temp2 = BSIG0(a) + MAJ(a, b, c);

		//cycle through the compression values
		h = g; g = f; f = e;
		e = (d + temp1) & 0xFFFFFFFF;
		d = c; c = b; b = a;
		a = (temp1 + temp2) & 0xFFFFFFFF;
	}

	H[0] = (H[0] + a) & 0xFFFFFFFF;
	H[1] = (H[1] + b) & 0xFFFFFFFF;
	H[2] = (H[2] + c) & 0xFFFFFFFF;
	H[3] = (H[3] + d) & 0xFFFFFFFF;
	H[4] = (H[4] + e) & 0xFFFFFFFF;
	H[5] = (H[5] + f) & 0xFFFFFFFF;
	H[6] = (H[6] + g) & 0xFFFFFFFF;
	H[7] = (H[7] + h) & 0xFFFFFFFF;
}

/*
* > Input: unsigned long number
* > Output: string containing the hexadecimal value of the input
*/
std::string SHA256::hex(unsigned long inp)
{
	std::stringstream ss;
	ss << std::hex << std::setw(8) << std::setfill('0') << inp;
	return ss.str();
}

std::string SHA256::pad512bits(std::string value, bool isFinalBlock, int ExtraLen) //mul value for multiple messages
{
	int bit_length = value.size();  // length in bytes
	int padding_length = 447 - bit_length;  // how many 0's to add

	if (!isFinalBlock)
	{
		if (512 - bit_length) //If length is below 512 bits
		{
			if (!is1)
			{
				value += "1";
				is1 = true;
			}
			value += std::string(511 - bit_length, '0');
		}
		else
			value += std::string(512 - bit_length, '0');
	}
	else
	{
		std::string length_bits = std::bitset<64>(bit_length + ExtraLen).to_string(); //transfer bit length to binary

		if (!is1)
		{
			value += "1";
			is1 = true;
		}
		value += std::string(padding_length, '0');

		value += length_bits;
	}
	
	return value;
}


std::string SHA256::convertBinary(std::string value)
{
	std::string temp = "";

	for (int i = 0; i < value.length(); i++)
	{
		std::bitset<8> b(value[i]);
		temp += b.to_string();
	}

	return temp;
}

std::string SHA256::calcHash(unsigned long H[])
{
	std::string temp = "";
	for (int i = 0; i < 8; i++) // concatenate all values
	{
		temp.append(hex(H[i]));
	}
	return temp;
}

std::string SHA256::digest(std::string value)
{
	unsigned long H[8];
	H[0] = 0x6a09e667; //sqrt 2
	H[1] = 0xbb67ae85; //sqrt 3
	H[2] = 0x3c6ef372; //sqrt 5
	H[3] = 0xa54ff53a; //sqrt 7
	H[4] = 0x510e527f;
	H[5] = 0x9b05688c;
	H[6] = 0x1f83d9ab;
	H[7] = 0x5be0cd19;

	if (value.size() > 55)
	{
		int i = 0;
		std::string pad;
		bool go = true;
		std::string bin;
		bin = convertBinary(value);
		std::string temp;
		//always check if over 447, if yes. run pad and update H array
		//if not, run as last block the remainder
		do {
			temp = bin.substr(i, 512);
			if (temp.size() > 447)
			{
				i += temp.size();
				pad = pad512bits(temp, false, 0); //if its over 447 bits, take the first 512
				compute_hash(pad, H);
			}
			else
			{
				pad = pad512bits(temp, true, i);
				compute_hash(pad, H);
				go = false;
			}
		} while (go);
		is1 = false;
		return calcHash(H);

	}

	value = convertBinary(value);
	value = pad512bits(value, true, 0);
	compute_hash(value, H);
	is1 = false;
	return calcHash(H);
}