#include <iostream>

#undefine



int main()
{
    1 + 2;
    F(1); // shouldn't be replaced, error thrown
    int b = F(1) + 12; // shouldn't be replaced, error thrown
    3 + 4;
    (45) - 33 +5 *12;
    K("not exist");
    1 + 2;
    1 + 2,3;
    1 + 2,3,4;
    G;
    5 + 7;

    return 0;
}
