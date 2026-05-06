#include <omp.h>
#include <stdio.h>

int main(){
    int num = omp_get_num_threads();
    printf("Total antes: %d\n", num);
    #pragma omp parallel
    {
       num = omp_get_num_threads();
       int np = omp_get_thread_num();
       printf("Thread %d (Total %d) \n", np, num);
    }
    return 0;
}