#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

int main(int argc, char *argv[]){
    int n, i = 0;
    double mypi, pi, h, sum, x, a;

    n = atoi(argv[1]);
    h = 1.0 / (double) n;
    sum = 0.0;
    #pragma omp parallel num_threads(4) private(i, x) reduction(+:sum)
    {
        int num = omp_get_num_threads();
        int thread = omp_get_thread_num();
        int inic = thread*(n/num) + 1;
        int fim = (thread+1)*(n/num);

        if(thread == num-1) fim = n;

        for(i = inic; i <= fim; i++){
            x = h * ((double) i - 0.5);
            sum += 4.0 / (1.0 + x*x);
        }
    }
    mypi = h * sum;

    printf("pi e aproximadamente %.16f .\n", mypi);

    return 0;
}