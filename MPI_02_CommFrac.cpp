#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// simulacao de broadcast fracionado ! 0 -> 1, 2, 3

int main(int argc, char** argv) {
    int tag = 0;
    int rank;
    int size;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size); 

  
    int N = atoi(argv[1]);
    int parte = N/size;

    if (rank == 0) {
        int dados[N];
        for(int i = 0; i < N; i++)
            dados[i] = -1;
        for (int i = 1; i < size; i++)
            MPI_Send(&dados[(i)*(parte)], parte, MPI_INT, i, tag, MPI_COMM_WORLD);
    }
    else {
        int dados[N/size];
        MPI_Recv(dados,parte , MPI_INT, 0, tag, MPI_COMM_WORLD, &status);
        printf("Processo %d recebeu os %d dados do processo 0\n", rank, parte);
        if (dados[0] == -1 && dados[parte-1] == -1)
            printf("Dados corretos!!\n");
    }
    MPI_Finalize();
    return 0;
}