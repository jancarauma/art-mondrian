#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "../graphics.h"
#include "../utils.h"
#include "problem_4.h"

/*
* PROBLEMA 4 - Utilizando uma Pilha, verificar se uma palavra é um palíndromo.
* Um palíndromo é uma palavra, frase ou número que fica igual quando lido de 
* trás para frente.
*/
unsigned char ascii_problem_4[] = {
  0x50, 0x52, 0x4f, 0x42, 0x4c, 0x45, 0x4d, 0x41, 0x20, 0x34, 0x20, 0x2d, 
  0x20, 0x55, 0x74, 0x69, 0x6c, 0x69, 0x7a, 0x61, 0x6e, 0x64, 0x6f, 0x20, 
  0x75, 0x6d, 0x61, 0x20, 0x50, 0x69, 0x6c, 0x68, 0x61, 0x2c, 0x20, 0x76, 
  0x65, 0x72, 0x69, 0x66, 0x69, 0x63, 0x61, 0x72, 0x20, 0x73, 0x65, 0x20, 
  0x75, 0x6d, 0x61, 0x20, 0x70, 0x61, 0x6c, 0x61, 0x76, 0x72, 0x61, 0x20, 
  0xc3, 0xa9, 0x0a, 0x75, 0x6d, 0x20, 0x70, 0x61, 0x6c, 0xc3, 0xad, 0x6e, 
  0x64, 0x72, 0x6f, 0x6d, 0x6f, 0x2e, 0x20, 0x55, 0x6d, 0x20, 0x70, 0x61, 
  0x6c, 0xc3, 0xad, 0x6e, 0x64, 0x72, 0x6f, 0x6d, 0x6f, 0x20, 0xc3, 0xa9, 
  0x20, 0x75, 0x6d, 0x61, 0x20, 0x70, 0x61, 0x6c, 0x61, 0x76, 0x72, 0x61, 
  0x2c, 0x20, 0x66, 0x72, 0x61, 0x73, 0x65, 0x20, 0x6f, 0x75, 0x20, 0x6e, 
  0xc3, 0xba, 0x6d, 0x65, 0x72, 0x6f, 0x20, 0x0a, 0x71, 0x75, 0x65, 0x20, 
  0x66, 0x69, 0x63, 0x61, 0x20, 0x69, 0x67, 0x75, 0x61, 0x6c, 0x20, 0x71, 
  0x75, 0x61, 0x6e, 0x64, 0x6f, 0x20, 0x6c, 0x69, 0x64, 0x6f, 0x20, 0x64, 
  0x65, 0x20, 0x20, 0x74, 0x72, 0xc3, 0xa1, 0x73, 0x20, 0x70, 0x61, 0x72, 
  0x61, 0x20, 0x66, 0x72, 0x65, 0x6e, 0x74, 0x65, 0x2e
};
unsigned int ascii_problem_4_length = 177;

// Inicializa o topo como -1, indicando que a pilha está vazia
void inicializarPilhaChar(PilhaChar *p) {
    p->topo = -1; 
}

// Verifica se a pilha está cheia
bool pilhaCharCheia(PilhaChar *p) {
    return p->topo == CAPACIDADE - 1;
}

// Verifica se a pilha está vazia
bool pilhaCharVazia(PilhaChar *p) {
    return p->topo == -1;
}

// Inserir elemento na pilha (push)
bool pushPilhaChar(PilhaChar *p, char valor) {
    if (pilhaCharCheia(p)) {
        printf("Estouro de pilha! Não é possível inserir o valor %d.\n", valor);
        return false; // Pilha cheia
    }
    p->topo++;
    p->dados[p->topo] = valor;
    return true;
}

// Desempilhar elemento da pilha (pop)
bool popPilhaChar(PilhaChar *p, char *valor) {
    if (pilhaCharVazia(p)) {
        printf("Pilha vazia! Não é possível remover elementos.\n");
        return false; // Pilha vazia
    }
    *valor = p->dados[p->topo];
    p->topo--;
    return true;
}

static bool igualSemCaso(const char *a, const char *b)
{
    while (*a != '\0' && *b != '\0') {
        unsigned char ca = (unsigned char) *a;
        unsigned char cb = (unsigned char) *b;

        if (tolower(ca) != tolower(cb)) {
            return false;
        }

        a++;
        b++;
    }

    return *a == '\0' && *b == '\0';
}

bool isPalindrome(char word[])
{
    int len = (int)strlen(word);
    
    if (len <= 1) {
        return true;
    }

    PilhaChar p;
    inicializarPilhaChar(&p);

    int metade = len / 2;
    for (int i = 0; i < metade; i++) {
        if (!pushPilhaChar(&p, word[i])) {
            return false;
        }
    }

    int inicioComparacao = (len % 2 == 0) ? metade : metade + 1;

    for (int i = inicioComparacao; i < len; i++) {
        char valor;
        if (!popPilhaChar(&p, &valor)) {
            return false;
        }

        if (word[i] != valor) {
            return false;
        }
    }

    return true;
}

void problem_4(void)
{
    draw_problem_screen(2,(const char *) ascii_problem_4);
    //----------------------------------------------------

    textcolor(WHITE); textbackground(BLACK);

    char textInput[CAPACIDADE];

    gotoxy(9, 12);
    printf("É ou não é um Palíndromo, meu bom?\n");
    printf("Digite 'sair' para encerrar.\n\n");

    while (1) {
        gotoxy(9, 14);
        printf("Mande bala, qual a palavra?: ");
        if (fgets(textInput, sizeof(textInput), stdin) == NULL) break;

        textInput[strcspn(textInput, "\r\n")] = '\0';

        if (igualSemCaso(textInput, "sair")) {
            gotoxy(9, 15);
            printf("Encerrando o programa...\n");
            break;
        }

        if (strlen(textInput) == 0) continue;

        gotoxy(9, 16);
        if (isPalindrome(textInput)) {            
            printf("-> \"%s\" eh um PALINDROMO!\n\n", textInput);
        } else {
            printf("-> \"%s\" NAO eh um palindromo.\n\n", textInput);
        }
    }

    //---------------------------------------------------
    waitEsc();
}