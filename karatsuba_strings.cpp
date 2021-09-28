#include <bits/stdc++.h>
using namespace std;

// Karatsuba's multiplication algorithm for strings

string string_karatsuba_multiply(string a, string b) {
    int a_length = a.length();
    int b_length = b.length();
    if (a_length == 0 || b_length == 0) {
        return "0";
    }

    vector<int> result(a_length + b_length, 0);

    int idx_a = 0, idx_b = 0;
    for (int i = a_length - 1, carry = 0; i >= 0; i--) {
        int x = a[i] - '0';
        idx_b = 0;

        for (int j = b_length - 1; j >= 0; j--) {
            int y = b[j] - '0';
            int res = x * y + result[idx_a + idx_b] + carry;
            carry = res / 10;
            result[idx_a + idx_b] = res % 10;
            idx_b++;
        }

        if (carry) {
            result[idx_a + idx_b] += carry;
        }

        ++idx_a;
    }

    int i = result.size() - 1;
    while (i >= 0 && result[i] == 0) {
        i--;
    }

    if (i == -1) {
        return "0";
    }

    string ans = "";
    while (i >= 0) {
        ans += to_string(result[i--]);
    }
    return ans;
}

int main() {

    string a, b;
    cin >> a >> b;
    cout << string_karatsuba_multiply(a, b);
    return 0;
}
