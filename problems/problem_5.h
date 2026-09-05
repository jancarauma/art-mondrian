#ifndef PROBLEM_5_H
#define PROBLEM_5_H

#include <stdlib.h>
#include <stdbool.h>

// Tamanho máximo da pilha (capacidade do vetor)
#define CAPACIDADE 100

typedef struct {
        int topo;                // índice do elemento no topo da pilha
        char dados[CAPACIDADE];  // vetor para armazenar os elementos da pilha
    } PilhaChar5;

void problem_5(void);
void inicializarPilhaChar5(PilhaChar5 *p);
bool pilhaCharCheia5(PilhaChar5 *p);
bool pilhaCharVazia5(PilhaChar5 *p);
bool pushPilhaChar5(PilhaChar5 *p, char valor);
bool popPilhaChar5(PilhaChar5 *p, char *valor);
bool simbolosBalanceados(const char *texto);

#endif /* PROBLEM_5_H */