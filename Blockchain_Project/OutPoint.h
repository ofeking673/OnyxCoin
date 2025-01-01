#pragma once
#include <string>
#include <sstream>

using TxID = std::string;

// Represents an out point - reference to a specific transaction output
class OutPoint
{
public:
	OutPoint(TxID txid, uint32_t index) : _txid(txid), _index(index) {};
	~OutPoint() {};

	// Getters
	TxID getTxID() const { return _txid; };
	uint32_t getIndex() const { return _index; };


	bool operator< (const OutPoint& other) const {
		return (this->getIndex() < other.getIndex());
	}

	bool operator==(const OutPoint& other) const
	{
		return (this->getTxID() == other.getTxID() && this->getIndex() == other.getIndex());
	}

	// Print OutPoint
	friend std::ostream& operator<<(std::ostream& os, OutPoint op)
	{
		os << "OutPoint(txid: " << op.getTxID() << ", index: " << op.getIndex() << ")";
		return os;
	}
private:
	TxID _txid;		// ID (hash) of the transaction
	uint32_t _index;	// Index of the output in that transaction
};

