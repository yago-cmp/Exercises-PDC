#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

int main(int argc, char * argv[])
{
    int **a;
    int **b;
    int **c;
    int **a_loc;
    int **c_loc;
    int *aa;
    int *bb;
    int *cc;
    int *aa_loc;
    int *cc_loc;

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
        if (argc > 1) N = atoi(argv[1]); // se existir parametro, N = parametro, se não já é 4

        aa = (int*)malloc(N*N*sizeof(int)); // aloca continuamente os dados
        bb = (int*)malloc(N*N*sizeof(int));
        cc = (int*)calloc(N*N, sizeof(int));

        a = (int**)malloc(N*sizeof(int*)); 
        b = (int**)malloc(N*sizeof(int*)); 
        c = (int**)malloc(N*sizeof(int*));

        for (i=0; i<N; i++){
            a[i] = &(aa[i*N]);
            b[i] = &(bb[i*N]); //referencia cada começo de linha para acesso por [][]
            c[i] = &(cc[i*N]);
        }

        for (i=0; i<N; i++)
            for (j=0 ;j<N; j++) { // preenche a e b com 1
                a[i][j] = 1;  
                b[i][j] = 1;
            }
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD); // envia e recebe N
    parte = N/tam; // calcula quantas linhas da matriz cada um vai calcular/receber

    aa_loc = (int*)malloc(parte*N*sizeof(int));
    cc_loc = (int*)calloc(parte*N, sizeof(int)); // cria continuo na memoria

    a_loc = (int**)malloc(parte*sizeof(int*));
    c_loc = (int**)malloc(parte*sizeof(int*)); // cria as matrizes locais para todos os processos

    for (i=0; i<parte; i++){
        a_loc[i] = &(aa_loc[i*N]); //referencia cada começo de linha para acesso por [][]
        c_loc[i] = &(cc_loc[i*N]);
    }

    MPI_Bcast(bb, N*N, MPI_INT, 0, MPI_COMM_WORLD); // todos recebem B inteiro
    MPI_Scatter(aa, N*parte, MPI_INT, aa_loc, N*parte, MPI_INT, 0, MPI_COMM_WORLD); // envia parte linhas da matriz a
 
    for (i=0; i<parte; i++)
        for (j=0; j<N; j++)
                for (k=0; k<N; k++) 
                    c_loc[i][j] = c_loc[i][j] + a_loc[i][k]*b[k][j];
            
    MPI_Gather(cc_loc, N*parte, MPI_INT, cc, N*parte, MPI_INT, 0, MPI_COMM_WORLD);
    
    if(rank ==0){
        printf("C[%d][%d] = %d\n", N-1, N-1, c[N-1][N-1]);
    }
    MPI_Finalize();
    return 0;
}
