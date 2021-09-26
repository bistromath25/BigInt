#ifndef BIG_INT_H
#define BIG_INT_H
#endif

#include <iomanip>
#include <iostream>

#include <math.h>
#include <algorithm>

#include <string>
#include <vector>

#include <assert.h>

using namespace std;

// TODO: support multiplication and division

class BigInt {
    /* BigInt Class for Big Integers */

public:
    BigInt() : sign(1) {}

    /* number of digits */
    int length() {
        if (digits.empty()) {
            return 0;
        }
        else {
            int sz = (digits.size() - 1) * BASE_DIGITS;
            int d = digits.back();
            while (d) {
                sz++;
                d /= 10;
            }
            return sz;
        }
    }

    BigInt(long long x) {
        *this = x;
    }
    BigInt(const string &s) {
        read(s);
    }

    void operator = (const BigInt &x) {
        sign = x.sign;
        digits = x.digits;
    }
    void operator = (long long x) {
        sign = 1;
        if (x < 0) {
            sign = -1;
            x = -x;
        }
        while (x > 0) {
            digits.push_back(x % BASE);
            x /= BASE;
        }
    }

    /* BigInt addition */
    BigInt operator + (const BigInt &x) const {
        if (sign == x.sign) {
            BigInt result = x;
            for (int i = 0, carry = 0; i < (int) max(digits.size(), x.digits.size()) || carry; i++) {
                if (i == (int) result.digits.size()) {
                    result.digits.push_back(0);
                }
                if (carry) {
                    result.digits[i] += carry;
                }
                if (i < (int) digits.size()) {
                    result.digits[i] += digits[i];
                }
                carry = (result.digits[i] >= BASE);
                if (carry) {
                    result.digits[i] -= BASE;
                }
            }
            return result;
        }
        return *this - (-x);
    }

    /* BigInt subtraction */
    BigInt operator - (const BigInt &x) const {
        if (sign == x.sign) {
            // Subtract the smaller BigInt from the larger and modify the result's sign accordingly
            if (abs() >= x.abs()) {
                BigInt result = *this;
                for (int i = 0, carry = 0; i < (int) x.digits.size() || carry; i++) {
                    if (carry) {
                        result.digits[i] -= carry;
                    }
                    if (i < (int) x.digits.size()) {
                        result.digits[i] -= x.digits[i];
                    }
                    carry = result.digits[i] < 0;
                    if (carry) {
                        result.digits[i] += BASE;
                    }
                }
                result.trim();
                return result;
            }
            return -(x - *this);
        }
        return *this + (-x);
    }

    /* Change the sign of a BigInt */
    BigInt operator - () const {
        BigInt result = *this;
        result.sign = -sign;
        return result;
    }

    /* Absolute value */
    BigInt abs() const {
        BigInt result = *this;
        result.sign *= result.sign;
        return result;
    }

    /* Shorthand addition and subtraction */
    void operator += (const BigInt &x) {
        *this = *this + x;
    }
    void operator -= (const BigInt &x) {
        *this = *this - x;
    }

    /* Compare the values of BigInts */
    bool operator < (const BigInt &x) const {
        if (sign != x.sign) {
            return sign < x.sign;
        }
        if (digits.size() != x.digits.size()) {
            return digits.size() * sign < x.digits.size() * x.sign;
        }
        for (int i = (int) digits.size() - 1; i >= 0; i--) {
            if (digits[i] != x.digits[i]) {
                return digits[i] * sign < x.digits[i] * sign;
            }
        }
        return false;
    }
    bool operator > (const BigInt &x) const {
        return x < *this;
    }
    bool operator <= (const BigInt &x) const {
        return !(x < *this);
    }
    bool operator >= (const BigInt &x) const {
        return !(*this < x);
    }
    bool operator == (const BigInt &x) const {
        return !(*this < x) && !(x < *this);
    }
    bool operator != (const BigInt &x) const {
        // return !(*this == x);
        return (*this < x) || (x < *this);
    }

    /* removes excess space from digits */
    void trim() {
        while (!digits.empty() && !digits.back()) {
            digits.pop_back();
        }
        if (digits.empty()) {
            sign = 1;
        }
    }

    /* Read in a string and convert to BigInt */
    void read(const string &s) {
        sign = 1;
        digits.clear();
        int i = 0;
        while (i < (int) s.size() && (s[i] == '-' || s[i] == '+')) {
            if (s[i] == '-') {
                sign = -sign;
            }
            i++;
        }
        for (int j = (int) s.size() - 1; j >= i; j -= BASE_DIGITS) {
            int d = 0;
            for (int k = max(i, j - BASE_DIGITS + 1); k <= j; k++) {
                d *= 10;
                d += s[k] - '0';
            }
            digits.push_back(d);
        }
        trim();
    }

    /* Inputting and Outputting BigInt using "friend" functions and operator>> and operator<< */
    friend istream & operator >> (istream &in, BigInt &x) {
        string s;
        in >> s;
        x.read(s);
        return in;
    }
    friend ostream & operator << (ostream &out, const BigInt &x) {
        if (x.sign == -1) {
            out << '-';
        }

        if (x.digits.empty()) {
            out << 0;
        }
        else {
            out << x.digits.back();
        }

        for (int i = (int) x.digits.size() - 2; i >= 0; i--) {
            out << setw(BASE_DIGITS) << setfill('0') << x.digits[i];
        }

        return out;
    }

private:
    static const int BASE = 1000000000;
    static const int BASE_DIGITS = 9;

    vector<int> digits;
    int sign;
};

