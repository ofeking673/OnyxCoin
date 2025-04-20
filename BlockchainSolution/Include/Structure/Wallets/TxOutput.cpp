#include "pch.h"
#include "TxOutput.h"

bool TxOutput::operator==(const TxOutput& other) const
{
    return _value == other._value && _scriptPubKey == other._scriptPubKey;
}
