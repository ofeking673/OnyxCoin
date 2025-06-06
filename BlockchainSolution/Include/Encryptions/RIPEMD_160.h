#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <sstream>
#include <iomanip>

#define RIPEMD160_BLOCK_SIZE 64 //512 bits

class RIPEMD_160
{
public:
	RIPEMD_160();

	static std::string hash(const std::string& input);
	static std::string hash(const std::vector<uint8_t>& input);
private:
	std::vector<uint8_t> _message;

	uint32_t _h0;
	uint32_t _h1;
	uint32_t _h2;
	uint32_t _h3;
	uint32_t _h4;

	uint8_t _buffer[RIPEMD160_BLOCK_SIZE];

	uint64_t _bitCount; // Total number of bits processed

	const std::vector<uint32_t> Kleft = { 0x00000000UL, 0x5A827999UL, 0x6ED9EBA1UL, 0x8F1BBCDCUL, 0xA953FD4EUL };
	const std::vector<uint32_t> Kright = { 0x50A28BE6UL, 0x5C4DD124UL, 0x6D703EF3UL, 0x7A6D76E9UL, 0x00000000UL };
	const std::vector<int> rleft = {
		0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
		3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
		1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2,
		4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13 };

	const std::vector<int> rright = {
		5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
		6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
		15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
		8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14,
		12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11 };


	const std::vector<int> sleft = {
		11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
		7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
		11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
		11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12,
		9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6 };

	const std::vector<int> sright = {
		8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
		9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
		9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
		15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8,
		8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11 };

	void pad_message();
	void processBlocks(const uint8_t* data, size_t length);
	void compressBlock(const uint32_t words[16]);

	void digest(uint8_t out[20]);

	std::vector<uint8_t> strMessageToBytesVector(std::string message);

	static std::string toHex(const uint8_t* data, size_t len);



	uint32_t Ffunc(size_t j, uint32_t xnum, uint32_t ynum, uint32_t znum)
	{
		if (0 <= j && j <= 15)
		{
			// F1
			return (xnum) ^ (ynum) ^ (znum);
		}
		else if (16 <= j && j <= 31)
		{
			// F2
			return ((xnum) & (ynum)) | (~(xnum) & (znum));
		}
		else if (32 <= j && j <= 47)
		{
			// F3
			return ((xnum) | ~(ynum)) ^ (znum);
		}
		else if (48 <= j && j <= 63)
		{
			// F4
			return ((xnum) & (znum)) | ((ynum) & ~(znum));
		}
		else if (64 <= j && j <= 79)
		{
			// F5
			return (xnum) ^ ((ynum) | ~(znum));
		}
	}
	uint32_t rotateLeft(uint32_t xnum, size_t times)
	{
		return ((xnum) << (times)) | ((xnum) >> (32 - (times)));
	}
};

