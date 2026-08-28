#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <stdio.h>

// Define as dimensões da tela do console
#define SCREEN_HEIGHT 24
#define SCREEN_WIDTH  80

// Atributos do texto no console
#define RESETATTR    0
#define BRIGHT       1
#define DIM          2
#define UNDERSCORE   4
#define BLINK        5   /* pode não funcionar em todos os monitores. */
#define REVERSE      7
#define HIDDEN       8

// Cores do texto no console Foreground   Background
#define BLACK        0x0    // \030[30m   \033[40m
#define RED          0x1    // \030[31m   \033[41m
#define GREEN        0x2    // \030[32m   \033[42m
#define BROWN        0x3    // \030[33m   \033[43m
#define BLUE         0x4    // \030[34m   \033[44m
#define MAGENTA      0x5    // \030[35m   \033[45m
#define CYAN         0x6    // \030[36m   \033[46m
#define LIGHTGREY    0x7    // \030[37m   \033[47m
#define DARKGREY     0x10   // \030[30;1m \033[40;1m
#define LIGHTRED     0x11   // \030[31;1m \033[41;1m
#define LIGHTGREEN   0x12   // \030[32;1m \033[42;1m
#define YELLOW       0x13   // \030[33;1m \033[43;1m
#define LIGHTBLUE    0x14   // \030[34;1m \033[44;1m
#define LIGHTMAGENTA 0x15   // \030[35;1m \033[45;1m
#define LIGHTCYAN    0x16   // \030[36;1m \033[46;1m
#define WHITE        0x17   // \030[37;1m \033[47;1m

// Caracteres espciais
#define CHAR_DOT     "·"
#define CHAR_BLANK   " "

// Menu de opções
#define MENU_MAIN_VISIBLE 10    // Número de opções visíveis no menu principal
#define MENU_MAIN_ITEMS   14    // Número total de opções no menu principal

extern char *mainmenu[MENU_MAIN_ITEMS];

// Oculta o cursor do console
#define hidecursor() puts("\e[?25l")   

// Mostra o cursor do console
#define showcursor() puts("\e[?25h")   

// Limpa a tela do console e posiciona o cursor no canto superior esquerdo
//#define clrscr() puts ("\e[2J\e[1;1H") 
#define clrscr() printf("\033[2J\033[3J\033[H")

// Limpa a linha atual do console
#define clreol() puts("\e[K")          

// Remove todo o conteúdo da linha atual
#define delline() puts("\e[2k")

// Posiciona o cursor do console na coordenada (x, y)
#define gotoxy(x,y) printf("\e[%d;%dH", y, x)

// Define os atributos do texto no console
#define textattr(attr) printf("\e[%dm", (attr)) 

// Define a cor do texto no console
#define textcolor(color) printf("\e[%d;%dm", ((color) & 0x10) ? 1 : 0, ((color) & 0xF) + 30) 

// Define a cor de fundo do texto no console
#define textbackground(color) printf("\e[%d;%dm", ((color) & 0x10) ? 1 : 0, ((color) & 0xF) + 40) 

/*
 * Função para exibir texto rolando no console.
 * @param _str: Ponteiro para o array de caracteres a ser exibido.
 * @param _size: Tamanho do array de caracteres.
 * @param _length: Comprimento máximo da linha a ser exibida.
 */
void textScrolling(const unsigned char *_str, unsigned int _size, int _length);

// Buffer de saída de um quadro de animação (definido em graphics.c)
typedef struct FrameBuf FrameBuf;

/*
 * Função para exibir texto rolando no console preservando o que já está desenhado
 * por trás (ex.: um fundo). Ao contrário de textScrolling(), não limpa/reescreve
 * as células que correspondem a espaços em branco na arte.
 * @param _str: Ponteiro para o array de caracteres a ser exibido.
 * @param _size: Tamanho do array de caracteres.
 * @param _length: Comprimento máximo da linha a ser exibida.
 * @param base_x: Coluna inicial onde a arte será desenhada.
 * @param base_y: Linha inicial onde a arte será desenhada.
 * @param restore_bg: Função chamada para repintar uma célula (x, y) que fica em
 *                    branco no quadro atual, evitando rastros da arte. Pode ser NULL.
 */
void textScrollingOverBg(const unsigned char *_str, unsigned int _size, int _length, int base_x, int base_y, void (*restore_bg)(FrameBuf *fb, int x, int y));

/*
 * Função para exibir texto rolando no console em dobro.
 * @param x: Coordenada x inicial.
 * @param y: Coordenada y inicial.
 * @param text: Ponteiro para o array de caracteres a ser exibido.
 * @param direction: Direção do movimento ('L' para esquerda, 'R' para direita).
 * @param color: Cor do texto.
 * @param background: Cor de fundo do texto.
 */
void textScrollingDoubleH(const int x, const int y, char *text, char direction, int color, int background);

/*
 * Adiciona caracteres a uma string no console, um por vez, com efeito de animação.
 * @param x: Coordenada x inicial.
 * @param y: Coordenada y inicial.
 * @param text: Ponteiro para o array de caracteres a ser exibido.
 * @param txtcolor: Cor do texto.
 * @param bkgcolor: Cor de fundo do texto.
 */
void textAddChar(const int x, const int y, char *text, int txtcolor, int bkgcolor);

/*
 * Função para exibir a animação de boas-vindas no console.
 * @return: Retorna 0 em caso de sucesso.
 */
int draw_welcome(void);

/*
 * Função para exibir a animação de despedida no console.
 * @return: Retorna 0 em caso de sucesso.
 */
int draw_goodbye(void);

/*
 * Função para exibir o menu principal no console.
 */
void draw_menu(void);

/*
 * Função para exibir o fundo no estilo Mondrian no console.
 */
void draw_mondrian(void);

/*
 * Função para exibir um céu nublado no console.
 */
void draw_cloudy_sky(void);

/*
 * Função para exibir a animação do helicóptero (turndownforwhat1) sobre o
 * fundo de céu nublado no console.
 * @return: Retorna 0 em caso de sucesso.
 */
int draw_turndownforwhat(void);

/*
 * Função para exibir os itens do menu no console.
 * @param index: Índice do item do menu.
 * @param selected: Indica se o item está selecionado (1) ou não (0).
 */
void draw_menu_items(int index, int selected);

/*
 * Função para renderizar a tela padrão de um problema.
 * @param selected: Índice do item selecionado no menu.
 * @param title: Título do problema a ser exibido.
 * @param line1: Primeira linha do conteúdo.
 * @param line2: Segunda linha do conteúdo.
 * @param line3: Terceira linha do conteúdo.
 */
void draw_problem_screen(int selected, const char *title);

/*
 * Função para exibir uma área genérica de conteúdo para o item selecionado.
 * @param selected: Índice do item selecionado no menu.
 */
void draw_content_placeholder(int selected);

#endif /* GRAPHICS_H */