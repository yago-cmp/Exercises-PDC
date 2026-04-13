#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <sys/time.h>

struct timeval tstart, tend;

double a[2000][2000];
double b[2000][2000];
double c[2000][2000];

int main(int argc, char * argv[])
{
    int N;
    int i, j, k;
    
    if (argc > 1) N = atoi(argv[1]);
    else N = 4;
    
    for (i=0 ;i<N; i++)  
        for (j=0 ;j<N; j++) {
            c[i][j] = 0;
            a[i][j] = 1; //drand48();
            b[i][j] = 1; //drand48();
        }

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

    printf("%ld", tempo);

    /*for (i=0; i<N; i++)
        for (j=0; j<N; j++)
            printf("C[%d][%d] = %f\n", i, j, c[i][j]);*/
}
