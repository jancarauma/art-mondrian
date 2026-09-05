#ifndef PROBLEM_4_H
#define PROBLEM_4_H

#include <stdlib.h>
#include <stdbool.h>

// Tamanho máximo da pilha (capacidade do vetor)
#define CAPACIDADE 100

typedef struct {
        int topo;                // índice do elemento no topo da pilha
        char dados[CAPACIDADE];  // vetor para armazenar os elementos da pilha
    } PilhaChar;

void problem_4(void);
void inicializarPilhaChar(PilhaChar *p);
bool pilhaCharCheia(PilhaChar *p);
bool pilhaCharVazia(PilhaChar *p);
bool pushPilhaChar(PilhaChar *p, char valor);
bool popPilhaChar(PilhaChar *p, char *valor);

#endif /* PROBLEM_4_H */