#pragma once
#include <boost/multiprecision/cpp_int.hpp>
#include <boost/integer/mod_inverse.hpp>

using namespace boost::multiprecision;
using namespace boost::integer;

class Point
{
public:
	Point(cpp_int x, cpp_int y) : _x(x), _y(y) {};
	
	cpp_int _x;
	cpp_int _y;
	bool is_infinity() { return _x == 0 && _y == 0; }
};

