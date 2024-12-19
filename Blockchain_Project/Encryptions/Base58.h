#pragma once
#include <vector>
#include <iostream>
#include "../Point.h"


class Base58
{
public:
	Base58() = default;
	std::vector<char> vect = {
		'1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'J', 'K', 'L', 'M', 'N', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'
	};

	std::string base58(cpp_int inp);
	cpp_int mod(cpp_int a, cpp_int m) {
		return (a % m + m) % m;
	}
};

