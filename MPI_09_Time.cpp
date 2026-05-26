#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// medicao de tempo de uma mensagem


int main(int argc, char** argv) {
    int tag = 0;
    int tagN = 1;
    int tagQ = 2;
    int rank;
//    int tam;

    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
//    MPI_Comm_size(MPI_COMM_WORLD, &tam); 

    int N = 0;
    int qtd = 0;
    double* envio;
    double* recebido;

    if (rank == 0){
        N = atoi(argv[1]);
        qtd = atoi(argv[2]);
        MPI_Send(&N, 1, MPI_INT, 1, tagN, MPI_COMM_WORLD);
        MPI_Send(&qtd, 1, MPI_INT, 1, tagQ, MPI_COMM_WORLD);

        envio = (double*)malloc(N*sizeof(double));
        for(int i = 0; i < N; i++) // preenche o vetor
            envio[i] = 1.0;
    }
    else{
        MPI_Recv(&N, 1, MPI_INT, 0, tagN, MPI_COMM_WORLD, &status);
        MPI_Recv(&qtd, 1, MPI_INT, 0, tagQ, MPI_COMM_WORLD, &status);
        envio = (double*)malloc(N*sizeof(double));
    }

    if(rank == 0){ 
        double start, end;
        double tempo = 0.0;
        double tempo_loc = 0.0;
            start = MPI_Wtime();
        for(int i = 0; i < qtd; i++){
                MPI_Send(envio, N, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD);
                MPI_Recv(envio, N, MPI_DOUBLE, 1, tag, MPI_COMM_WORLD, &status);
        }
        end = MPI_Wtime();
        tempo = (end-start)/(qtd*2);
        //fim do loop
        tempo = tempo/qtd;
        printf("O tempo medio e: %f\n", tempo);
    }
    else{
        for(int i = 0; i < qtd; i++){
            MPI_Recv(envio, N, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD, &status);
            MPI_Send(envio, N, MPI_DOUBLE, 0, tag, MPI_COMM_WORLD);
        }
    }

    MPI_Finalize();
    return 0;

}
