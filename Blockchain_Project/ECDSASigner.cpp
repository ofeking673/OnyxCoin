#include "ECDSASigner.h"

ECDSASigner::ECDSASigner() {
    _sha = new SHA256();
    key = new KeyGenerator();
}

Point* ECDSASigner::signMessage(cpp_int priv_key, std::string message)
{
    std::string e = _sha->digest(message);
    cpp_int k;
    Point* R;
    cpp_int r;
    do {
        k = generateNumber();
        R = key->ECMul(k, key->GPoint);
        r = R->_x;
    } while (r == 0);

    cpp_int s = KeyGenerator::mod((KeyGenerator::mod_inverse(k, key->n) * (cpp_int("0x" + e) + r * priv_key)), key->n);
    Point* signature = new Point(r, s);
    return signature;
}

bool ECDSASigner::verifySignature(Point* rs, std::string hexMessage, Point* publicKey)
{
    cpp_int s1 = KeyGenerator::mod(KeyGenerator::mod_inverse(rs->_y, key->n), key->n);
    cpp_int u1 = KeyGenerator::mod(cpp_int("0x" + hexMessage) * s1, key->n);
    cpp_int u2 = KeyGenerator::mod((rs->_x * s1), key->n);

    Point* pnt = key->ECAdd(key->ECMul(u1, key->GPoint), key->ECMul(u2, publicKey));

    return (pnt->_x == rs->_x);
}

cpp_int ECDSASigner::generateNumber()
{
    std::mt19937_64 gen(std::random_device{}());
    cpp_int num;
    for (int i = 0; i < 4; i++)
    {
        num << 64;
        num += gen();
    }
    return num;
}