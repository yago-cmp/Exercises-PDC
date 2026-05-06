#include <omp.h>
#include <stdio.h>

int main(){

    int num = omp_get_num_threads();
    int np;
    printf("Total antes: %d\n", num);
    #pragma omp parallel private(np) firstprivate(num)
    {
       num = omp_get_num_threads();
       np = omp_get_thread_num();
       printf("Thread %d (Total %d) \n", np, num);
    }
    return 0;
}