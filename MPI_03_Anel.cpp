#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// simulacao de anel ! 0 -> 1 -> 2 -> 3 -> 0

int main(int argc, char** argv) {
    int tag = 0;
    int rank;
    int tam;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tam); 

    int rec = 0;
    int number = rand() % 100;

    if (rank == 0) {
        MPI_Send(&number, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD);
        printf("Processo %d enviou %d para o processo %d\n", rank, number, rank+1);  
    }
    else{
        MPI_Recv(&rec, 1, MPI_INT, rank-1, tag, MPI_COMM_WORLD, &status);
        printf("Processo %d recebeu %d do processo %d\n", rank, rec, rank-1);  
        rec = rec + number;
        if(rank == tam-1){
            MPI_Send(&rec, 1, MPI_INT, 0, tag, MPI_COMM_WORLD);
            printf("Processo %d enviou %d para o processo %d\n", rank, rec, 0);   
        }
        else{
            MPI_Send(&rec, 1, MPI_INT, rank+1, tag, MPI_COMM_WORLD);
            printf("Processo %d enviou %d para o processo %d\n", rank, rec, rank+1);   
        }
    }

    if(rank == 0){
        MPI_Recv(&rec, 1, MPI_INT, tam-1, tag, MPI_COMM_WORLD, &status);
        printf("Processo %d recebeu %d do processo %d\n", rank, rec, tam-1);     
    }

    MPI_Finalize();
    return 0;
}