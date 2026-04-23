import random
import sys

def gerar(nome, num_nos, num_arestas):
    lim = (num_nos * (num_nos - 1)) // 2
    if num_arestas > lim:
        print("No de Arestas excede o limite (n*(n-1))/2")
        return

    arestas = set()
    
    while len(arestas) < num_arestas:
        u = random.randint(0, num_nos - 1)
        v = random.randint(0, num_nos - 1)
        if u != v:
            ordenada = (min(u, v), max(u, v))
            arestas.add(ordenada)

    with open(nome, 'w') as f:
        f.write(f"{num_nos}\n")
        
        for u, v in arestas:
            f.write(f"{u} {v} 1\n")

gerar(str(sys.argv[1]),int(sys.argv[2]), int(sys.argv[3]))