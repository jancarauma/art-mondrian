#ifndef COBRA_H
#define COBRA_H

#include "../../utils.h"
#include "../../graphics.h"

// *******************
// Parâmetros da Arena
// *******************

// Tabuleiro
#define HEIGHT 14
#define WIDTH 64
#define FIELD_SIZE ( HEIGHT * WIDTH )

// Canto superior esquerdo da arena dentro da área escura
// desenhada por draw_problem_screen() (x: 8..73, y: 6..22).
#define ARENA_X 9
#define ARENA_Y 6

// Posiciona o cursor em uma célula (px, py) da arena.
#define arena_gotoxy(px, py) gotoxy(ARENA_X + (px), ARENA_Y + (py))

// Número do problema associado ao easter egg.
#define COBRA_PROBLEM 3.1415926535897932384626433832795

// *****************************************
// Caracteres UTF-8 usados no desenho da arena
// (o console é colocado em CP_UTF8 por init_console())
// *****************************************

#define CHAR_WALL_H     "═"
#define CHAR_WALL_V     "║"
#define CHAR_CORNER_TL  "╔"
#define CHAR_CORNER_TR  "╗"
#define CHAR_CORNER_BL  "╚"
#define CHAR_CORNER_BR  "╝"
#define CHAR_SNAKE_HEAD "◉"
#define CHAR_SNAKE_BODY "●"
#define CHAR_FOOD       "◆"
#define CHAR_EMPTY      " "

// Mete bala
#define CHAR_BULLET_UP    "▲"
#define CHAR_BULLET_DOWN  "▼"
#define CHAR_BULLET_LEFT  "◄"
#define CHAR_BULLET_RIGHT "►"
#define BULLET_COLOR      YELLOW

// Escreve um caractere UTF-8 na posição atual do cursor.
#define putstr(s) fputs((s), stdout)

// Outras infos
#define FOOD 0
#define UNDEFINED (( HEIGHT + 1 ) * ( WIDTH + 1 ))
#define SNAKE ( 2* UNDEFINED )

// *******************
// Parâmetros da Cobra
// *******************

// A cabeça da cobra está sempre no primeiro elemento do array. 
// A cauda da cobra está sempre no último elemento do array.
#define HEAD 0 

// Deslocamentos
#define LEFT  ( -1 )
#define RIGHT (  1 )
#define UP    ( -WIDTH )
#define DOWN  (  WIDTH )

// Nenhum movimento
#define ERR   ( -1111 )

// Tecla de saída
#define KEY_ESC_CODE 27

// Tecla de disparo
#define KEY_SPACE_CODE 32

/*
* Função principal do jogo da cobra.
* @return: Retorna 0 em caso de sucesso.
*/
int cobraRun(void);

#endif /* COBRA_H */