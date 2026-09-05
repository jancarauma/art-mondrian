#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "../graphics.h"
#include "../utils.h"
#include "problem_3.h"

/*
* PROBLEMA 3 - Escreva o código correspondente a uma FILA.
*/
unsigned char ascii_problem_3[] = {
  0x50, 0x52, 0x4f, 0x42, 0x4c, 0x45, 0x4d, 0x41, 0x20, 0x33, 0x20, 0x2d,
  0x20, 0x45, 0x73, 0x63, 0x72, 0x65, 0x76, 0x61, 0x20, 0x6f, 0x20, 0x63,
  0xc3, 0xb3, 0x64, 0x69, 0x67, 0x6f, 0x20, 0x63, 0x6f, 0x72, 0x72, 0x65,
  0x73, 0x70, 0x6f, 0x6e, 0x64, 0x65, 0x6e, 0x74, 0x65, 0x20, 0x61, 0x20,
  0x75, 0x6d, 0x61, 0x20, 0x46, 0x49, 0x4c, 0x41
};
unsigned int ascii_problem_3_length = 56;

// Inicializa a fila vazia
void inicializarFila3(Fila3 *f) {
    f->inicio = 0;
    f->fim = -1;
    f->quantidade = 0;
}

// Verifica se a fila está cheia
bool fila3Cheia(Fila3 *f) {
    return f->quantidade == CAPACIDADE_3;
}

// Verifica se a fila está vazia
bool fila3Vazia(Fila3 *f) {
    return f->quantidade == 0;
}

// Inserir elemento na fila (enqueue)
bool enqueue3(Fila3 *f, int valor) {
    if (fila3Cheia(f)) {
        printf("Fila cheia! Não é possível inserir o valor %d.\n", valor);
        return false;
    }

    if (fila3Vazia(f)) {
        f->inicio = 0;
        f->fim = 0;
    } else {
        f->fim = (f->fim + 1) % CAPACIDADE_3;
    }

    f->dados[f->fim] = valor;
    f->quantidade++;
    return true;
}

// Remover elemento da fila (dequeue)
bool dequeue3(Fila3 *f, int *valor) {
    if (fila3Vazia(f)) {
        printf("Fila vazia! Não é possível remover elementos.\n");
        return false;
    }

    *valor = f->dados[f->inicio];
    f->inicio = (f->inicio + 1) % CAPACIDADE_3;
    f->quantidade--;

    if (fila3Vazia(f)) {
        f->inicio = 0;
        f->fim = -1;
    }

    return true;
}

// Consultar o elemento da frente da fila
bool frente3(Fila3 *f, int *valor) {
    if (fila3Vazia(f)) {
        printf("Fila vazia! Não há elementos para consultar.\n");
        return false;
    }

    *valor = f->dados[f->inicio];
    return true;
}

// Exibir os elementos da fila
void printFila3(Fila3 *f) {
    if (fila3Vazia(f)) {
        printf("Fila vazia!\n");
        return;
    }

    printf("Elementos da fila (da frente para o fim): ");
    for (int i = 0; i < f->quantidade; i++) {
        printf("%d ", f->dados[(f->inicio + i) % CAPACIDADE_3]);
    }
    printf("\n");
}

void problem_3(void)
{
    draw_problem_screen(3, (const char *) ascii_problem_3);
    //----------------------------------------------------

    textcolor(WHITE); textbackground(BLACK);

    Fila3 f;
    inicializarFila3(&f);
    enqueue3(&f, 1);
    enqueue3(&f, 3);
    enqueue3(&f, 7);

    gotoxy(9, 12);
    printFila3(&f);

    gotoxy(9, 13);
    int valor;
    if (frente3(&f, &valor)) {
        printf("Frente da fila: %d\n", valor);
    }

    gotoxy(9, 14);
    if (dequeue3(&f, &valor)) {
        printf("Desenfileirando elemento: %d\n", valor);
    }

    gotoxy(9, 15);
    printFila3(&f);

    enqueue3(&f, 9);
    enqueue3(&f, 11);
    gotoxy(9, 16);
    printFila3(&f);

    //---------------------------------------------------
    waitEsc();
}