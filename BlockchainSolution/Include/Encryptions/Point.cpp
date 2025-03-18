#include "pch.h"
#include "Point.h"

Point* Point::reverseToStringNoSeperator(const std::string& s)
{
	// Convert the hex string to a cpp_int.
	cpp_int combined;
	std::stringstream ss(s);
	ss >> std::hex >> combined;

	// When outputting, cpp_int will drop any leading zeros.
	// So we need to re-pad the hex string to ensure it is 128 characters.
	std::stringstream paddedStream;
	paddedStream << std::setw(128) << std::setfill('0') << std::hex << combined;
	std::string padded = paddedStream.str();

	// Split into two halves of 64 characters each.
	std::string xStr = padded.substr(0, 64);
	std::string yStr = padded.substr(64, 64);

	// Convert each hex string back to cpp_int.
	cpp_int x = ECDSASigner::hexStringToCppInt(xStr);
	cpp_int y = ECDSASigner::hexStringToCppInt(yStr);

	return new Point(x, y);
}

Point* Point::parseHexStringPoint(const std::string& hexString)
{
	if (hexString.size() != 128) {
		// Invalid format; expected exactly 128 hex characters.
		return nullptr;
	}

	// Split the string in half
	std::string firstHalf = hexString.substr(0, 64);
	std::string secondHalf = hexString.substr(64, 64);

	// Convert each half from hex to a cpp_int.
	cpp_int firstValue = ECDSASigner::hexStringToCppInt(firstHalf);
	cpp_int secondValue = ECDSASigner::hexStringToCppInt(secondHalf);

	return new Point(firstValue, secondValue);
}

std::string Point::usePointToHex(const Point* p)
{
	if (p == nullptr) {
		throw std::invalid_argument("Null pointer provided to pointToHex.");
	}

	std::stringstream ss;

	// Convert x coordinate to hex string
	ss << std::hex << p->_x;
	std::string xHex = ss.str();

	// Clear the stream for the next conversion
	ss.str("");
	ss.clear();

	// Convert y coordinate to hex string
	ss << std::hex << p->_y;
	std::string yHex = ss.str();

	// Combine both hex strings with a colon as delimiter
	return xHex + ":" + yHex;
}

Point* Point::useHexToPoint(const std::string& hexStr)
{
	size_t delimiterPos = hexStr.find(":");
	if (delimiterPos == std::string::npos) {
		throw std::invalid_argument("Invalid hex string format: missing ':' delimiter.");
	}

	// Create new Point on the heap.
	Point* p = new Point(0, 0);

	// Extract the substrings for x and y.
	std::string xHex = hexStr.substr(0, delimiterPos);
	std::string yHex = hexStr.substr(delimiterPos + 1);

	std::stringstream ss;

	// Convert x from hex string to cpp_int.
	ss << std::hex << xHex;
	ss >> p->_x;

	// Clear the stream before processing y.
	ss.clear();
	ss.str("");

	// Convert y from hex string to cpp_int.
	ss << std::hex << yHex;
	ss >> p->_y;

	return p;
}
