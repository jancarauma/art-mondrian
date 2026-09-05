#ifndef PROBLEM_6_H
#define PROBLEM_6_H

#include <stdlib.h>
#include <stdbool.h>

// Estrutura do nó da fila
typedef struct No {
    int valor;
    struct No *prox;
} No;

// Estrutura da Fila
typedef struct {
    No *inicio;
    No *fim;
    int quantidade;
} Fila;

void inicializarFila(Fila *f);
bool filaVazia(Fila *f);
bool enqueue(Fila *f, int valor);
bool dequeue(Fila *f, int *valor);
void destruirFila(Fila *f);

int batataquente(int n, int k);
void problem_6(void);

#endif /* PROBLEM_6_H */