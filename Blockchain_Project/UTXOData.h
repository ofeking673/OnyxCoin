#pragma once
#include <string>

class UTXOData
{
public:
	UTXOData(uint64_t value, std::string scriptPubKey) :_value(value), _scriptPubKey(scriptPubKey) {};
	~UTXOData() {};

	uint64_t getValue() const { return _value; };
	std::string getScriptPubKey() const { return _scriptPubKey; };

private:
	uint64_t _value;
	std::string _scriptPubKey; // Locking script
};

