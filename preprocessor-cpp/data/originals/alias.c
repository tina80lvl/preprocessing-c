#define f(x) 3 * x
#define g f

#define e(x) 6 - x
#define q(x) e(x)

#define r(x) r(2 * (x))

f(9);
g(8);

e(2);
q(2);

r(4);

r(u+1) + r(r(7))