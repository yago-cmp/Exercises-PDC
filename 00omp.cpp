#include <iostream>
#include <omp.h>

int main() {
    std::cout << "primeiro programa paralelo!!!" << std::endl;
    #pragma omp parallel num_threads(5) //cada thread roda todo o bloco
    {
        int threads = omp_get_thread_num();
        #pragma omp critical //faz as threads nao se atropelarem
        {
            std::cout << "thread de no: " << threads << std::endl;
        }
    }
    return 0;
}