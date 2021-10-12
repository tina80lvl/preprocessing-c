#include <iostream>

#undefine

#define F(x,y) x + y
#define G(x,y) x + y
#define H( x,    tttt  ,  r5)    (x) - tttt +5 *r5


int main()
{
    F(1,    2);
    F(1); // shouldn't be replaced, error thrown
    int b = F(1) + 12; // shouldn't be replaced, error thrown
    G(  3  , 4);
    H(45, 33, 12);
    K("not exist");


    return 0;
}