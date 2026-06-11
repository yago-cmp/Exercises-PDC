#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <sys/time.h>
#include <mpi.h>

struct timeval tstart, tend;

typedef unsigned int uint;

template <class _STREAM> void TurnExceptionsOn(_STREAM& stream)
{
        stream.exceptions(std::ios_base::badbit |
                          std::ios_base::failbit |
                          std::ios_base::eofbit);
}

uint** makeSquareMatrix(uint n)
{
	if (n == 0) {
		return NULL;
	}

	uint** B = NULL;
	uint* store = NULL;

	// Allocate matrix
	store = new uint[n*n];
	// Clear it...
	memset(store, 0x00, n * n * sizeof(uint));
	// Setup array B
	B = (uint**) new uint*[n];
	for (int row = 0; row < n; ++row) 
		B[row] = &store[row * n];
	return B;

}

uint** readFile(std::istream& input, uint& n)
{
	uint** M = NULL;
	uint u, v, k;

	n = 0;
	TurnExceptionsOn(input);

	input >> n;
	input.exceptions(std::ios_base::goodbit);

	M = makeSquareMatrix(n);
	if (M == NULL) {
		return NULL;
	}

	while (input >> u >> v >> k) {
		M[u][v] = k;
		M[v][u] = k;
	}
	return M;
}


double findClusterCoefficient(uint** M, uint n, int tam, int rank)
{
	int parte = n/tam;
	double global_cc = 0.0;
	double partial_cc = 0.0;
	std::vector<uint> neighbors;

	neighbors.reserve(n);

	int inicio = parte*rank;
	int fim = parte*(rank+1);
	if(rank == tam-1) // se for o ultimo, vai ate n EVITA ITENS SOBRANDO
		fim = n;

	for(int i = inicio; i < fim; i++) {
		// Set things up for this iteration
		neighbors.clear();
		uint n_triangles = 0;
		// Finds i's neighborhood
		for (uint j = 0; j < n; ++j) {
			if ( i != j && M[i][j]) {
				neighbors.push_back(j);
			}
		}
		// Find the number of clusters in i's neighborhood
		if (neighbors.size() < 2) {
			// At least two neighbors are need in order to
			// be able to calc. i's Clustering Coefficient
			continue;
		}
		const uint nei_len = neighbors.size();
		for (uint j = 0; j < nei_len; ++j) {
			uint u = neighbors[j];
			for (uint k = j+1; k < nei_len; k ++) {
				uint v = neighbors[k];
				if (M[u][v]) {
					++n_triangles;
				}
			}
		}
		partial_cc += 2 * n_triangles / double(nei_len * (nei_len - 1));
	}
	
	MPI_Reduce(&partial_cc, &global_cc, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);

	if(rank == 0)
		return global_cc / n;
	else
		return 0;
}


int main(int argc, char* argv[])
{
	uint** matrix = NULL;
	uint n = 4;

	int i, j, k;

	int tagN = 0;
	
	int rank;
	int tam;

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &tam); 

	MPI_Status status;

    std::ifstream input;
    std::ofstream output;

	TurnExceptionsOn(input);
	TurnExceptionsOn(output);

	if(rank == 0){
		if (argc != 3) {
					std::cerr << "ERROR: Wrong number of arguments.\n" <<
								"Usage: ./clustering_coefficient INPUT OUTPUT\n" 
					<< std::endl;
					exit(EXIT_FAILURE);
		}

		input.open(argv[1]);  
		output.open(argv[2]);

		matrix = readFile(input, n);
		if (matrix == NULL) {
			std::cerr << "Error creating square matrix" << std::endl;
			return 1;
		}
	}
	
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); // envia e recebe N

	if(rank != 0)
		matrix = makeSquareMatrix(n); // inicia a matriz

	MPI_Bcast(matrix[0], n*n, MPI_UNSIGNED, 0, MPI_COMM_WORLD); // matriz é enviada
	
	if(rank==0)
  		gettimeofday(&tstart, NULL);
//--------- Mede o tempo da função abaixo ---------------
	float clustering_coefficient = findClusterCoefficient(matrix, n, tam, rank);
//-------------------------------------------------------
	if(rank==0){
		gettimeofday(&tend, NULL);
		long tempo = (tend.tv_sec * 1000000 + tend.tv_usec)-(tstart.tv_sec * 1000000 + tstart.tv_usec);
		output << clustering_coefficient << std::endl; // resultado na file
		std::cout << tempo; 
	}

	MPI_Finalize();
	return 0;
}
