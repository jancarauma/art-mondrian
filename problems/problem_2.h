#ifndef PROBLEM_2_H
#define PROBLEM_2_H

#include <stdlib.h>
#include <stdbool.h>

// Tamanho máximo da pilha (capacidade do vetor)
#define CAPACIDADE 100

typedef struct {
        int topo;               // índice do elemento no topo da pilha
        int dados[CAPACIDADE];  // vetor para armazenar os elementos da pilha
    } Pilha;

void problem_2(void);
void inicializar(Pilha *p);
bool estaCheia(Pilha *p);
bool estaVazia(Pilha *p);
bool push(Pilha *p, int valor);
bool pop(Pilha *p, int *valor);

#endif /* PROBLEM_2_H */