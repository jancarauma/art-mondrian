#ifndef PROBLEM_3_H
#define PROBLEM_3_H

#include <stdlib.h>
#include <stdbool.h>

#define CAPACIDADE_3 100

typedef struct {
    int inicio;                 // índice do primeiro elemento da fila
    int fim;                    // índice do último elemento da fila
    int dados[CAPACIDADE_3];   // vetor para armazenar os elementos da fila
    int quantidade;             // quantidade atual de elementos
} Fila3;

void problem_3(void);
void inicializarFila3(Fila3 *f);
bool fila3Cheia(Fila3 *f);
bool fila3Vazia(Fila3 *f);
bool enqueue3(Fila3 *f, int valor);
bool dequeue3(Fila3 *f, int *valor);
bool frente3(Fila3 *f, int *valor);
void printFila3(Fila3 *f);

#endif /* PROBLEM_3_H */