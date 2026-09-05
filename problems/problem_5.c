#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "../graphics.h"
#include "../utils.h"
#include "problem_5.h"


/*
* PROBLEMA 5 - Dada uma sequência de texto contendo caracteres 
* matemáticos e símbolos de agrupamento—como parênteses (), 
* colchetes [] e chaves {}, determinar se esses símbolos foram
* abertos e fechados na ordem correta.
*/
unsigned char ascii_problema_5[] = {
  0x50, 0x52, 0x4f, 0x42, 0x4c, 0x45, 0x4d, 0x41, 0x20, 0x35, 0x20, 0x2d, 
  0x20, 0x44, 0x61, 0x64, 0x61, 0x20, 0x75, 0x6d, 0x61, 0x20, 0x73, 0x65, 
  0x71, 0x75, 0xc3, 0xaa, 0x6e, 0x63, 0x69, 0x61, 0x20, 0x64, 0x65, 0x20, 
  0x74, 0x65, 0x78, 0x74, 0x6f, 0x20, 0x63, 0x6f, 0x6e, 0x74, 0x65, 0x6e, 
  0x64, 0x6f, 0x20, 0x63, 0x61, 0x72, 0x61, 0x63, 0x74, 0x65, 0x72, 0x65, 
  0x73, 0x20, 0x0a, 0x6d, 0x61, 0x74, 0x65, 0x6d, 0xc3, 0xa1, 0x74, 0x69, 
  0x63, 0x6f, 0x73, 0x20, 0x65, 0x20, 0x73, 0xc3, 0xad, 0x6d, 0x62, 0x6f, 
  0x6c, 0x6f, 0x73, 0x20, 0x64, 0x65, 0x20, 0x61, 0x67, 0x72, 0x75, 0x70, 
  0x61, 0x6d, 0x65, 0x6e, 0x74, 0x6f, 0xe2, 0x80, 0x94, 0x63, 0x6f, 0x6d, 
  0x6f, 0x20, 0x70, 0x61, 0x72, 0xc3, 0xaa, 0x6e, 0x74, 0x65, 0x73, 0x65, 
  0x73, 0x20, 0x28, 0x29, 0x2c, 0x20, 0x0a, 0x63, 0x6f, 0x6c, 0x63, 0x68, 
  0x65, 0x74, 0x65, 0x73, 0x20, 0x5b, 0x5d, 0x20, 0x65, 0x20, 0x63, 0x68, 
  0x61, 0x76, 0x65, 0x73, 0x20, 0x7b, 0x7d, 0x2c, 0x20, 0x64, 0x65, 0x74, 
  0x65, 0x72, 0x6d, 0x69, 0x6e, 0x61, 0x72, 0x20, 0x73, 0x65, 0x20, 0x65, 
  0x73, 0x73, 0x65, 0x73, 0x20, 0x73, 0xc3, 0xad, 0x6d, 0x62, 0x6f, 0x6c, 
  0x6f, 0x73, 0x20, 0x66, 0x6f, 0x72, 0x61, 0x6d, 0x0a, 0x61, 0x62, 0x65, 
  0x72, 0x74, 0x6f, 0x73, 0x20, 0x65, 0x20, 0x66, 0x65, 0x63, 0x68, 0x61, 
  0x64, 0x6f, 0x73, 0x20, 0x6e, 0x61, 0x20, 0x6f, 0x72, 0x64, 0x65, 0x6d, 
  0x20, 0x63, 0x6f, 0x72, 0x72, 0x65, 0x74, 0x61, 0x2e
};
unsigned int ascii_problema_5_length = 225;

// Inicializa o topo como -1, indicando que a pilha está vazia
void inicializarPilhaChar5(PilhaChar5 *p) {
    p->topo = -1; 
}

// Verifica se a pilha está cheia
bool pilhaCharCheia5(PilhaChar5 *p) {
    return p->topo == CAPACIDADE - 1;
}

// Verifica se a pilha está vazia
bool pilhaCharVazia5(PilhaChar5 *p) {
    return p->topo == -1;
}

// Inserir elemento na pilha (push)
bool pushPilhaChar5(PilhaChar5 *p, char valor) {
    if (pilhaCharCheia5(p)) {
        printf("Estouro de pilha! Não é possível inserir o valor %d.\n", valor);
        return false; // Pilha cheia
    }
    p->topo++;
    p->dados[p->topo] = valor;
    return true;
}

// Desempilhar elemento da pilha (pop)
bool popPilhaChar5(PilhaChar5 *p, char *valor) {
    if (pilhaCharVazia5(p)) {
        //printf("Pilha vazia! Não é possível remover elementos.\n");
        return false; // Pilha vazia
    }
    *valor = p->dados[p->topo];
    p->topo--;
    return true;
}

/* Retorna true se o caractere é um símbolo de abertura */
static bool isOpening(char c)
{
    return c == '(' || c == '[' || c == '{';
}

/* Retorna true se o caractere é um símbolo de fechamento */
static bool isClosing(char c)
{
    return c == ')' || c == ']' || c == '}';
}

/* Retorna o símbolo de abertura correspondente ao fechamento informado */
static char openingSymbol(char fechamento)
{
    switch (fechamento) {
        case ')': return '(';
        case ']': return '[';
        case '}': return '{';
        default:  return '\0';
    }
}

/* Verifica se os símbolos de agrupamento em texto estão corretamente
 * abertos e fechados, na ordem correta, utilizando uma Pilha. */
bool balancedText(const char *text)
{
    PilhaChar5 p;

    inicializarPilhaChar5(&p);

    for (int i = 0; text[i] != '\0'; i++) {
        char c = text[i];

        // Se o caractere for um símbolo de abertura, empilha-o
        if (isOpening(c)) {
            if (!pushPilhaChar5(&p, c)) {
                return false;
            }
        // Se o caractere for um símbolo de fechamento, verifica se corresponde ao topo da pilha
        } else if (isClosing(c)) {
            char valor;

            if (pilhaCharVazia5(&p)) {
                return false; 
            }

            popPilhaChar5(&p, &valor);

            if (valor != openingSymbol(c)) {
                return false; 
            }
        }
    }

    // Se a pilha estiver vazia no final, todos os símbolos foram balanceados
    return pilhaCharVazia5(&p); 
}


void problem_5(void)
{
    draw_problem_screen(2,(const char *) ascii_problema_5);
    //----------------------------------------------------

    textcolor(WHITE); textbackground(BLACK);

    char textInput[CAPACIDADE];

    gotoxy(9, 12);
    printf("A parada tá balanceada?\n");
    gotoxy(9, 13);
    printf("Digite 'sair' para encerrar.\n\n");

    while (1) {
        gotoxy(9, 14);textcolor(YELLOW);
        printf("Mande bala, qual a expressão, meu nobre?: ");
        if (fgets(textInput, sizeof(textInput), stdin) == NULL) break;

        textcolor(CYAN);
        textInput[strcspn(textInput, "\r\n")] = '\0';

        if (strcmp(textInput, "sair") == 0) {
            gotoxy(9, 18);textcolor(RED);
            printf("Saindo...\n");
            break;
        }

        if (strlen(textInput) == 0) continue;

        
        if (balancedText(textInput)) {
            gotoxy(9, 16);textcolor(GREEN);            
            printf("Resposta: \"%s\" está balanceado, meu bom.\n\n", textInput);
        } else {
            gotoxy(9, 16);textcolor(RED);
            printf("Resposta: \"%s\" não está balanceado, meu bom.\n\n", textInput);
        }
        gotoxy(9, 14);printf("                                                   ");
    }

    //---------------------------------------------------
    waitEsc();
}
