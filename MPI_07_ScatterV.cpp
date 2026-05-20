#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

// simulacao de produto escalar com com. coletiva usando Scatter V! 0 -> 1, 2, 3 -> 0

double escalar(double x[], double y[], int n){
    double result = 0;
    for(int i=0; i < n; i++)
        result += x[i] * y[i];
    return result;
}

int main(int argc, char** argv) {
    double* x;
    double* y;
    double* x_loc;
    double* y_loc;

    int rank;
    int tam;
    int parte;
    int resto;

    MPI_Status status;

    int* counts = NULL; // No de elementos pra cada processo
    int* displ = NULL; // offset de inicio pra cada processo
    int cont = 0; // para contar o offset

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tam); 

    double result = 0;
    double result_loc = 0;
    int N = 0;

    if(rank == 0){ // 0 preenche os vetores x e y, assim como o valor de N
        N = atoi(argv[1]);

        x = (double*)malloc(N*sizeof(double));
        y = (double*)malloc(N*sizeof(double));

        for(int i = 0; i < N; i++){
            x[i] = 1;
            y[i] = 1;
        }
    }

    MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD); // 0 envia e todos recebem N
    parte = N/tam; // todos calculam o valor da parte
    resto = N%tam; // todos calculam o valor do resto

    if (rank == 0){
        counts = (int*)malloc(tam*sizeof(int)); // qtos itens pra cada
        displ = (int*)malloc(tam*sizeof(int)); // onde comecar no vetor
        for(int proc = 0; proc < tam; proc++){ // para cad processo
            counts[proc] = parte; // base da divisao inteira
            if(proc < resto) // se o No do processo for menor que o resto, adiciona 1 para enviar
                counts[proc]++;
            displ[proc] = cont; // proximo começa onde o ultimo parou
            cont += counts[proc]; // adiciona o ultimo
        }
    }

    if(rank < resto) // se recebeu a mais, o processo adiciona em parte
        parte++;

    x_loc = (double*)malloc(parte*sizeof(double)); // cria os valores locais para cada processo
    y_loc = (double*)malloc(parte*sizeof(double));

    MPI_Scatterv(x, counts, displ, MPI_DOUBLE, x_loc, parte, MPI_DOUBLE, 0, MPI_COMM_WORLD); // 0 envia as partes
    MPI_Scatterv(y, counts, displ, MPI_DOUBLE, y_loc, parte, MPI_DOUBLE, 0, MPI_COMM_WORLD); // de x e y para todos
     
    result_loc = escalar(x_loc, y_loc, parte); // calcula localmente o resultado
    printf("O processo %d enviou %f\n", rank, result_loc);

    MPI_Reduce(&result_loc, &result, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); // reduz, pega o valor de result loc de todos

    if(rank == 0)
        printf("O resultado é: %f\n", result);

    MPI_Finalize();
    return 0;
}
