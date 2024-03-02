#include <iostream>
#include <string>
using namespace std;

/*int gcd(int a, int b) {
    if (a == 0) return b;
    if (b == 0) return a;
    int ta = max(a, b);
    int tb = a + b - ta;
    int r = ta % tb;
    while (r > 0) {
        ta = tb;
        tb = r;
        r = ta % tb;
    }
    return tb;
    /*if (a == 0 || b == 0) {
        return 0;
    }
    if (a > b) {
        if (a % b == 0) { return b; }
        else { return gcd(b, a % b); }
    }
    else if (b > a) {
        if (b % a == 0) { return a; }
        else { return gcd(a, b % a); }
    }
    else { return a; } 
}
*/

/*class Rational {
public:
    int a, b;
    bool neg;
    Rational(int numerator = 1, int denominator = 1) {
        a = numerator;
        b = denominator;
        neg = false;
        simplify();
    }
    string toString() {
        if (neg) {
            return '-' + to_string(a) + '/' + to_string(b);
        }
        return to_string(a) + '/' + to_string(b);
    }
    bool isEquals(Rational other) {
        return a == other.a && b == other.b && neg == other.neg;
    }
    Rational add(Rational other) {
        return Rational((a * other.b) + (b * other.a), b * other.b);
    }
    Rational multiply(Rational other) {
        return Rational(a * other.a, b * other.b);
    }
    Rational add_inverse() {
        return Rational(a * -1, b);
    }
    Rational mult_inverse() {
        return Rational(b, a);
    }
    Rational subtract(Rational other) {
        return add(other.add_inverse());
    }
    Rational divide(Rational other) {
        return multiply(other.mult_inverse());
    }
    void simplify() {
        if (b == 0) { a = 1; }
        else if (a == 0) { b = 1; }
        else {
            if (a < 0) {
                neg = !neg;
                a *= -1;
            }
            if (b < 0) {
                neg = !neg;
                b *= -1;
            }
            int thisgcd = gcd(a, b);
            a /= thisgcd;
            b /= thisgcd;
        }
    }
};*/

// Over field 256
const int field = 256;

class Polynomial {
public:
    static const int polynomialMaxSize = 200;
    int coeff[polynomialMaxSize];
    int degree;

    Polynomial(int coIn[polynomialMaxSize]);

    Polynomial();

    int getDegree();

    string toString();

    Polynomial add(const Polynomial& other);

    Polynomial add_inverse() const;

    Polynomial subtract(const Polynomial& other);

    Polynomial multiply_x(int power);
};