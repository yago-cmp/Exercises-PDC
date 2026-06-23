import random
import sys


# Gerador de Grafos em .txt em formato:
# 3 (No de nós)
# 1 2 1 (1 e 2 se conectam)
# 0 1 1 (0 e 1 se conectam)
# 0 2 1 (0 e 2 se conectam)

# Uso:
# ./programa.out <nome do arquivo de saida> <numero de nós> <numero de arestas> <modo>
# com modo sendo 0 para grafos 100% aleatórios e 1 para grafos desbalanceados


def gerar(nome, num_nos, num_arestas, modo):
    lim = (num_nos * (num_nos - 1)) // 2
    if num_arestas > lim:
        print("No de Arestas excede o limite (n*(n-1))/2")
        return

    arestas = set()
    if modo == 0: # modo 0: completamente aleatorio ------------------------------
        while len(arestas) < num_arestas:
            u = random.randint(0, num_nos - 1) # u e v puramente aleatorios
            v = random.randint(0, num_nos - 1)
            if u != v:
                ordenada = (min(u, v), max(u, v)) # adiciona nas arestas se forem diferentes
                arestas.add(ordenada)

    else: # modo 1: grafo desbalanceado ----------------------------------------------
        lista_sorteio = list(range(num_nos)) # add todos os vertices uma vez, 0 a num_nos-1
        while len(arestas) < num_arestas:
            u = random.choice(lista_sorteio) # escolhe dois aleatoriamente
            v = random.choice(lista_sorteio)
            if u != v:
                ordenada = (min(u, v), max(u, v)) # adiciona nas arestas se forem diferentes
                arestas.add(ordenada)
                lista_sorteio.append(u) # adiciona u e v de novo no sorteio
                lista_sorteio.append(v) # mais chances de cair em u e v novamente

    with open(nome, 'w') as f: # escreve no arquivo no formato
        f.write(f"{num_nos}\n")
        
        for u, v in arestas:
            f.write(f"{u} {v} 1\n")

gerar(str(sys.argv[1]),int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]))