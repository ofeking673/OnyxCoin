#pragma once
#include <string>
#include <sstream>

class TxOutput
{
public:
	TxOutput(uint64_t value, std::string scriptPubKey) : _value(value), _scriptPubKey(scriptPubKey) {};
	~TxOutput() {};

	// Getters
	uint64_t getValue() const { return _value; };
	std::string getScriptPubKey() const { return _scriptPubKey; };

	bool operator==(const TxOutput& other) {
		return(this->getScriptPubKey() == other.getScriptPubKey() && this->getValue() == other.getValue());
	}

	// Print TxOutput
	friend std::ostream& operator<<(std::ostream& os, TxOutput txout)
	{
		os << "TxOutput(";
		os << "value: " << txout.getValue() << ", ";
		os << "scriptPubKey: " << txout.getScriptPubKey();
		os << ")";
		return os;
	}

	bool operator==(const TxOutput& other) const;
private:
	uint64_t _value;
	std::string _scriptPubKey;
};
