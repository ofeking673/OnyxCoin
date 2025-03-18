#include "pch.h"
#include "ECDSASigner.h"

ECDSASigner::ECDSASigner() {
    key = new KeyGenerator();
}

Point* ECDSASigner::signMessage(cpp_int priv_key, std::string message)
{
    std::string e = SHA256::digest(message);
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





Point* parseSignaturePoint(const std::string& signatureHex)
{
    // Suppose signatureHex is "abcd... (64 hex chars) ..."
    // You need to split it into r & s.
    // This is just an example. Adjust to how you actually store them.
    if (signatureHex.size() < 64) {
        return nullptr;
    }

    // Let's say half is r, half is s. 
    // Real code would handle odd-length, prefix, etc.
    std::string rHex = signatureHex.substr(0, signatureHex.size() / 2);
    std::string sHex = signatureHex.substr(signatureHex.size() / 2);

    cpp_int rInt = ECDSASigner::hexStringToCppInt(rHex);
    cpp_int sInt = ECDSASigner::hexStringToCppInt(sHex);

    return new Point(rInt, sInt);
}

Point* parsePublicKeyPoint(const std::string& pubKeyHex)
{
    // If publicKey is stored as X||Y in hex, we do similarly
    if (pubKeyHex.size() < 128) {
        return nullptr;
    }

    std::string xHex = pubKeyHex.substr(0, pubKeyHex.size() / 2);
    std::string yHex = pubKeyHex.substr(pubKeyHex.size() / 2);

    cpp_int xInt = ECDSASigner::hexStringToCppInt(xHex);
    cpp_int yInt = ECDSASigner::hexStringToCppInt(yHex);

    return new Point(xInt, yInt);
}






std::string ECDSASigner::cppIntToHexString(const cpp_int& v)
{
    std::stringstream ss;
    ss << std::hex << v;
    return ss.str();
}

cpp_int ECDSASigner::hexStringToCppInt(const std::string& hex)
{
    //return cpp_int("0x" + hex);
    cpp_int num;
    std::stringstream ss(hex);
    ss >> std::hex >> num;
    return num;
}