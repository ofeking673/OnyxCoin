#pragma once
#include "OutPoint.h"

class TxInput
{
public:
	TxInput(OutPoint previousOutPoint, std::string scriptSig) : _previousOutPoint(previousOutPoint), _scriptSig(scriptSig) {};
	~TxInput() {};

	OutPoint getPreviousOutPoint() const { return _previousOutPoint; };
	std::string getScriptSig() const { return _scriptSig; };

	void setScriptSig(const std::string& scriptSig) { _scriptSig = scriptSig; };

	// Print TxInput
	friend std::ostream& operator<<(std::ostream& os, TxInput txin)
	{
		os << "TxInput(";
		os << "previousOutpoint: " << txin.getPreviousOutPoint() << ", ";
		os << "scriptSig: " << txin.getScriptSig();
		os << ")";
		return os;
	}
private:
	OutPoint _previousOutPoint; // Which UTXO this input is spending
	std::string _scriptSig;
};

