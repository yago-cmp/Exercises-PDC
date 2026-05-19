#include <math.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    int tam;
    int rank;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &tam); 

    int n, p, i, fim;
    double mypi, h, sum, sum_loc, x;

    if(rank == 0)
        n = atoi(argv[1]);
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);    

    p = n/tam;
    h = 1.0 / (double) n;

    sum_loc = 0.0;

    if (rank == (tam-1))
        fim = n;
    else
        fim = ((rank+1)*(p))+1;

    for (i = (rank*p)+1; i <= fim; i ++) {
        x = h * ((double) i - 0.5);
        sum_loc += 4.0 / (1.0 + x*x);
    }

    MPI_Reduce(&sum_loc, &sum, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); 

    if (rank == 0){
        mypi = h * sum;
        printf("pi is approximately %.16f\n", mypi);
    }
    return 0;
}
