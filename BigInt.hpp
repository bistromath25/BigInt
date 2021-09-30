#ifndef BIG_INT_H
#define BIG_INT_H
#endif

#include <iomanip>
#include <iostream>
#include <sstream>

#include <math.h>
#include <algorithm>

#include <string>
#include <vector>

#include <assert.h>

using namespace std;

// TODO: support division

class BigInt {
    /* BigInt Class for Big Integers */

public:
    BigInt() : sign(1) {}

    /* Create BigInt from existing values */
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
    void operator = (const string& s) {
        BigInt result(s);
        *this = result;
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

    /* Multiplication with an int */
    void operator *= (int x) {
        if (x < 0) {
            sign = -sign;
            x = -x;
        }
        int n = digits.size();
        int carry = 0;
        for (int i = 0; i < n; ++i) {
            if (i == n) {
                digits.push_back(0);
            }
            long long t = digits[i] * (long long) x + carry;
            carry = (int) (t / BASE);
            digits[i] = (int) (t % BASE);
        }
        digits[n - 1] += carry;
        trim();
    }

    /* Multiplication with a long long */
    void operator *= (long long x) {
        if (x < 0) {
            sign = -sign;
            x = -x;
        }
        int n = digits.size();
        int carry = 0;
        for (int i = 0; i < n; ++i) {
            if (i == (int) digits.size()) {
                digits.push_back(0);
            }
            long long t = digits[i] * x + carry;
            carry = (int) (t / BASE);
            digits[i] = (int) (t % BASE);
        }
        digits[n - 1] += carry;
        trim();
    }

    /* Multiply two BigInts */
    vector<int> multiply(const vector<int> &a, const vector<int> &b) {
        int a_len = a.size(), b_len = b.size();
        vector<long long> result(a_len + b_len, 0);
        for (int i = 0, carry = 0; i < a_len; ++i) {
            for (int j = 0; j < b_len; ++j) {
                result[i + j] += (long long) a[i] * b[j];
                if (result[i + j] >= BASE) {
                    carry = result[i + j] / BASE;
                    result[i + j] %= BASE;
                    result[i + j + 1] += carry;
                }
            }
        }
        return to_vector_int(result);
    }

    BigInt operator * (int x) const {
        BigInt result = *this;
        result *= x;
        return result;
    }
    BigInt operator * (long long x) const {
        BigInt result = *this;
        result *= x;
        return result;
    }

    BigInt operator * (BigInt &x) {
        BigInt result = *this;
        if (result.sign != x.sign) {
            result.sign = -1;
        }
        else {
            result.sign = 1;
        }
        result.digits = multiply(digits, x.digits);
        result.trim();
        return result;
    }

    /* Shorthand methods */
    void operator += (const BigInt &x) {
        *this = *this + x;
    }
    void operator -= (const BigInt &x) {
        *this = *this - x;
    }
    void operator *= (BigInt &x) {
        *this = *this * x;
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

    /* Absolute value */
    BigInt abs() const {
        BigInt result = *this;
        result.sign *= result.sign;
        return result;
    }

    /* Change the sign of a BigInt */
    BigInt operator - () const {
        BigInt result = *this;
        result.sign = -sign;
        return result;
    }

    /* Number of digits */
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

    /* Removes excess space from digits */
    void trim() {
        while (!digits.empty() && !digits.back()) {
            digits.pop_back();
        }
        if (digits.empty()) {
            sign = 1;
        }
    }

    /* Create a vector<int> from a vector<long long> */
    vector<int> to_vector_int(const vector<long long> &x) {
        vector<int> result((int) x.size(), 0);
        for (int i = 0; i < (int) x.size(); ++i) {
            result[i] = (int) x[i];
        }
        return result;
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

    /* Convert BigInt to string */
    static string to_string(BigInt &x) {
        // BigInt::to_string(x)
        stringstream ss;
        ss << x;
        string result;
        ss >> result;
        return result;
    }
    string to_string() const {
        // a.to_string()
        stringstream ss;
        ss << *this;
        string result;
        ss >> result;
        return result;
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

    /* Print the contents of digits, for debugging purposes */
    void print_digits() {
        for (const int &d : digits) {
            cout << d << " ";
        }
        cout << '\n';
    }

private:
    /*
     * The BigInt is stored as a base-BASE number with each group of BASE_DIGITS stored as a single int in a vector<int>
     * The sign of the BigInt (positive or negative) is stored in int sign, with 1 meaning positive and -1 meaning negative
     */

    static const int BASE = 1000000000;
    static const int BASE_DIGITS = 9;
    vector<int> digits;
    int sign;
};
