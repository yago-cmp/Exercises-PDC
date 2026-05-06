#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#define N 1000000

double a[N];
double b[N];

struct timeval tstart, tend;

int main(void)
{
    double sum;
    int i;

  gettimeofday(&tstart, NULL);
        for (i=0; i < N; i++){
            a[i] = 0.5 * i;
            b[i] = 1.0 * i;
        }

    sum = 0.0;

        for (i = 0; i < N; i++) {
                sum += a[i] * b[i];
        }
    gettimeofday(&tend, NULL);

  long tempo = (tend.tv_sec * 1000000 + tend.tv_usec)-(tstart.tv_sec * 1000000 + tstart.tv_usec);

    printf("Resultado= %f\nTempo: %ld\n", sum, tempo);
    
    return 1;
}
