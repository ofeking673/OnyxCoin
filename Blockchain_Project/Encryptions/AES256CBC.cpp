#include "AES256CBC.h"

AES256CBC::AES256CBC(cpp_int masterKey)
{
	keyMatrix = expand_key(masterKey);
}

std::string AES256CBC::cbcEncrypt(std::string plain_text, std::vector<uint8_t> iv)
{
	plain_text = pad(plain_text);

	std::vector<std::vector<uint8_t>> blocks;
	std::vector<uint8_t> prevCipher = iv;

	for (auto block : splitToBlocks(plain_text)) {
		auto xord = xorBytes(block, prevCipher);
		auto cipher_block = encrypt(xord);

		blocks.push_back(cipher_block);
		prevCipher = cipher_block;
	}

	return parseMatrix(blocks);
}

std::vector<uint8_t> AES256CBC::encrypt(std::vector<uint8_t> val)
{
	assert(val.size() == 16);

	auto plain_state = parseVectorToMatrix(val);
	add_round_key(plain_state, keyMatrix[0]);

	for (uint8_t i = 1; i < nround-1; i++) {
		sub_bytes(plain_state);
		shift_rows(plain_state);
		mix_columns(plain_state);
		add_round_key(plain_state, keyMatrix[i]);
	}

	sub_bytes(plain_state);
	shift_rows(plain_state);
	add_round_key(plain_state, keyMatrix[keyMatrix.size() - 1]);

	return parseMatrixToVector(plain_state);
}

std::vector<std::vector<std::vector<uint8_t>>> AES256CBC::expand_key(cpp_int key)
{
	auto key_col = parseInitialKey(key);
	int iteration_size = key_col.size();

	int j = 1;
	for (int i = 8; i < 60; i++) { //First 8 words already exist.
		std::vector<uint8_t> t = key_col[i - 1];

		if (i % 8 == 0) {
			std::rotate(t.begin(), t.begin() + 1, t.end());
			t = subVector(t);
			t[0] ^= r_con[j];
			j++;
		}
		else if (i % 4 == 0) {
			t = subVector(t);
		}
		key_col[i] = xorBytes(t, key_col[i - 8]);
	}

	std::vector<std::vector<std::vector<uint8_t>>> result(15, std::vector<std::vector<uint8_t>>(4));

	for (int i = 0; i < key_col.size(); i++) {
		result[i / 4][i % 4] = key_col[i];
	}
	return result;
}

std::vector<uint8_t> AES256CBC::subVector(std::vector<uint8_t> v)
{
	for (uint8_t n = 0; n < v.size(); n++) {
		v[n] = s_box[v[n]];
	}
	return v;
}

void AES256CBC::sub_bytes(std::vector<std::vector<uint8_t>>& matrix)
{
	for (uint8_t i = 0; i < 4; i++) {
		for (uint8_t j = 0; j < 4; j++) {
			matrix[i][j] = s_box[matrix[i][j]];
		}
	}
}

void AES256CBC::inv_sub_bytes(std::vector<std::vector<uint8_t>>& matrix)
{
	for (uint8_t i = 0; i < 4; i++) {
		for (uint8_t j = 0; j < 4; j++) {
			matrix[i][j] = inv_s_box[matrix[i][j]];
		}
	}
}

void AES256CBC::shift_rows(std::vector<std::vector<uint8_t>>& matrix)
{
	std::rotate(matrix[1].begin(), matrix[1].begin() + 1, matrix[1].end());

	std::rotate(matrix[2].begin(), matrix[2].begin() + 2, matrix[2].end());

	std::rotate(matrix[3].begin(), matrix[3].begin() + 3, matrix[3].end());
}

void AES256CBC::inv_shift_rows(std::vector<std::vector<uint8_t>>& matrix)
{
    /*    s[0][1], s[1][1], s[2][1], s[3][1] = s[3][1], s[0][1], s[1][1], s[2][1]
    s[0][2], s[1][2], s[2][2], s[3][2] = s[2][2], s[3][2], s[0][2], s[1][2]
    s[0][3], s[1][3], s[2][3], s[3][3] = s[1][3], s[2][3], s[3][3], s[0][3]*/

	std::rotate(matrix[1].begin(), matrix[1].begin() + (matrix.size() - 1), matrix[1].end());
	std::rotate(matrix[2].begin(), matrix[2].begin() + (matrix.size() - 2), matrix[2].end());
	std::rotate(matrix[3].begin(), matrix[3].begin() + (matrix.size() - 3), matrix[3].end());
}

uint8_t AES256CBC::gmul(uint8_t a, uint8_t b) //Almost like ecc point-scalar multiplication 5b = ((b*2)*2)+b
{
	int result = 0;
	uint8_t high_bit_mask = 0x80; // MSB in 8 bits
	uint8_t modulo = 0x1B; // AES irreducible polynomial

	for (uint8_t i = 0; i < 8; i++) {
		if (b & 1) {
			result ^= a; // Add if LSB of b is 1
		}

		bool high_bit = a & high_bit_mask;
		a <<= 1;
		if (high_bit) {
			a ^= modulo;
		}
		b >>= 1; // Move to the next bit
	}

	return result & 0xFF;
}

void AES256CBC::mix_columns(std::vector<std::vector<uint8_t>>& matrix)
{
	std::vector<std::vector<uint8_t>> mulVect = { {2, 3, 1, 1}, {1, 2, 3, 1}, {1, 1, 2, 3}, {3, 1, 1, 2} };
	for (uint8_t i = 0; i < 4; i++) {
		mix_single_column(matrix[i], mulVect);
	}
}

void AES256CBC::mix_single_column(std::vector<uint8_t>& s, std::vector<std::vector<uint8_t>> mulVect)
{
	std::vector<uint8_t> temp(4);

	for (uint8_t i = 0; i < 4; i++) {
		temp[i] = gmul(s[0], mulVect[i][0]) ^
				  gmul(s[1], mulVect[i][1]) ^
				  gmul(s[2], mulVect[i][2]) ^
				  gmul(s[3], mulVect[i][3]);
	}

	s = temp;
}

void AES256CBC::inv_mix_columns(std::vector<std::vector<uint8_t>>& matrix)
{
	/*0x0E  0x0B  0x0D  0x09
0x09  0x0E  0x0B  0x0D
0x0D  0x09  0x0E  0x0B
0x0B  0x0D  0x09  0x0E*/
	std::vector<std::vector<uint8_t>> mulVect = { {0x0E, 0x0B, 0x0D, 0x09}, 
											  {0x09, 0x0E, 0x0B, 0x0D}, 
											  {0x0D, 0x09, 0x0E, 0x0B},
											  {0x0B, 0x0D, 0x09, 0x0E} };

	for (uint8_t i = 0; i < 4; i++) {
		mix_single_column(matrix[i], mulVect);
	}
}

void AES256CBC::add_round_key(std::vector<std::vector<uint8_t>>& matrix, std::vector<std::vector<uint8_t>> roundkey)
{
	for (uint8_t i = 0; i < 4; i++) {
		for (uint8_t j = 0; j < 4; j++) {
			matrix[i][j] = matrix[i][j] ^ roundkey[i][j];
		}
	}
}

std::vector<std::vector<uint8_t>> AES256CBC::textToMatrix(std::string text)
{
	std::vector<std::vector<uint8_t>> vec(4, std::vector<uint8_t>(4));
	for (uint8_t i = 0; i < 4; i++) {
		vec[i] = std::vector<uint8_t>(4);
	}

	for (uint8_t i = 0; i < 16; i++) {
		vec[i / 4][i % 4] = static_cast<uint8_t>(text[i]);
	}
	return vec;
}

std::vector<uint8_t> AES256CBC::xorBytes(std::vector<uint8_t> s, std::vector<uint8_t> s2)
{
	std::vector<uint8_t> a(std::min(s.size(), s2.size()));
	for (uint8_t i = 0; i < a.size(); i++) {
		a[i] = s[i] ^ s2[i];
	}
	return a;
}

std::string AES256CBC::pad(std::string plain)
{
	uint8_t paddingLen = 16 - (plain.size() % 16);
	std::string padding(paddingLen, static_cast<char>(paddingLen));
	return plain + padding;
}

std::string AES256CBC::unpad(std::string padded)
{
	uint8_t len = static_cast<uint8_t>(padded.back());
	for (uint8_t i = 0; i < len; i++) {
		if (static_cast<uint8_t>(padded[padded.size() - 1 - i]) != len) {
			std::cout << "Invalid padding!\n";
			return "";
		}
	}


	std::string plain(padded.begin(), padded.end() - len);
	return plain;
}

std::vector<std::vector<uint8_t>> AES256CBC::splitToBlocks(std::string message)
{
	std::vector<std::vector<uint8_t>> blocks;
	for (uint8_t i = 0; i < message.size(); i+=16) {
		blocks.push_back(parseString(message.substr(i, 16)));
	}

	return blocks;
}

std::vector<uint8_t> AES256CBC::parseString(std::string v)
{
	std::vector<uint8_t> ans;
	for (char c : v) {
		ans.push_back(static_cast<uint8_t>(c));
	}
	return ans;
}

std::vector<uint8_t> AES256CBC::parseMatrixToVector(std::vector<std::vector<uint8_t>> s)
{
	std::vector<uint8_t> v(16);
	for (uint8_t i = 0; i < s.size(); i++) {
		for (uint8_t j = 0; j < s[i].size(); j++) {
			v[i*4 + j] = s[i][j];
		}
	}
	return v;
}

std::vector<std::vector<uint8_t>> AES256CBC::parseVectorToMatrix(std::vector<uint8_t> v)
{
	std::vector<std::vector<uint8_t>> m(4, std::vector<uint8_t>(4));

	for (uint8_t i = 0; i < 16; i++) {
		m[i / 4][i % 4] = v[i];
	}
	return m;
}

std::string AES256CBC::parseMatrix(std::vector<std::vector<uint8_t>> m)
{
	std::string final;
	for (uint8_t i = 0; i < m.size(); i++) {
		std::ostringstream block;
		for (uint8_t num : m[i]) {
			block << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(num);
		}
		
		final += block.str();
	}
	return final;
}

std::vector<std::vector<uint8_t>> AES256CBC::parseInitialKey(cpp_int key)
{
	std::vector<uint8_t> byteArray;
	export_bits(key, std::back_inserter(byteArray), 8);

	int padding_needed = (32 - byteArray.size() % 32) % 32;
	byteArray.insert(byteArray.end(), padding_needed, 0);

	if (byteArray.size() > 32) {
		byteArray.resize(32);
	}

	std::vector<std::vector<uint8_t>> byteMatrix(60);
	for (size_t i = 0; i < 8; i++) {
		byteMatrix[i] = byteArray;
	}

	return byteMatrix;
}

std::string AES256CBC::ByteHex(int val)
{
	std::stringstream ss;
	ss << std::hex << val;
	return ss.str();
}

std::string AES256CBC::repeatString(std::string val, uint8_t amt)
{
	std::string repeat;
	for (uint8_t i = 0; i < amt; i++) {
		repeat += val;
	}
	return repeat;
}

