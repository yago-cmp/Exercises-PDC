/*
-_-_-_-_-_    TEMPOS    _-_-_-_-_-
 N     DINAMICA   ESTATICA   SEQUENCIAL
100      2325      1662       2930
        (1.22)    (1.76)
500      92958     78437      392233
        (4.22)    (5.00)
1000    956965    665288     3189331
        (3.33)     (4.79)
2000   10354305   7814793   47315258
        (4.56)      (6.05)
-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_
*/



#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

struct timeval tstart, tend;

int main(int argc, char * argv[])
{
    int N;
    int i, j, k;
    
    if (argc > 1) N = atoi(argv[1]);
    else N = 4;

    // ALOCACAO DINAMICA DAS 3 MATRIZES
    int **a = (int**)malloc(N*sizeof(int*));
    int **b = (int**)malloc(N*sizeof(int*));
    int **c = (int**)malloc(N*sizeof(int*));
    for (i=0 ;i<N; i++)  {
        a[i] = (int*)malloc(N*sizeof(int));
        b[i] = (int*)malloc(N*sizeof(int));
        c[i] = (int*)malloc(N*sizeof(int)); 
        for (j=0 ;j<N; j++) {
            c[i][j] = 0;
            a[i][j] = 1; 
            b[i][j] = 1; 
        }
    }
    // -----------------------------------


    gettimeofday(&tstart, NULL);

    #pragma omp parallel private(i,j,k)
    {
        #pragma omp for    
        for (i=0; i<N; i++) {
            for (j=0; j<N; j++){
                    for (k=0; k<N; k++) 
                        c[i][j] = c[i][j] + a[i][k]*b[k][j];
            }
        }

    }

   gettimeofday(&tend, NULL);
   long tempo = (tend.tv_sec * 1000000 + tend.tv_usec)-(tstart.tv_sec * 1000000 + tstart.tv_usec);

    printf("%ld microssecs\n", tempo);

    /*for (i=0; i<N; i++)
        for (j=0; j<N; j++)
            printf("C[%d][%d] = %f\n", i, j, c[i][j]);*/
}
