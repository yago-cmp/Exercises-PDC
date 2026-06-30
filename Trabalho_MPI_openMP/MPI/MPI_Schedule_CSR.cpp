#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include <sys/time.h>
#include <mpi.h>

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


double findClusterCoefficient(uint* offsets, uint* valores, uint n, int tam, int proc)
{
	double global_cc = 0.0; // variaves globais
	double partial_cc = 0.0;

    std::vector<uint> neighbors; // vetor para guardar vizinhos do vertice atual
	neighbors.reserve(n); // reserva n inteiros, pode ter ate n vizinhos

    if(n >= 10000){ // SCHEDULE, SE N É GRANDE  ----------------------------------------------------------------------------------------
        int tamChunk = n/(tam*10); // tamanho do chunk de vértices, cada processo computará 10 chunks
        int tagChunk = 0; // tag para envio do No do chunk
        int tagInd = 1; // tag de solicitacao de trabalho
        int nChunks = (n + tamChunk - 1) / tamChunk; // No de chunks total, forçando para cima (pois a divisao inteira descarta qualquer resto)

        if (proc == 0){ // PROCESSO 0 atuando como coordenador do trabalho -----------------------------

            int proxChunk = 0; // indicacao do proximo chunk a ser enviado
            int proxProc = 0; // guardar o rank do processo que solicitou para envio do chunk
            int numProcs = tam-1; // numero de processos ativos (calculando o coeficiente)

            int chunk0 = proxChunk; // define o chunk do proc 0
            proxChunk++; // incrementa
            int vert = chunk0*tamChunk; // primeiro vertice do 0, o chunk dele * tamanho do chunk
            int final = vert + tamChunk; // ultimo vertice do 0

            while (numProcs > 0 || vert < final){ // loop principal do proc 0, se

                if(vert == final || vert % (tamChunk/5) == 0){ // quando proc 0 acabar sua computação OU 5 vezes por chunk, proc 0 procura mensagem de requisicao
                    MPI_Status status;
                    int recebimento = 0; // 0 para nenhuma mensagem recebida, 1 para mensagem recebida


                    MPI_Iprobe(MPI_ANY_SOURCE, tagInd, MPI_COMM_WORLD, &recebimento, &status); // usa o Iprobe para verificar se há algum pedido de Indicação de Chunk, salva em recebimento

                    if(recebimento){ // se existe pedido de trabalho de algum processo
                        proxProc = status.MPI_SOURCE; // extrai o autor da mensagem do status
                        int lixo; // var para fazer o recebimento da requisicao
                        MPI_Recv(&lixo, 1, MPI_INT, proxProc, tagInd, MPI_COMM_WORLD, &status); // tira da fila a requisicao do processo

                        if(proxChunk < nChunks){ // se ainda existem chunks a serem enviados
                            MPI_Send(&proxChunk, 1, MPI_INT, proxProc, tagChunk, MPI_COMM_WORLD); // envia para o proc requisitante o No do proximo chunk a ser computado
                            proxChunk++; // incrementa
                        }
                        else{ // se acabaram os chunks
                            int acabou = -1;
                            MPI_Send(&acabou, 1, MPI_INT, proxProc, tagChunk, MPI_COMM_WORLD); // envia para o proc requisitante -1, indicando que acabou
                            numProcs--;
                        }
                   }
                }

                if(vert < final){ // se ainda existem vertices para o proc 0 computar

                    // PROCESSO 0 CALCULA UM VÉRTICE POR VEZ ---------------------------------------------------
                    neighbors.clear(); // limpa vizinhos
                    uint n_triangles = 0; // limpa No de triangulos
                    for (uint j = offsets[vert]; j < offsets[vert+1]; j++) // itera sobre os vizinhos de vert
                            neighbors.push_back(valores[j]); // e salva eles
                    const uint nei_len = neighbors.size(); // qtd vizinhos de i	

                    if (nei_len >= 2){ // se a qtd de vizinhos for maior que 2, faz o cálculo
                        for (uint j = 0; j < nei_len; ++j) { // para cada vizinho u de i
                            uint u = neighbors[j];
                            for (uint k = j+1; k < nei_len; k ++) { // e outro vizinho v de i
                                uint v = neighbors[k];
                                for(int h = offsets[u]; h < offsets[u+1]; h++) // itera sobre as conexoes de u
                                    if (valores[h] == v){ // se v é uma das conexoes de u 
                                            ++n_triangles; // soma a qtd de triangulos
                                            break;
                                    }	
                            }
                        }
                        // ------------------------------------------------------------------------------------------
                        partial_cc += 2 * n_triangles / double(nei_len * (nei_len - 1)); // usa a qtd de triangulos na vizinhanca de i e calcula o coeficiente de agrupamento parcial
                    }
                    vert++; // soma ao no de vertices calculados
                    if(vert == final && proxChunk < nChunks){ // se acabaram os vertices de 0 E ainda existem chunks disponiveis
                        chunk0 = proxChunk; // define o chunk do proc 0
                        proxChunk++; // incrementa
                        vert = chunk0*tamChunk; // primeiro vertice do 0, o chunk dele * tamanho do chunk
                        final = vert + tamChunk; // ultimo vertice do 0

                    }
                }
            }
        }

        else{ // OUTROS PROCESSOS --------------------------------
            int trabalho = 1; // indica se há trabalho ou não
            int requisicao = 1; // indica a intencao de requisitar trabalho
            int proxChunk = 0; // indicacao do proximo chunk a ser computado

            while(trabalho){ // enquanto existe trabalho a ser feito

                MPI_Status status;

                if(requisicao){ // faz uma requisição
                    MPI_Send(&requisicao, 1, MPI_INT, 0, tagInd, MPI_COMM_WORLD); // envia o pedido
                    MPI_Recv(&proxChunk, 1, MPI_INT, 0, tagChunk, MPI_COMM_WORLD, &status); // recebe o indicador do prox chunk a ser computado
                    if(proxChunk == -1){
                        trabalho = 0;
                        break;
                    }
                }

                int inicio = proxChunk*tamChunk; // o inicio da computação é a indicacao do chunk * o tamanho dele
                int fim = (proxChunk+1)*tamChunk; // o fim é o começo do próximo
                for(int i = inicio; i < fim; i++) { // para cada vertice desse processo

                    neighbors.clear(); // limpa vizinhos
                    uint n_triangles = 0; // limpa No de triangulos

                    for (uint j = offsets[i]; j < offsets[i+1]; j++) // itera sobre os vizinhos de i
                            neighbors.push_back(valores[j]); // e salva eles

                    const uint nei_len = neighbors.size(); // qtd vizinhos de i	
                    if (nei_len < 2) // se a qtd de vizinhos for menor que 2, não configura um triangulo

                        continue; // passa para o proximo vertice
                    for (uint j = 0; j < nei_len; ++j) { // para cada vizinho u de i
                        uint u = neighbors[j];
                        for (uint k = j+1; k < nei_len; k ++) { // e outro vizinho v de i
                            uint v = neighbors[k];
                            for(int h = offsets[u]; h < offsets[u+1]; h++) // itera sobre as conexoes de u
                                if (valores[h] == v){ // se v é uma das conexoes de u 
                                        ++n_triangles; // soma a qtd de triangulos
                                        break;
                                }	
                        }
                    }
                    partial_cc += 2 * n_triangles / double(nei_len * (nei_len - 1)); // usa a qtd de triangulos na vizinhanca de i e calcula o coeficiente de agrupamento parcial
                }
            }

        }   
    }

    else{ // PARA VALORES DE N MENORES QUE 10000, SEM SCHEDULE ------------------------------------------------------------
        int parte = n/tam; // calcula a qtd pra cada processo
        int inicio = parte*proc; // calcula o inicio de cada parte
        int fim = parte*(proc+1); // e tambem o fim
        if(proc == tam-1) // se for o ultimo, vai ate n (evita itens sobrando)
            fim = n;

        for(int i = inicio; i < fim; i++) { // para cada vertice desse processo

            neighbors.clear(); // limpa vizinhos
            uint n_triangles = 0; // limpa No de triangulos

            for (uint j = offsets[i]; j < offsets[i+1]; j++) // itera sobre os vizinhos de i
                    neighbors.push_back(valores[j]); // e salva eles

            const uint nei_len = neighbors.size(); // qtd vizinhos de i	

            if (nei_len < 2) // se a qtd de vizinhos for menor que 2, não configura um triangulo
                continue; // passa para o proximo vertice
            
            for (uint j = 0; j < nei_len; ++j) { // para cada vizinho u de i
                uint u = neighbors[j];
                for (uint k = j+1; k < nei_len; k ++) { // e outro vizinho v de i
                    uint v = neighbors[k];
                    for(int h = offsets[u]; h < offsets[u+1]; h++) // itera sobre as conexoes de u
                        if (valores[h] == v){ // se v é uma das conexoes de u 
                                ++n_triangles; // soma a qtd de triangulos
                                break;
                        }	
                }
            }
            partial_cc += 2 * n_triangles / double(nei_len * (nei_len - 1)); // usa a qtd de triangulos na vizinhanca de i e calcula o coeficiente de agrupamento parcial
        }
    }

    // COM OU SEM SCHEDULE: reduz os coeficientes parciais para o global
    MPI_Reduce(&partial_cc, &global_cc, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD); // X -> 0 (+), soma os valores parciais de todos os processos e guarda em global_cc no proc 0

	if(proc == 0)
		return global_cc / n; // proc 0 retorna a média dos coeficientes do grafo, esse é o valor global real
	else
		return 0;
}


int main(int argc, char* argv[])
{
    // CSR -------------------------
	uint* offsets; // tamanho n+1
    uint* valores; // tamanho 2m
	uint n; 
    // ------------------------------

	int proc;
	int tam;

    std::ifstream input;
    std::ofstream output;

	MPI_Init(&argc, &argv); // limpa parametros e inicia MPI
	MPI_Comm_rank(MPI_COMM_WORLD, &proc); // salva o No do processo
	MPI_Comm_size(MPI_COMM_WORLD, &tam);  // salva a qtd de processos

    MPI_Status status;

	if(proc == 0){
		input.open(argv[1]);  // entrada -> arquivo "100_500.txt" ou similar
		output.open(argv[2]); // saida -> resultado do coeficiente para conferir "resultado.txt"
		lerArq(input, &n, &offsets, &valores); // extrai n, os offsets e os valores do arquivo
  		gettimeofday(&tstart, NULL);
    }
// Medição do Tempo-------------------------------------
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD); // 0 -> X, valor de n

	if(proc != 0)
        offsets = (uint*)malloc((n+1)*sizeof(uint)); // cria offset com >> n+1 << espacos nos outros processos
    
	MPI_Bcast(offsets, n+1, MPI_UNSIGNED, 0, MPI_COMM_WORLD); // 0 -> X, vetor de offsets

	if(proc != 0)
		valores = (uint*)malloc(offsets[n]*sizeof(uint)); // cria o vetor de destinos das arestas com >> 2m << espaços 

	MPI_Bcast(valores, offsets[n], MPI_UNSIGNED, 0, MPI_COMM_WORLD); // 0 -> X, vetor de valores
	
	float clustering_coefficient = findClusterCoefficient(offsets, valores, n, tam, proc); // Função Principal -------------------
// ------------------------------------------------------
	if(proc==0){
		gettimeofday(&tend, NULL);

		long tempo = (tend.tv_sec * 1000000 + tend.tv_usec)-(tstart.tv_sec * 1000000 + tstart.tv_usec);
		output << clustering_coefficient << std::endl; // resultado na file
		std::cout << tempo; // tempo no terminal
	}

	MPI_Finalize();
	free(offsets);
	free(valores);
	return 0;
}
