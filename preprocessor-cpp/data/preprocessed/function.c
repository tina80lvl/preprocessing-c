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


    return 0;
}
