#include <iostream>

//#ifdef _OPENMP
    #include <omp.h>
//#endif

int main() {
//    #ifdef _OPENMP
        omp_set_num_threads(4);
        int num = omp_get_num_threads();
        std::cout << "__--__ programa paralelo!!! __--__" << std::endl
        << "num threads inicial: " << num << std::endl;

        #pragma omp parallel //cada thread roda todo o bloco
        {
            num = omp_get_num_threads();
            int thread = omp_get_thread_num();
            
            #pragma omp critical //faz as threads nao se atropelarem
            {
                std::cout << "thread de no: " << thread
                << " total threads: " << num << std::endl;
            }
        }
        return 0;
//    #endif
//       std::cout << "__--__ openMP nao definido!!! __--__" << std::endl;
//       return 1;
}