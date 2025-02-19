#pragma once
#include <iostream>
#include <random>
#include "Point.h"
#include "SHA256.h"

//#define ACurve 0
//#define BCurve 7

class KeyGenerator
{
public:
	KeyGenerator();

	Point* ECAdd(Point* pnt, Point* pnt2);
	Point* ECMul(cpp_int privKey, Point* GPoint);
	Point* ECDouble(Point* pnt);

    cpp_int generatePrivate();

    static cpp_int mod_inverse(cpp_int a, cpp_int m) {
        cpp_int m0 = m, t, q;
        cpp_int x0 = 0, x1 = 1;

        if (m == 1) return 0; // Inverse does not exist

        // Extended Euclidean Algorithm
        while (a > 1) {
            // q is quotient
            q = a / m;

            t = m; // Store m
            m = mod(a, m); // Update a and m
            a = t;
            t = x0; // Update x0 and x1
            x0 = x1 - q * x0;
            x1 = t;
        }

        // Make x1 positive
        if (x1 < 0) x1 += m0;

        return x1;
    }

    static cpp_int mod(cpp_int a, cpp_int m) {
        return (a % m + m) % m;
    }

    static cpp_int cat(cpp_int x, cpp_int y);

	cpp_int PCurve;
    Point* GPoint;
	cpp_int n;
	cpp_int ACurve;
	cpp_int BCurve;
};




