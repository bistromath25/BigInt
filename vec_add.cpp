#include <bits/stdc++.h>
using namespace std;

#define all(x) x.begin(), x.end()

vector<int> vector_addition(vector<int> &a, vector<int> &b) {
    if (a.size() > b.size()) {
        return vector_addition(b, a);
    }
    int a_length = a.size(), b_length = b.size();
    vector<int> result;
    reverse(all(a));
    reverse(all(b));
    int carry = 0;
    for (int i = 0; i < a_length; ++i) {
        int t = a[i] + b[i] + carry;
        result.push_back(t % 10);
        carry = t / 10;
    }
    for (int i = a_length; i < b_length; ++i) {
        int t = b[i] + carry;
        result.push_back(t % 10);
        carry = t / 10;
    }
    if (carry) {
        result.push_back(carry);
    }
    reverse(all(result));
    return result;
}

int main() {
    vector<int> a = {1, 1, 1, 1, 0};
    vector<int> b = {1, 0, 0, 9};
    vector<int> res = vector_addition(a, b);
    for (const int &x : res) {
        cout << x << ' ';
    }
    return 0;
}
