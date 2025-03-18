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
	
	
	//std::string ToStringNoSeperator(){
	//	std::string concatenated = HelperT::padString(_x.convert_to<std::string>(), 64) + HelperT::padString(_y.convert_to<std::string>(), 64);
	//	cpp_int result(concatenated);

	//	std::stringstream ss;
	//	ss << std::hex << result;
	//	return ss.str();
	//}


	//static Point* reverseToStringNoSeperator(const std::string& s);

	//std::string ToString() 
	//{ 
	//	std::stringstream ss;
	//	ss << std::hex << cpp_int(HelperT::padString(_x.convert_to<std::string>(), 64)) << '|' << std::hex << cpp_int(HelperT::padString(_y.convert_to<std::string>(), 64));
	//	return ss.str();
	//}

	//static Point* parseString(std::string str) 
	//{
	//	return new Point(cpp_int(str.substr(0, 64)), cpp_int(str.substr(64)));
	//}

	//static Point* parseHexString(std::string str)
	//{
	//	int len = str.find_first_of('|');
	//	return new Point(cpp_int("0x" + HelperT::padString(str.substr(0, len), 64)), cpp_int("0x" + HelperT::padString(str.substr(len+1), 64)));
	//}

	//static Point* parseHexStringNoSeperator(std::string str)
	//{
	//	int len = str.size() / 2;
	//	return new Point(cpp_int("0x" + HelperT::padString(str.substr(0, len), 64)), cpp_int("0x" + HelperT::padString(str.substr(len), 64)));
	//}
	//static Point* parseHexStringPoint(const std::string& hexString);









	/////////////////
    // Convert a Point into a hex string (format: "xHex:yHex")
	static std::string usePointToHex(const Point* p);

    // Convert a hex string back to a Point (expects format: "xHex:yHex")
	static Point* useHexToPoint(const std::string& hexStr);
};

