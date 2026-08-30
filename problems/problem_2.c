#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "../graphics.h"
#include "../utils.h"
#include "problem_2.h"

/*
* PROBLEMA 2 - Escreva o código correspondente a uma PILHA.
*/
unsigned char ascii_problem_2[] = {
  0x50, 0x52, 0x4f, 0x42, 0x4c, 0x45, 0x4d, 0x41, 0x20, 0x32, 0x20, 0x2d, 
  0x20, 0x45, 0x73, 0x63, 0x72, 0x65, 0x76, 0x61, 0x20, 0x6f, 0x20, 0x63, 
  0xc3, 0xb3, 0x64, 0x69, 0x67, 0x6f, 0x20, 0x63, 0x6f, 0x72, 0x72, 0x65, 
  0x73, 0x70, 0x6f, 0x6e, 0x64, 0x65, 0x6e, 0x74, 0x65, 0x20, 0x61, 0x20, 
  0x75, 0x6d, 0x61, 0x20, 0x50, 0x49, 0x4c, 0x48, 0x41, 0x2e
};
unsigned int ascii_problem_2_length = 58;

// Inicializa o topo como -1, indicando que a pilha está vazia
void inicializar(Pilha *p) {
    p->topo = -1; 
}

// Verifica se a pilha está cheia
bool estaCheia(Pilha *p) {
    return p->topo == CAPACIDADE - 1;
}

// Verifica se a pilha está vazia
bool estaVazia(Pilha *p) {
    return p->topo == -1;
}

// Inserir elemento na pilha (push)
bool push(Pilha *p, int valor) {
    if (estaCheia(p)) {
        printf("Estouro de pilha! Não é possível inserir o valor %d.\n", valor);
        return false; // Pilha cheia
    }
    p->topo++;
    p->dados[p->topo] = valor;
    return true;
}

// Desempilhar elemento da pilha (pop)
bool pop(Pilha *p, int *valor) {
    if (estaVazia(p)) {
        printf("Pilha vazia! Não é possível remover elementos.\n");
        return false; // Pilha vazia
    }
    *valor = p->dados[p->topo];
    p->topo--;
    return true;
}

// Consultar o elemento no topo da pilha (peek)
bool peek(Pilha *p, int *valor) {
    if (estaVazia(p)) {
        printf("Pilha vazia! Não há elementos para consultar.\n");
        return false; // Pilha vazia
    }
    *valor = p->dados[p->topo];
    return true;
}

// Exibir os elementos da pilha
void print(Pilha *p) {
    if (estaVazia(p)) {
        printf("Pilha vazia!\n");
        return;
    }
    printf("Elementos da pilha (do topo para a base): ");
    for (int i = p->topo; i >= 0; i--) {
        printf("%d ", p->dados[i]);
    }
    printf("\n");
}

void problem_2(void)
{
    draw_problem_screen(2,(const char *) ascii_problem_2);
    //----------------------------------------------------

    textcolor(WHITE); textbackground(BLACK);

    Pilha p;
    inicializar(&p);
    push(&p, 1);
    push(&p, 3);
    push(&p, 7);
    gotoxy(9, 12);
    print(&p);
    gotoxy(9, 13);
    printf("Topo da pilha: %d\n", p.dados[p.topo]);
    int valor;
    gotoxy(9, 14);
    if (pop(&p, &valor)) {
        printf("Desempilhando elemento: %d\n", valor);
    }
    gotoxy(9, 15);
    print(&p);
    push(&p, 9);
    gotoxy(9, 16);
    print(&p);

    //#define VECTOR_SIZE 5341
    //int i, collisions = 0, value;
    //int vector[VECTOR_SIZE] = {0};

    //srand((unsigned int) time(NULL));

    //for (i = 0 ; i < 2000; i++)
   // {
    //    value = (rand() % 1000000) + 1;
    //    if (vector[value % VECTOR_SIZE] != 0)
    //        collisions++;
    //    vector[value % VECTOR_SIZE] = value;
    //}    
    //gotoxy(9, 12);
    //textcolor(WHITE); textbackground(BLACK);
    //printf("Número total de colisões: \e[31;40m%d\e[37;40m.", collisions);
    //---------------------------------------------------
    waitEsc();
}