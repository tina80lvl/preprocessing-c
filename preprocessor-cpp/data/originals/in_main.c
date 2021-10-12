#include <iostream>
using namespace std;

int main()
{
    cout << "Size of char: " << sizeof(char) << " byte" << endl;
    cout << "Size of int: " << sizeof(int) << " bytes" << endl;
    cout << "Size of float: " << sizeof(float) << " bytes" << endl;
    #define N1 "line"
    cout << "Size of double: " << sizeof(double) << " bytes" << endl;
    cout << "define: " << N1 << endl;

    return 0;
}