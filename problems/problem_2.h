#ifndef PROBLEM_2_H
#define PROBLEM_2_H

// Tamanho máximo da pilha (capacidade do vetor)
#define CAPACIDADE 5

typedef struct {
        int topo;               // índice do elemento no topo da pilha
        int dados[CAPACIDADE];  // vetor para armazenar os elementos da pilha
    } Pilha;

void problem_2(void);

#endif /* PROBLEM_2_H */