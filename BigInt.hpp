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

using namespace std;

class BigInt {
public:
    BigInt() : sign(1) {}

    /* Constructors */
    BigInt(int x) {
        *this = x;
    }

    BigInt(long long x) {
        *this = x;
    }

    BigInt(const string &s) {
        read(s);
    }

    void operator = (int x) {
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

    void operator = (const BigInt &x) {
        sign = x.sign;
        digits = x.digits;
    }

    /* Addition function */
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

    /* Subtraction function */
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

    /* Multiplication functions */
    static vector<int> multiply(const vector<int> &a, const vector<int> &b) {
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

    BigInt operator * (const BigInt &x) const {
        BigInt result;
        result.digits = multiply(digits, x.digits);
        if (sign != x.sign) {
            result.sign = -1;
        }
        else {
            result.sign = 1;
        }
        result.trim();
        return result;
    }

    /* Division functions */
    friend pair<BigInt, BigInt> divide(const BigInt &dividend, const BigInt &divisor) {
        // return (quotient, remainder)
        BigInt a = dividend.abs() * (BASE / (divisor.digits.back() + 1));
        BigInt b = divisor.abs() * (BASE / (divisor.digits.back() + 1));
        BigInt quo, rem;
        quo.digits.resize(a.digits.size());

        for (int i = a.digits.size() - 1; i >= 0; i--) {
            rem *= BASE;
            rem += a.digits[i];
            int x = 0, y = 0;

            if (rem.digits.size() > b.digits.size()) {
                x = rem.digits[b.digits.size()];
            }
            if (rem.digits.size() > b.digits.size() - 1) {
                y = rem.digits[b.digits.size() - 1];
            }

            int d = ((long long) BASE * x + y) / b.digits.back();
            rem -= b * d;
            while (rem < 0) {
                rem += b;
                d--;
            }
            quo.digits[i] = d;
        }

        if (dividend.sign != divisor.sign) {
            quo.sign = -1;
        }
        rem.sign = dividend.sign;
        quo.trim();
        rem.trim();
        return make_pair(quo, rem);
    }

    BigInt operator / (int x) const {
        BigInt result = *this;
        result /= x;
        return result;
    }

    BigInt operator / (const BigInt &x) const {
        return divide(*this, x).first;
    }

    /* Remainder function */
    int operator % (int x) const {
        if (x < 0) {
            x = -x;
        }
        int result = 0;
        for (int i = digits.size() - 1; i >= 0; i--) {
            result = (digits[i] + result * (long long) BASE) % x;
        }
        if (sign == -1) {
            result = -result;
        }
        return result;
    }

    long long operator % (long long x) const {
        if (x < 0) {
            x = -x;
        }
        long long result = 0;
        for (int i = digits.size() - 1; i >= 0; i--) {
            result = (digits[i] + result * (long long) BASE) % x;
        }
        if (sign == -1) {
            result = -result;
        }
        return result;
    }

    BigInt operator % (const BigInt &x) const {
        return divide(*this, x).second;
    }

    /* Shorthand methods */
    void operator *= (int x) {
        if (x < 0) {
            sign = -sign;
            x = -x;
        }
        for (int i = 0, carry = 0; i < (int) digits.size() || carry; i++) {
            if (i == (int) digits.size()) {
                digits.push_back(0);
            }
            long long d = digits[i] * (long long) x + carry;
            carry = (int) (d / BASE);
            digits[i] = (int) (d % BASE);
        }
        trim();
    }

    void operator /= (int x) {
        if (x < 0) {
            sign = -sign;
            x = -x;
        }
        for (int i = (int) digits.size() - 1, rem = 0; i >= 0; i--) {
            long long d = digits[i] + rem * (long long) BASE;
            digits[i] = (int) (d / x);
            rem = (int) (d % x);
        }
        trim();
    }

    void operator *= (long long x) {
        if (x < 0) {
            sign = -sign;
            x = -x;
        }
        for (int i = 0, carry = 0; i < (int) digits.size() || carry; i++) {
            if (i == (int) digits.size()) {
                digits.push_back(0);
            }
            long long d = digits[i] * (long long) x + carry;
            carry = (int) (d / BASE);
            digits[i] = (int) (d % BASE);
        }
        trim();
    }

    void operator /= (long long x) {
        if (x < 0) {
            sign = -sign;
            x = -x;
        }
        for (int i = (int) digits.size() - 1, rem = 0; i >= 0; i--) {
            long long d = digits[i] + rem * (long long) BASE;
            digits[i] = (int) (d / x);
            rem = (int) (d % x);
        }
        trim();
    }

    void operator += (const BigInt &x) {
        *this = *this + x;
    }

    void operator -= (const BigInt &x) {
        *this = *this - x;
    }

    void operator *= (const BigInt &x) {
        *this = *this * x;
    }

    void operator /= (const BigInt &x) {
        *this = *this / x;
    }

    void operator %= (const BigInt &x) {
        *this = *this % x;
    }

    /* Compare BigInts */
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

    /* Change the sign of a BigInt */
    BigInt operator - () const {
        BigInt result = *this;
        result.sign = -sign;
        return result;
    }

    /* Absolute value of a BigInt */
    BigInt abs() const {
        BigInt result = *this;
        result.sign *= result.sign;
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

    /* Create a vector<int> from a vector<long long> */
    static vector<int> to_vector_int(const vector<long long> &x) {
        vector<int> result((int) x.size(), 0);
        for (int i = 0; i < (int) x.size(); ++i) {
            result[i] = (int) x[i];
        }
        return result;
    }

    /* Read in a string to BigInt */
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
        stringstream ss;
        ss << x;
        string result;
        ss >> result;
        return result;
    }

    string to_string() const {
        stringstream ss;
        ss << *this;
        string result;
        ss >> result;
        return result;
    }

    /* Remove leading zeroes in digits */
    void trim() {
        while (!digits.empty() && !digits.back()) {
            digits.pop_back();
        }
        if (digits.empty()) {
            sign = 1;
        }
    }

    /* Input and Output */
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
    /*
     * The BigInt is stored as a base-BASE number with each group of BASE_DIGITS digits stores as a single digit int in a vector<int>
     * The sign of the BigInt is stored in int sign, with 1 meaning positive and -1 meaning negative
     */
    static const int BASE = 1000000000;
    static const int BASE_DIGITS = 9;
    vector<int> digits;
    int sign;
};



