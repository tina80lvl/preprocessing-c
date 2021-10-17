#include <iostream>

#undefine

#define F(x,y) x + y
#define G(x,y) x + y
#define D G
#define H( x,    tttt  ,  r5)    (x) - tttt +5 *r5
#define PARAMS(x, ...)    x + __VA_ARGS__


int main()
{
    F(1,    2);
    F(1); // shouldn't be replaced, error thrown
    int b = F(1) + 12; // shouldn't be replaced, error thrown
    G(  3  , 4);
    H(45, 33, 12);
    K("not exist");
    PARAMS(1, 2);
    PARAMS(1, 2, 3);
    PARAMS(1, 2, 3, 4);
    D;
    D(5,7);

    return 0;
}