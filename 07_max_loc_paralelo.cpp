#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define N 10000
struct timeval tstart, tend;

int main (int argc, char *argv[]){
  srand(time(NULL));
  int i;
  double x[N];
  double max_global, maxval = 0.0;
  int maxloc_global, maxloc = 0;
  int qtd = 0;

  for(i=0; i < N;i++)
    x[i] = rand() % 999;
    
  gettimeofday(&tstart, NULL);
  #pragma omp parallel num_threads(4) private(i, maxval, maxloc) shared(max_global, maxloc_global, qtd)
  {
      #pragma omp for // paraleliza a busca
        for (i=0; i < N; i++) {
          if (x[i] >= maxval) {
            if (x[i] > maxval)
              qtd = 0;
            maxval = x[i];
            maxloc = i;
            qtd++;

          }
        }
      #pragma omp critical // evita race condition, compara o max local de cada thread
      {    
          if(maxval > max_global){
            max_global = maxval;
            maxloc_global = maxloc;
          }
      }      
  }
  gettimeofday(&tend, NULL);

  long tempo = (tend.tv_sec * 1000000 + tend.tv_usec)-(tstart.tv_sec * 1000000 + tstart.tv_usec);

  printf("Maximum value: %f  - Loc: %d - QTD: %d - Tempo: %ld us\n", max_global, maxloc_global, qtd, tempo);
  return 0;
}


   
