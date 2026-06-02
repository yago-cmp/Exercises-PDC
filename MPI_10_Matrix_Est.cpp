#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int a[1000][1000];
int b[1000][1000];
int c[1000][1000];

int main(int argc, char * argv[])
{
    int tagA = 1; // enviar matriz A
    int tagB = 2; // enviar matriz B
    int tagC = 3; // enviar matriz C

    int N = 4;
    int i, j, k;

    int rank;
    int tam;
    int parte;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tam); 

    MPI_Status status;

    if (rank == 0){
        if (argc > 1) N = atoi(argv[1]);
        for (i=0 ;i<N; i++)  
            for (j=0 ;j<N; j++) {
                a[i][j] = 1; 
                b[i][j] = 1;
                c[i][j] = 0;
            }
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD); // envia e recebe N
    parte = N/tam; // calcula quantas linhas cada um vai receber

    MPI_Bcast(b, 1000*1000, MPI_INT, 0, MPI_COMM_WORLD); // envia e recebe b
    MPI_Scatter(a, 1000*parte, MPI_INT, a, 1000*parte, MPI_INT, 0, MPI_COMM_WORLD); // envia parte linhas da matriz a
 
    for (i=0; i<parte; i++)
        for (j=0; j<N; j++){
                c[i][j] = 0;
                for (k=0; k<N; k++) 
                    c[i][j] = c[i][j] + a[i][k]*b[k][j];
        }
            
    MPI_Gather(c, 1000*parte, MPI_INT, &c[rank*parte][0], 1000*parte, MPI_INT, 0, MPI_COMM_WORLD);
    
    if(rank ==0){
        printf("C[%d][%d] = %d\n", N-1, N-1, c[N-1][N-1]);
    }
    MPI_Finalize();
    return 0;
}
