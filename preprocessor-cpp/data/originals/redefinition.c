#include <iostream>

#define A "what does the fox say"
#define A "ratatata"
#define F(x) x
#define F(x) ---x---

int main()
{
    cout << "Size of char: " << sizeof(char) << " byte" << endl;
    cout << "Size of int: " << sizeof(int) << " bytes" << endl;
    cout << "Size of float: " << sizeof(float) << " bytes" << endl;
    cout << "Size of double: " << sizeof(double) << " bytes" << endl;

    cout << "define: " << A << endl;
    cout << "define: " << F("pip") << endl;

    return 0;
}