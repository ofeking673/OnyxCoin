#include "pch.h"
#include "TxInput.h"

bool TxInput::operator==(const TxInput& other) const
{
    return _scriptSig == other._scriptSig && _previousOutPoint == other._previousOutPoint;
}
