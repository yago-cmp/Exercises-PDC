TRABALHO OPENMP & MPI - Yago Campello
Prog. Paralela e Distribuída - Prof. Cristiana

Escolhi o algoritmo que determina o coeficiente de agrupamento de grafos não ordenados.
Embora a paralelização dos algoritmos tenha sido tranquila, aprendi bastante: além da
paralelização tive que fazer um código em python que gera os arquivos de entrada com
os grafos de teste 'gerador.py', e o trabalho foi um incentivo para pesquisar e aprender
sobre scripts Bash, para medir os tempos 'tempos.sh'. A coluna "Result" serve apenas
para verificar os resultados.

Sequencial - Tempos --__--__--__--__--__--__

Nós	Média/10x	Result
100	88		0.10125
200	225		0.0418435
500	888		0.0200958
1000	2998		0.00936057
5000	70395		0.0018823
10000	264679		0.000982875

OpenMP - Tempos  --__--__--__--__--__--__--__

-> Paralelização 1: Reduction
	Nós	Média/10x	Result		Speedup
	100	685		0.10125		0.13x -	
	200	794		0.0418435	0.28x -
	500	1075		0.0200958	0.82x -
	1000	1821		0.00936057	1.64x +
	5000	19674		0.0018823	3.57x +
	10000	70608		0.000982875	3.75x +

	Obs: Speedup maior com dados maiores (Overhead)

-> Paralelização 2: Critical
	Nós	Média/10x	Result		Speedup
	100	699		0.10125		0.12x -	
	200	870		0.0418435	0.25x -
	500	1047		0.0200958	0.84x -
	1000	1697		0.00936057	1.76x +
	5000	19704		0.0018823	3.57x +
	10000	72384		0.000982875	3.65x +

	Obs: Abordagem 1 melhor com dados maiores, mas estão razoavelmente empatadas.

MPI - Tempos  --__--__--__--__--__--__--__--__

	Nós	Média/10x	Result		Speedup
	100	685		0.10125		0.13x -	
	200	794		0.0418435	0.28x -
	500	1075		0.0200958	0.82x -
	1000	1821		0.00936057	1.64x +
	5000	19674		0.0018823	3.57x +
	10000	70608		0.000982875	3.75x +
