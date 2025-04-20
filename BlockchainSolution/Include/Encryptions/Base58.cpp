#include "pch.h"
#include "Base58.h"

std::string Base58::base58(cpp_int inp)
{
    std::string output;

    while (inp > 0) {
        output += vect[int(mod(inp, cpp_int(58)))];
        inp /= 58;
    }
    std::reverse(output.begin(), output.end());
    return output;
}
