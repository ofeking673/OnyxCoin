#include "..\Encryptions\RIPEMD_160.h"

RIPEMD_160::RIPEMD_160()
{
	// Initial values
	_h0 = 0x67452301;
	_h1 = 0xEFCDAB89;
	_h2 = 0x98BADCFE;
	_h3 = 0x10325476;
	_h4 = 0xC3D2E1F0;

	// Initialize the buffer to 0
	for (size_t i = 0; i < RIPEMD160_BLOCK_SIZE; i++)
	{
		_buffer[i] = 0;
	}

	_bitCount = 0;
}

std::string RIPEMD_160::hash(const std::string& input)
{
	RIPEMD_160 ripemd160;
	ripemd160._message = ripemd160.strMessageToBytesVector(input);
	ripemd160.pad_message();
	ripemd160.processBlocks(ripemd160._message.data(), ripemd160._message.size());

	uint8_t out[20];
	ripemd160.digest(out);

	return toHex(out, 20);
}

std::string RIPEMD_160::hash(const std::vector<uint8_t>& input)
{
	RIPEMD_160 ripemd160;
	ripemd160._message = input;
	ripemd160.pad_message();
	ripemd160.processBlocks(ripemd160._message.data(), ripemd160._message.size());

	uint8_t out[20];
	ripemd160.digest(out);

	return toHex(out, 20);
}

void RIPEMD_160::pad_message()
{
	// Calculate the original length in bits
	uint64_t originalLengthBits = static_cast<uint64_t>(_message.size()) * 8;

	// Append 0x80 byte (1000 0000 in binary)
	_message.push_back(0x80);

	// Add zero bytes until message length in bits % 512 = 448

	size_t bytesLengthMod64 = _message.size() % RIPEMD160_BLOCK_SIZE;

	// Need bytesLengthMod64 to be 56 (56 bytes = 448 bits)
	// 8 bytes are reserved at the end for the length

	size_t paddingNeeded = 0;
	if (bytesLengthMod64 <= 56)
	{
		paddingNeeded = (56 - bytesLengthMod64);
	}
	else
	{
		paddingNeeded = (56 + RIPEMD160_BLOCK_SIZE - bytesLengthMod64);
	}

	// Append the required number of 0s
	_message.insert(_message.end(), paddingNeeded, 0x00);

	// Append the original length in bits as a 64bit little-endian
	for (size_t i = 0; i < 8; i++)
	{
		_message.push_back(static_cast<uint8_t>(originalLengthBits >> (8 * i)));
	}
}

void RIPEMD_160::processBlocks(const uint8_t* data, size_t length)
{
	// length is a multiple of 64 (after padding)
	while (length >= 64)
	{
		// Break current 64 block into 16 words of 32 bits
		uint32_t words[16];
		for (size_t i = 0; i < 16; i++)
		{
			// cur word = data[i*4], data[i*4+1], data[i*4+2], data[i*4+3]
			words[i] = (static_cast<uint32_t>(data[i * 4 + 0]))
				| (static_cast<uint32_t>(data[i * 4 + 1]) << 8)
				| (static_cast<uint32_t>(data[i * 4 + 2]) << 16)
				| (static_cast<uint32_t>(data[i * 4 + 3]) << 24);
		}

		compressBlock(words);

		// Move to the next block
		data += 64;
		length -= 64;
	}
}

void RIPEMD_160::compressBlock(const uint32_t words[16])
{
	uint32_t aleft = _h0, bleft = _h1, cleft = _h2, dleft = _h3, eleft = _h4;
	uint32_t aright = _h0, bright = _h1, cright = _h2, dright = _h3, eright = _h4;

	uint32_t temp = 0;
	for (size_t j = 0; j < 80; j++)
	{
		// Left line
		temp = aleft;
		temp += Ffunc(j, bleft, cleft, dleft);
		temp += words[rleft[j]];
		temp += Kleft[j / 16];
		temp = rotateLeft(temp, sleft[j]);
		temp += eleft;

		aleft = eleft;
		eleft = dleft;
		dleft = rotateLeft(cleft, 10);
		cleft = bleft;
		bleft = temp;

		// Right line
		temp = aright;
		temp += Ffunc(79 - j, bright, cright, dright);
		temp += words[rright[j]];
		temp += Kright[j / 16];
		temp = rotateLeft(temp, sright[j]);
		temp += eright;

		aright = eright;
		eright = dright;
		dright = rotateLeft(cright, 10);
		cright = bright;
		bright = temp;
	}

	temp = _h1 + cleft + dright;
	_h1 = _h2 + dleft + eright;
	_h2 = _h3 + eleft + aright;
	_h3 = _h4 + aleft + bright;
	_h4 = _h0 + bleft + cright;
	_h0 = temp;
}

void RIPEMD_160::digest(uint8_t out[20])
{
	uint32_t sum[5] = { _h0, _h1, _h2, _h3, _h4 };
	for (size_t i = 0; i < 5; i++)
	{
		out[i * 4 + 0] = static_cast<uint8_t>(sum[i]);
		out[i * 4 + 1] = static_cast<uint8_t>(sum[i] >> 8);
		out[i * 4 + 2] = static_cast<uint8_t>(sum[i] >> 16);
		out[i * 4 + 3] = static_cast<uint8_t>(sum[i] >> 24);
	}
}

std::vector<uint8_t> RIPEMD_160::strMessageToBytesVector(std::string message)
{
	std::vector<uint8_t> bytes(message.begin(), message.end());
	return bytes;
}

std::string RIPEMD_160::toHex(const uint8_t* data, size_t len)
{
	std::ostringstream oss;
	oss << std::hex << std::setfill('0');
	for (size_t i = 0; i < len; i++)
	{
		oss << std::setw(2) << static_cast<unsigned>(data[i]);
	}
	return oss.str();
}
