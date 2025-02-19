#pragma once
#include "OutPoint.h"

// A custom hash for OutPoint so it can be used in unordered map
struct OutPointHash
{
	size_t operator()(const OutPoint& op) const
	{
		std::hash<std::string> str_hash;
		return str_hash(op.getTxID()) ^ (std::hash<uint32_t>()(op.getIndex()) << 1);
	}
};