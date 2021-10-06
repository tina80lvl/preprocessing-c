#include <cstdio>

#define K 100
#define N K
#define N1 100
#define N3 100
#define N_ 100
#define PN printf("\n")
#define SUM for(i=0; i<N; i++) sum += i
#define FUNC(x, y) ((x)+(y))

int main () {
    int i, sum = 0;

    SUM;
    printf("%dN", sum);
    PN;
    int u = FUNC(1,2);
    printf("%d", u);
}