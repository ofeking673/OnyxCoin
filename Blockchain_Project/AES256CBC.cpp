#include "AES256CBC.h"

AES256CBC::AES256CBC(std::string masterKey)
{
	keyMatrix = expand_key(masterKey);
}

std::string AES256CBC::cbcEncrypt(std::string plain_text, std::vector<int> iv)
{
	if (plain_text.size() % 16) {
		pad(plain_text);
	}

	std::vector<std::vector<int>> blocks;
	std::vector<int> prevCipher = iv;

	for (auto block : splitToBlocks(plain_text)) {
		auto xord = xorBytes(block, prevCipher);
		auto cipher_block = encrypt(xord);

		blocks.push_back(cipher_block);
		prevCipher = cipher_block;
	}

	return parseMatrix(blocks);
}

std::vector<int> AES256CBC::encrypt(std::vector<int> val)
{
	assert(val.size() == 16);

	auto plain_state = parseVectorToMatrix(val);
	add_round_key(plain_state, keyMatrix[0]);

	for (int i = 0; i < nround; i++) {
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

std::vector<std::vector<std::vector<int>>> AES256CBC::expand_key(std::string key)
{
	auto key_col = parseInitialKey(key);
	int iteration_size = key_col.size();

	for (int i = 0; i < 8; i++) {
		std::vector<int> word = key_col[i];
	}

	int j = 1;
	for (int i = 8; i < 60; i++) { //First 8 words already exist.
		std::vector<int> t = key_col[i - 1];

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

	std::vector<std::vector<std::vector<int>>> result;
	std::vector<std::vector<int>> group(4);

	for (int i = 0; i < key_col.size(); i++) {
		if (i % 4 == 0 && i!=0) {
			result.push_back(group);  // Add the 4x4 matrix to the result
			std::vector<std::vector<int>> group(4);
		}
		group[i%4] = key_col[i];
	}
	result.push_back(group);

	std::cout << result.size() << std::endl;
	for (int k = 0; k < result.size(); k++) {
		for (int i = 0; i < result[k].size(); i++) {
			for (int j = 0; j < result[k][i].size(); j++) {
				std::cout << std::hex << std::setw(2) << std::setfill('0') << result[k][i][j] << " ";
			}
			std::cout << "\n";
		}
	}
	return result;
}

std::vector<int> AES256CBC::subVector(std::vector<int> v)
{
	for (int n = 0; n < v.size(); n++) {
		v[n] = s_box[v[n]];
	}
	return v;
}

void AES256CBC::sub_bytes(std::vector<std::vector<int>>& matrix)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix[i][j] = s_box[matrix[i][j]];
		}
	}
}

void AES256CBC::inv_sub_bytes(std::vector<std::vector<int>>& matrix)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix[i][j] = inv_s_box[matrix[i][j]];
		}
	}
}

void AES256CBC::shift_rows(std::vector<std::vector<int>>& matrix)
{
	std::rotate(matrix[1].begin(), matrix[1].begin() + 1, matrix[1].end());

	// Row 3: Shift left by 2
	std::rotate(matrix[2].begin(), matrix[2].begin() + 2, matrix[2].end());

	// Row 4: Shift left by 3
	std::rotate(matrix[3].begin(), matrix[3].begin() + 3, matrix[3].end());
}

void AES256CBC::inv_shift_rows(std::vector<std::vector<int>>& matrix)
{
    /*    s[0][1], s[1][1], s[2][1], s[3][1] = s[3][1], s[0][1], s[1][1], s[2][1]
    s[0][2], s[1][2], s[2][2], s[3][2] = s[2][2], s[3][2], s[0][2], s[1][2]
    s[0][3], s[1][3], s[2][3], s[3][3] = s[1][3], s[2][3], s[3][3], s[0][3]*/

	std::rotate(matrix[1].begin(), matrix[1].begin() + (matrix.size() - 1), matrix[1].end());
	std::rotate(matrix[2].begin(), matrix[2].begin() + (matrix.size() - 2), matrix[2].end());
	std::rotate(matrix[3].begin(), matrix[3].begin() + (matrix.size() - 3), matrix[3].end());
}

int AES256CBC::gmul(int a, int b) //Almost like ecc point-scalar multiplication 5b = ((b*2)*2)+b
{
	int result = 0;
	int high_bit_mask = 0x80; // MSB in 8 bits
	int modulo = 0x1B; // AES irreducible polynomial

	for (int i = 0; i < 8; i++) {
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

void AES256CBC::mix_columns(std::vector<std::vector<int>>& matrix)
{
	std::vector<std::vector<int>> mulVect = { {2, 3, 1, 1}, {1, 2, 3, 1}, {1, 1, 2, 3}, {3, 1, 1, 2} };
	for (int i = 0; i < 4; i++) {
		mix_single_column(matrix[i], mulVect);
	}
}

void AES256CBC::mix_single_column(std::vector<int>& s, std::vector<std::vector<int>> mulVect)
{
	/*
	2  3  1  1
	1  2  3  1
	1  1  2  3
	3  1  1  2*/
	std::vector<int> temp(4);

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			temp[i] ^= gmul(s[j], mulVect[i][j]);
		}
	}

	for (int i = 0; i < 4; i++) {
		s[i] = temp[i];
	}
}

void AES256CBC::inv_mix_columns(std::vector<std::vector<int>>& matrix)
{
	/*0x0E  0x0B  0x0D  0x09
0x09  0x0E  0x0B  0x0D
0x0D  0x09  0x0E  0x0B
0x0B  0x0D  0x09  0x0E*/
	std::vector<std::vector<int>> mulVect = { {0x0E, 0x0B, 0x0D, 0x09}, 
											  {0x09, 0x0E, 0x0B, 0x0D}, 
											  {0x0D, 0x09, 0x0E, 0x0B},
											  {0x0B, 0x0D, 0x09, 0x0E} };

	for (int i = 0; i < 4; i++) {
		mix_single_column(matrix[i], mulVect);
	}
}

void AES256CBC::add_round_key(std::vector<std::vector<int>>& matrix, std::vector<std::vector<int>> roundkey)
{
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			matrix[i][j] = matrix[i][j] ^ roundkey[i][j];
		}
	}
}

std::vector<std::vector<int>> AES256CBC::textToMatrix(std::string text)
{
	std::vector<std::vector<int>> vec(4);
	for (int i = 0; i < 4; i++) {
		vec[i] = std::vector<int>(4);
	}

	for (int i = 0; i < 16; i++) {
		vec[i / 4][i % 4] = text[i];
	}
	return vec;
}

std::vector<int> AES256CBC::xorBytes(std::vector<int> s, std::vector<int> s2)
{
	std::vector<int> a(std::min(s.size(), s2.size()));
	for (int i = 0; i < a.size(); i++) {
		a[i] = s[i] ^ s2[i];
	}
	return a;
}

void AES256CBC::incBytes(std::vector<int>& s)
{
	for (int i = 0; i < s.size(); i++) {
		if (s[i] == 0xFF) {
			s[i] = 0;
		}
		else {
			s[i]++;
		}
	}
}

std::string AES256CBC::pad(std::string plain)
{
	int paddingLen = 16 - (plain.size() % 16);
	std::string padding = repeatString(ByteHex(paddingLen), paddingLen);
	return plain + padding;
}

std::string AES256CBC::unpad(std::string padded)
{
	char len = padded.back();
	for (int i = 0; i < len-'0'; i++) {
		if (padded[padded.size() - 1 - i] != len) {
			std::cout << "Invalid padding!\n";
			return "";
		}
	}

	std::string plain(padded.begin(), padded.end() - len);
	return plain;
}

std::vector<std::vector<int>> AES256CBC::splitToBlocks(std::string message, int block_size, bool requirePadding)
{
	if (message.size() % block_size) {
		message = pad(message);
	}
	std::vector<std::vector<int>> blocks;
	for (int i = 0; i < message.size() / block_size; i++) {
		blocks.push_back(parseString(message.substr(i * 16, 16)));
	}
	return blocks;
}

std::vector<int> AES256CBC::parseString(std::string v)
{
	std::vector<int> ans;
	for (char c : v) {
		ans.push_back(c - '0');
	}
	return ans;
}

std::vector<int> AES256CBC::parseMatrixToVector(std::vector<std::vector<int>> s)
{
	std::vector<int> v(16);
	for (int i = 0; i < s.size(); i++) {
		for (int j = 0; j < s[i].size(); j++) {
			v.push_back(s[i][j]);
		}
	}
	return v;
}

std::vector<std::vector<int>> AES256CBC::parseVectorToMatrix(std::vector<int> v)
{
	std::vector<std::vector<int>> m(4);
	for (int i = 0; i < 4; i++) {
		m[i] = std::vector<int>(4);
	}

	for (int i = 0; i < 16; i++) {
		m[i / 4][i % 4] = v[i];
	}
	return m;
}

std::string AES256CBC::parseMatrix(std::vector<std::vector<int>> m)
{
	std::string final;
	for (int i = 0; i < m.size(); i++) {
		std::string block;
		for (int j = 0; j < m[i].size(); j++) {
			block += std::to_string(m[i][j]);
		}
		auto start = block.find_first_not_of('0');
		if(start != std::string::npos)
			block = block.substr(start);
		else
		{
			block = "";
		}
		final += block;
	}
	return final;
}

std::vector<std::vector<int>> AES256CBC::parseInitialKey(std::string key)
{
	std::vector<std::vector<int>> k(60, std::vector<int>(4));
	for (int i = 0; i < key.size(); i++) {
		k[i / 4][i % 4] = key[i]-'0'; //{1, 2, 3, 4}
	}
	return k;
}

std::string AES256CBC::ByteHex(int val)
{
	std::stringstream ss;
	ss << std::hex << val;
	return ss.str();
}

std::string AES256CBC::repeatString(std::string val, int amt)
{
	std::string repeat;
	repeat.reserve(val.size() * amt);
	for (int i = 0; i < amt; i++) {
		repeat += val;
	}
	return repeat;
}

