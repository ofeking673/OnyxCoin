#include "KeyGenerator.h"

KeyGenerator::KeyGenerator()
{
    PCurve = cpp_int("0xFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEFFFFFC2F");
    GPoint = new Point(cpp_int("0x79be667ef9dcbbac55a06295ce870b07029bfcdb2dce28d959f2815b16f81798"),
                       cpp_int("0x483ada7726a3c4655da4fbfc0e1108a8fd17b448a68554199c47d08ffb10d4b8"));
    n = cpp_int("0xfffffffffffffffffffffffffffffffebaaedce6af48a03bbfd25e8cd0364141");
}

cpp_int KeyGenerator::generatePrivate() {
    cpp_int num;
    std::mt19937_64 gen(std::random_device{}());

    do {
        for (int i = 0; i < 2; i++)
        {
            num << 64;
            num += gen();
        }
    } while (num > n || num < 0);

    return num;
}

cpp_int KeyGenerator::cat(cpp_int x, cpp_int y)
{
    return cpp_int(x.str() + y.str());
}

Point* KeyGenerator::ECAdd(Point* pnt1, Point* pnt2)
{
    if (pnt1->is_infinity()) return pnt2;
    if (pnt2->is_infinity()) return pnt1;
    if (pnt1->_x == pnt2->_x && pnt1->_y == pnt2->_y) return ECDouble(pnt1);
    if (pnt1->_x == pnt2->_x && pnt1->_y != pnt2->_y) return new Point(0, 0); // Point at infinity

    cpp_int DeltaY = mod((pnt2->_y - pnt1->_y), PCurve);
    cpp_int DeltaX = mod((pnt2->_x - pnt1->_x), PCurve);
    cpp_int lambda = mod(DeltaY * mod_inverse(DeltaX, PCurve), PCurve);

    cpp_int x3 = mod((lambda * lambda - pnt1->_x - pnt2->_x), PCurve);
    cpp_int y3 = mod((lambda * (pnt1->_x - x3) - pnt1->_y), PCurve);
    return new Point(x3, y3);
}

Point* KeyGenerator::ECDouble(Point* pnt) {
    if (pnt->is_infinity()) return new Point(0, 0); // Point at infinity

    cpp_int lambda = mod(mod((3 * (pnt->_x * pnt->_x) + ACurve), PCurve)
        * mod_inverse(2 * pnt->_y, PCurve), PCurve);

    cpp_int x3 = mod((lambda * lambda - 2 * pnt->_x), PCurve);
    cpp_int y3 = mod((lambda * (pnt->_x - x3) - pnt->_y), PCurve);
    

    return new Point(x3, y3);
}

Point* KeyGenerator::ECMul(cpp_int privKey, Point* point)
{
    if (privKey == 0 || privKey > n)
    {
        throw new std::exception("Invalid private key!\n");
    }

    Point* Q = new Point(0, 0); //infinity point
    Point* pnt = new Point(point->_x, point->_y);
    while(privKey != 0) //2 5 -> x2 +1 //101 -> 1 + 2x2
    {
        if (privKey % 2) {
            Q = ECAdd(Q, pnt);
        }

        pnt = ECDouble(pnt);
        //std::cout <<"Value: " << i << "G X: " << pnt->_x << " Y: " << pnt->_y << std::endl;
        //i *= 2;
        privKey /= 2;
    }

    return Q;
}
