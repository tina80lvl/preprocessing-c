#include <cstdio>

#define K 100
#define N K
#define SUM for(i=0; i<N; i++) sum += i

int main () {
    int i, sum = 0;

    SUM;
    printf("%dN", sum);
}