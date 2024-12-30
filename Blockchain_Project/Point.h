#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/integer/mod_inverse.hpp>
#include "HelperT.h"

using namespace boost::multiprecision;
using namespace boost::integer;

class Point
{
public:
	Point(cpp_int x, cpp_int y) : _x(x), _y(y) {};
	
	cpp_int _x;
	cpp_int _y;
	bool is_infinity() { return _x == 0 && _y == 0; }
	std::string ToStringNoPad(){
		std::stringstream ss;
		ss << std::hex << cpp_int((_x.convert_to<std::string>() + _y.convert_to<std::string>()));
		return ss.str();
	}

	std::string ToString() 
	{ 
		std::stringstream ss;
		ss << std::hex << cpp_int(HelperT::padString(_x.convert_to<std::string>(), 32) + HelperT::padString(_y.convert_to<std::string>(), 32));
		return ss.str();
	}

	static Point* parseString(std::string str) 
	{
		return new Point(cpp_int(str.substr(0, 64)), cpp_int(str.substr(64)));
	}

	static Point* parseHexString(std::string str)
	{
		return new Point(cpp_int("0x" + str.substr(0, 64)), cpp_int("0x" + str.substr(64)));
	}
};

