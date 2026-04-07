#include <stdlib.h>
#include <stdio.h>

#define N 10000

int main ( int argc, char *argv[] ){
   int i;
   double x[N];
   double maxval = 0.0; int maxloc = 0;
   
   for(i=0; i < N;i++){
     x[i] = rand() % 999;
   }

    
        double max_local, maxloc_local = 0.0;
        for (i=0; i < N; i++){
                if (x[i] > max_local) {
                    max_local = x[i];
                    maxloc_local = i;
                }
        }
        
   printf("Maximum value: %f - Location: %d \n", maxval, maxloc);
  return 0;
}


   
