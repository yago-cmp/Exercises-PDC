#include <omp.h>
#include <stdio.h>

int main(){
    int csum = 0;
    int i = 0;
    int np;
    int N = 5;
    int a[5] = {1, 2, 3, 4, 5};
    int b[5] = {1, 2, 3, 4, 5};    
    #pragma omp parallel private(i, np) shared(a,b,N) reduction(+:csum)
    {
       int num = omp_get_num_threads();
       int np = omp_get_thread_num();
        for(i = (i*(N/num)); i < (i+1)*(N/num); i++){
            csum += a[i] + b[i];
        }
    }
    printf("Csum: %d\n", csum);
    return 0;
}