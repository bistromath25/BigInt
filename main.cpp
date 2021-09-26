#include "BigInt.hpp"
using namespace std;

int main() {

    cout << "Starting some asserts...\n";
    BigInt A("1234567890123456789012345678901234567890");
    assert(A.length() == 40);

    BigInt B("1000000000000000000000000000000000000000");
    assert(B.length() == 40);

    BigInt C = 69;
    assert(C.length() == 2);

    BigInt D = -A;
    assert(D.length() == 40);

    assert(D != A);

    cout << A + B << endl;
    assert(A + B == BigInt("2234567890123456789012345678901234567890"));
    cout << B - A << endl;
    assert(B - A == BigInt("-234567890123456789012345678901234567890"));

    assert(A > B + C);

    assert(A > B && B < A);
    assert(A >= B && B <= A);

    BigInt user_input;
    cout << "Enter a BigInt: ";
    cin >> user_input;
    cout << "Your BigInt is: ";
    cout << user_input << endl;

    cout << "Finished all the tests" << endl;
    return 0;
}
