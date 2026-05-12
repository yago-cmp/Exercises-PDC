#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// simulacao de produto escalar fracionado ! 0 -> 1, 2, 3 -> 0

double escalar(double x[], double y[], int n){
    double result = 0;
    for(int i=0; i < n; i++)
        result += x[i] * y[i];
    return result;
}

int main(int argc, char** argv) {
    int tagX = 0;
    int tagY = 1;
    int tagR = 2;

    int rank;
    int tam;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tam); 

    int N = atoi(argv[1]);
    if(N % tam != 0){
        printf("Insira um Tamanho de vetor divisivel pelo No de Processos!!! \n");
        return 1;
    }
    int parte = N/tam;

    double result = 0;

    if (rank == 0) {
        double recebido = 0;
        double x[N];
        double y[N];

        for(int i = 0; i < N; i++){
            x[i] = 1;
            y[i] = 1;
        }
        for (int i = 1; i < tam; i++){
            MPI_Send(&x[(i)*(parte)], parte, MPI_DOUBLE, i, tagX, MPI_COMM_WORLD);
            MPI_Send(&y[(i)*(parte)], parte, MPI_DOUBLE, i, tagY, MPI_COMM_WORLD);
        }

        result += escalar(x, y, parte);

        for (int i = 1; i < tam; i++){    
            MPI_Recv(&recebido, 1, MPI_DOUBLE, MPI_ANY_SOURCE, tagR, MPI_COMM_WORLD, &status);
            result += recebido;
        }

        printf("O resultado é: %f\n", result);
    }
    else {
        double x[parte];
        double y[parte];

        MPI_Recv(x, parte, MPI_DOUBLE, 0, tagX, MPI_COMM_WORLD, &status);
        MPI_Recv(y, parte, MPI_DOUBLE, 0, tagY, MPI_COMM_WORLD, &status);

        result = escalar(x, y, parte);

        MPI_Send(&result, 1, MPI_DOUBLE, 0, tagR, MPI_COMM_WORLD);
        printf("O processo %d enviou %f\n", rank, result);
    }
    MPI_Finalize();
    return 0;
}
