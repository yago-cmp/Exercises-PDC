#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <sys/time.h>

struct timeval tstart, tend;

typedef unsigned int uint;

void lerArq(std::istream& input, uint* n_ref, uint** offsets_ref, uint** valores_ref) // FUNCAO REESCRITA PARA CSR -----------------------------------
{
	uint u, v, k; // iteradores para o arquivo
	uint n = 0; // qtd de vertices
    uint m = 0; // qtd de arestas

	input >> n; // guarda a qtd de vertices

    uint* graus = (uint*)calloc(n, sizeof(uint)); // grau de cada vértice

    while (input >> u >> v >> k) { // conta o grau de cada vertice e a qtd de arestas
        graus[u]++;
        graus[v]++;
        m++;
	}

    uint* offsets = (uint*)malloc((n+1)*sizeof(uint)); // cria o vetor de offsets com n+1 espaços (guarda m em offsets[n])
    uint* valores = (uint*)malloc((2*m)*sizeof(uint)); // cria o vetor de destinos das arestas com 2m espaços (o grafo é bidirecional)

    // o offset de cada um dos vértices é o anterior + o grau do vértice anterior
    offsets[0] = 0;
    for(int i =0; i < n; i++)
        offsets[i+1] = offsets[i] + graus[i]; // na ultima iteracao, offsets[n] guarda 2m

    input.clear();                     
    input.seekg(0, std::ios::beg); // volta para o início do arquivo
    input >> n; // pula o n

    //o preenchimento é do começo ao fim, é preciso um vetor para guardar a posicao de cada vertice
    uint* posicoes = (uint*)malloc(n*sizeof(uint));
    for(int i = 0; i < n; i++)
        posicoes[i] = offsets[i]; // começa no offset inicial de cada um

    while (input >> u >> v >> k) { // le novamente o arquivo todo
        valores[posicoes[u]] = v; // salva nos valores de u o vertice v
        valores[posicoes[v]] = u; // salva nos valores de v o vertice u
        posicoes[u]++; 
        posicoes[v]++; // anda um para cada aresta encontrada 
	}    

    free(graus);
    free(posicoes); // desaloca os auxiliares
    
    *offsets_ref = offsets; // retorna como referencia os vetores
    *valores_ref = valores;
    *n_ref = n;
}

double findClusterCoefficient(uint* offsets, uint* valores, uint n)
{
	double partial_cc = 0.0;
	std::vector<uint> neighbors;

	neighbors.reserve(n);

	// For each node
	for (uint i = 0; i < n; ++i) {

		neighbors.clear();
		uint n_triangles = 0;

		for (uint j = offsets[i]; j < offsets[i+1]; j++) // itera sobre os vizinhos de i
					neighbors.push_back(valores[j]); // e salva eles

		if (neighbors.size() < 2) 
			continue;
		
		const uint nei_len = neighbors.size();
		for (uint j = 0; j < nei_len; ++j) {
			uint u = neighbors[j];
			for (uint k = j; k < nei_len; k ++) {
				uint v = neighbors[k];
				for(int h = offsets[u]; h < offsets[u+1]; h++) // itera sobre as conexoes de u
                    if (valores[h] == v){ // se v é uma das conexoes de u 
                        ++n_triangles; // soma a qtd de triangulos
                        break;
                    }	
			}
		}
		// Remember: The graph is undireted, so M[u][v] is treated as
		// being equivalent to M[v][u]. But in the loop above we only
		// accounted for one of those edges (namely, M[u][v]). Thus
		// n_triangles should be doubled.
		// Also notice that the maximum number of clusters between
		// i's neighbors is nei_len * (nei_len - 1)
		partial_cc += 2 * n_triangles / double(nei_len * (nei_len - 1));
	}

	return partial_cc / n;
}

int main(int argc, char* argv[])
{
    // CSR -------------------------
	uint* offsets; // tamanho n+1
    uint* valores; // tamanho 2m
	uint n; 
    // ------------------------------

    std::ifstream input;
    std::ofstream output;

	input.open(argv[1]);  
	output.open(argv[2]);

	lerArq(input, &n, &offsets, &valores); // extrai n, os offsets e os valores do arquivo

  	gettimeofday(&tstart, NULL);
//--------- Mede o tempo da função abaixo ---------------
	float clustering_coefficient = findClusterCoefficient(offsets, valores, n);
//-------------------------------------------------------
	gettimeofday(&tend, NULL);

	long tempo = (tend.tv_sec * 1000000 + tend.tv_usec)-(tstart.tv_sec * 1000000 + tstart.tv_usec);

	output << clustering_coefficient << std::endl; // resultado na file
	std::cout << tempo; 

    free(offsets);
	free(valores);
	return 0;
}
