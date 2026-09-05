#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "../graphics.h"
#include "../utils.h"
#include "problem_6.h"

/*
* PROBLEMA 6 - n crianças em roda passam uma batata, 
* a cada k rodadas quem estiver com ela sai da brincadeira,
* implemente a função int batataquente (n, k) e devolve o 
* numero da criança que sobra no final, considerando que as 
* crianças estão numeradas de 1 a n.
*/
unsigned char ascii_problema_6[] = {
  0x50, 0x52, 0x4f, 0x42, 0x4c, 0x45, 0x4d, 0x41, 0x20, 0x36, 0x20, 0x2d, 
  0x20, 0x6e, 0x20, 0x63, 0x72, 0x69, 0x61, 0x6e, 0xc3, 0xa7, 0x61, 0x73, 
  0x20, 0x65, 0x6d, 0x20, 0x72, 0x6f, 0x64, 0x61, 0x20, 0x70, 0x61, 0x73, 
  0x73, 0x61, 0x6d, 0x20, 0x75, 0x6d, 0x61, 0x20, 0x62, 0x61, 0x74, 0x61, 
  0x74, 0x61, 0x2c, 0x20, 0x0a, 0x61, 0x20, 0x63, 0x61, 0x64, 0x61, 0x20, 
  0x6b, 0x20, 0x72, 0x6f, 0x64, 0x61, 0x64, 0x61, 0x73, 0x20, 0x71, 0x75, 
  0x65, 0x6d, 0x20, 0x65, 0x73, 0x74, 0x69, 0x76, 0x65, 0x72, 0x20, 0x63, 
  0x6f, 0x6d, 0x20, 0x65, 0x6c, 0x61, 0x20, 0x73, 0x61, 0x69, 0x20, 0x64, 
  0x61, 0x20, 0x62, 0x72, 0x69, 0x6e, 0x63, 0x61, 0x64, 0x65, 0x69, 0x72, 
  0x61, 0x2c, 0x0a, 0x69, 0x6d, 0x70, 0x6c, 0x65, 0x6d, 0x65, 0x6e, 0x74, 
  0x65, 0x20, 0x61, 0x20, 0x66, 0x75, 0x6e, 0xc3, 0xa7, 0xc3, 0xa3, 0x6f, 
  0x20, 0x69, 0x6e, 0x74, 0x20, 0x62, 0x61, 0x74, 0x61, 0x74, 0x61, 0x71, 
  0x75, 0x65, 0x6e, 0x74, 0x65, 0x20, 0x28, 0x6e, 0x2c, 0x20, 0x6b, 0x29, 
  0x20, 0x71, 0x75, 0x65, 0x20, 0x64, 0x65, 0x76, 0x6f, 0x6c, 0x76, 0x65, 
  0x20, 0x6f, 0x20, 0x0a, 0x6e, 0xc3, 0xba, 0x6d, 0x65, 0x72, 0x6f, 0x20, 
  0x64, 0x61, 0x20, 0x63, 0x72, 0x69, 0x61, 0x6e, 0xc3, 0xa7, 0x61, 0x20, 
  0x71, 0x75, 0x65, 0x20, 0x73, 0x6f, 0x62, 0x72, 0x61, 0x20, 0x6e, 0x6f, 
  0x20, 0x66, 0x69, 0x6e, 0x61, 0x6c, 0x2c, 0x20, 0x63, 0x6f, 0x6e, 0x73, 
  0x69, 0x64, 0x65, 0x72, 0x61, 0x6e, 0x64, 0x6f, 0x20, 0x71, 0x75, 0x65, 
  0x20, 0x61, 0x73, 0x20, 0x0a, 0x63, 0x72, 0x69, 0x61, 0x6e, 0xc3, 0xa7, 
  0x61, 0x73, 0x20, 0x65, 0x73, 0x74, 0xc3, 0xa3, 0x6f, 0x20, 0x6e, 0x75, 
  0x6d, 0x65, 0x72, 0x61, 0x64, 0x61, 0x73, 0x20, 0x64, 0x65, 0x20, 0x31, 
  0x20, 0x61, 0x20, 0x6e, 0x2e
};
unsigned int ascii_problema_6_length = 269;

// Inicializa a fila
void inicializarFila(Fila *f) {
    f->inicio = NULL;
    f->fim = NULL;
    f->quantidade = 0;
}

// Verifica se a fila está vazia
bool filaVazia(Fila *f) {
    return (f->inicio == NULL);
}

// Inserir elemento na fila (enqueue)
bool enqueue(Fila *f, int valor) {
    No *novo = (No *) malloc(sizeof(No));
    if (!novo) return false;
    novo->valor = valor;
    novo->prox = NULL;
    if (f->fim == NULL) {
        f->inicio = novo;
        f->fim = novo;
    } else {
        f->fim->prox = novo;
        f->fim = novo;
    }
    f->quantidade++;
    return true;
}

// Remover elemento da fila (dequeue)
bool dequeue(Fila *f, int *valor) {
    if (filaVazia(f)) return false;
    No *temp = f->inicio;
    *valor = temp->valor;
    f->inicio = f->inicio->prox;
    if (f->inicio == NULL) {
        f->fim = NULL;
    }
    free(temp);
    f->quantidade--;
    return true;
}

// Destrói a fila e libera toda a memória alocada
void destruirFila(Fila *f) {
    int dummy;
    while (dequeue(f, &dummy));
}

// Resolve o problema da Batata Quente usando Fila
int hotPotato(int n, int k) {

    if (n <= 0 || k <= 0) {
        return 0;
    }

    Fila f;
    inicializarFila(&f);

    // Enfileira as crianças de 1 a n
    for (int i = 1; i <= n; i++) {
        enqueue(&f, i);
    }

    // Passa a batata até sobrar apenas uma criança
    while (f.quantidade > 1) {

        // Passa a batata k - 1 vezes (desenfileira e reenfileira)
        for (int i = 1; i < k; i++) {

            int crianca;

            if (dequeue(&f, &crianca)) {

                enqueue(&f, crianca);

            }

        }

        // A k-ésima criança sai da brincadeira (eliminada)

        int eliminado;

        dequeue(&f, &eliminado);
    }

    int vencedor = 0;

    dequeue(&f, &vencedor);

    destruirFila(&f);

    return vencedor;
}

void problem_6(void)
{
    draw_problem_screen(2,(const char *) ascii_problema_6);
    //----------------------------------------------------

    textcolor(WHITE); textbackground(BLACK);

    char buffer[100];
    int n, k;

    gotoxy(9, 12);
    printf("A batata está quente, meu nobre");
    gotoxy(9, 13);
    printf("Digite 'sair' para encerrar.");

    while (1) {
        gotoxy(9, 14); textcolor(YELLOW);
        printf("                                                                 ");
        gotoxy(9, 14);
        printf("Número de crianças (n): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;

        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (strcmp(buffer, "sair") == 0) {
            gotoxy(9, 18); textcolor(RED);
            printf("Saindo...\n");
            break;
        }

        if (strlen(buffer) == 0) continue;
        
        n = atoi(buffer); // Converte a entrada para inteiro

        if (n <= 0) {
            gotoxy(9, 16); textcolor(RED);
            printf("Número inválido para n! Tente novamente.                          ");
            continue;
        }

        gotoxy(9, 15); textcolor(YELLOW);
        printf("                                                                 ");
        gotoxy(9, 15);
        printf("Passos a cada eliminação (k): ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) break;

        buffer[strcspn(buffer, "\r\n")] = '\0';

        if (strcmp(buffer, "sair") == 0 || strcmp(buffer, "0") == 0) {
            gotoxy(9, 18); textcolor(RED);
            printf("Saindo...\n");
            break;
        }

        k = atoi(buffer);

        if (k <= 0) {
            gotoxy(9, 16); textcolor(RED);
            printf("Número inválido para k! Tente novamente.                          ");
            continue;
        }

        int vencedor = hotPotato(n, k);

        gotoxy(9, 16); textcolor(GREEN);
        printf("Resposta: A criança vencedora é a %d.", vencedor);

        gotoxy(9, 14); printf("                                                                 ");
        gotoxy(9, 15); printf("                                                                 ");
    }

    //---------------------------------------------------
    waitEsc();
}