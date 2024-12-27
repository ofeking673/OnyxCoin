#pragma once
#include "OutPoint.h"

// A custom hash for OutPoint so it can be used in unordered map
class OutPointHash
{
public:
	OutPointHash(const OutPoint& op)
	{
		std::hash<std::string> str_hash;
		_hash = (str_hash(op.getTxID()) ^ (std::hash<uint32_t>()(op.getIndex()) << 1));
	}

	size_t getHash() const { return _hash; };
private:
	size_t _hash;
};

