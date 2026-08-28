#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#include "graphics.h"
#include "utils.h"
#include "ascii/django.h"
#include "ascii/ufrr.h"
#include "ascii/turndownforwhat1.h"
#include "ascii/goodbye.h"

/*
 * Acumula toda a saída de um quadro de animação em memória para que seja
 * enviada ao terminal em uma única escrita, evitando que centenas de
 * pequenas chamadas printf/gotoxy por quadro deixem a animação lenta
 * quando há muitos caracteres visíveis.
 */
typedef struct FrameBuf
{
    char *buf;
    size_t len;
    size_t cap;
} FrameBuf;

static void fb_reserve(FrameBuf *fb, size_t cap)
{
    fb->buf = (char *) malloc(cap);
    fb->len = 0;
    fb->cap = cap;
}

static void fb_append(FrameBuf *fb, const char *fmt, ...)
{
    if (fb->len >= fb->cap) return;
    va_list args;
    va_start(args, fmt);
    int n = vsnprintf(fb->buf + fb->len, fb->cap - fb->len, fmt, args);
    va_end(args);
    if (n > 0) fb->len += (size_t) n;
}

static void fb_putbytes(FrameBuf *fb, const char *bytes, size_t n)
{
    if (fb->len + n > fb->cap) return;
    memcpy(fb->buf + fb->len, bytes, n);
    fb->len += n;
}

static void fb_gotoxy(FrameBuf *fb, int x, int y)
{
    fb_append(fb, "\e[%d;%dH", y, x);
}

static void fb_textcolor(FrameBuf *fb, int color)
{
    fb_append(fb, "\e[%d;%dm", (color & 0x10) ? 1 : 0, (color & 0xF) + 30);
}

static void fb_textbackground(FrameBuf *fb, int color)
{
    fb_append(fb, "\e[%d;%dm", (color & 0x10) ? 1 : 0, (color & 0xF) + 40);
}

static void fb_flush(FrameBuf *fb)
{
    fwrite(fb->buf, 1, fb->len, stdout);
    fflush(stdout);
    fb->len = 0;
}

// Repinta uma célula do fundo de céu nublado (definida mais abaixo no arquivo)
static void cloudy_sky_restore_cell(FrameBuf *fb, int x, int y);

char *mainmenu[MENU_MAIN_ITEMS] = {
	" Aula 01 | Núm. Total de Colisões ······· 21/08/2026 ",  
	" Problema 02 - ····································· ",  
    " Problema 03 - ····································· ",  
    " Problema 04 - ····································· ",  
    " Problema 05 - ····································· ",  
    " Problema 06 - ····································· ",  
    " Problema 07 - ····································· ",  
    " Problema 08 - ····································· ",  
    " Problema 09 - ····································· ",  
    " Problema 10 - ····································· ",  
    " Problema 11 - ····································· ",  
    " Problema 12 - ····································· ",  
    " Problema 13 - ····································· ",  
    " ·······································C·O·B·R·A··· ",
};

/*
 * Função para exibir texto rolando no console.
 * @param _str: Ponteiro para o array de caracteres a ser exibido.
 * @param _size: Tamanho do array de caracteres.
 * @param _length: Comprimento máximo da linha a ser exibida.
 */
void textScrolling(const unsigned char *_str, unsigned int _size, int _length)
{
    int i, j;
    int head;
    int l = 0;  // número de linhas do blob
    int n = 0;  // largura da linha mais longa
    unsigned int start, end;

    // Descobre quantas linhas existem e qual é a mais longa
    for (start = 0, end = 0; end < _size; end++)
    {
        if (_str[end] == '\n')
        {
            if ((int) (end - start) > n) n = (int) (end - start);
            l++;
            start = end + 1;
        }
    }
    if (start < _size)
    {
        if ((int) (_size - start) > n) n = (int) (_size - start);
        l++;
    }
    if (l == 0) return;

    char *str[l];

    // Preenche a string com espaços e a string original
    start = 0;
    for (i = 0; i < l; i++)
    {
        // Localiza o fim da linha atual dentro do blob
        end = start;
        while (end < _size && _str[end] != '\n') end++;

        // Aloca memória para cada string com espaço adicional para o comprimento máximo e a string original
        // +3 para o espaço antes, o espaço depois e o caractere nulo
        str[i] = (char *) malloc ((_length + n + 3) * sizeof(char));

        for (j = 0; j <=  _length; j++)
           str[i][j] = ' ';
        // Linhas mais curtas que a maior são completadas com espaços
        for (j = 0; j < n; j++)
           str[i][_length + 1 + j] = (start + (unsigned int) j < end) ? (char) _str[start + j] : ' ';
        str[i][_length + 1 + n] = ' ';
        str[i][_length + 2 + n] = '\0';

        start = end + 1;
    }

    // Imprimir fazendo rolagem (não imprimir os primeiros caracteres - head)
    n = strlen(str[0]);
    for (head = 0; head < n;)
    {
        // Reposiciona o cursor sem limpar a tela inteira
        printf("\x1b[0m\x1b[H");
        for (i = 0; i < l; i++)
        {
            // Mudar cor do texto -> VERMELHO            
            textcolor(LIGHTRED);
            int column = 0, bytes = 0;
            while (str[i][head + bytes] && column < _length)
            {
                // Imprime o caractere atual
                unsigned char byte = (unsigned char) str[i][head + bytes];
                // Determina o número de bytes do caractere UTF-8
                int character_bytes = (byte < 0x80) ? 1 : (byte < 0xE0) ? 2: (byte < 0xF0) ? 3 : 4;
                // Se o próximo caractere ultrapassar o comprimento, saia do loop
                if (column + 1 > _length) break; 
                // Imprime o caractere atual
                fwrite(str[i] + head + bytes, 1, character_bytes, stdout);
                // Atualiza o número de bytes e a coluna
                bytes += character_bytes;
                // Atualiza a coluna (cada caractere ocupa uma coluna)
                column++;
            }
            // Linux: printf("%.*s", _length, str[i] + head);
            // Repositiona o cursor para a próxima linha
            printf("\x1b[0m\n");
        }
        // Aguarda um curto período antes de atualizar a tela
        fflush(stdout);
        usleep(25000); // 40ms
        // Atualiza o índice do cabeçalho para a próxima posição, considerando caracteres UTF-8
        unsigned char byte = (unsigned char) str[0][head];
        // Incrementa o índice do cabeçalho com base no número de bytes do caractere UTF-8
        head += (byte < 0x80) ? 1 : (byte < 0xE0) ? 2: (byte < 0xF0) ? 3 : 4;
        // Linux: head++;
    }
    // Libera a memória alocada para as strings
    for (i = 0; i < l; i++)
    {
        free(str[i]);
    }        
}

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
void textScrollingOverBg(const unsigned char *_str, unsigned int _size, int _length, int base_x, int base_y, void (*restore_bg)(FrameBuf *fb, int x, int y))
{
    int i, j;
    int head;
    int l = 0;  // número de linhas do blob
    int n = 0;  // largura da linha mais longa
    unsigned int start, end;

    // Descobre quantas linhas existem e qual é a mais longa
    for (start = 0, end = 0; end < _size; end++)
    {
        if (_str[end] == '\n')
        {
            if ((int) (end - start) > n) n = (int) (end - start);
            l++;
            start = end + 1;
        }
    }
    if (start < _size)
    {
        if ((int) (_size - start) > n) n = (int) (_size - start);
        l++;
    }
    if (l == 0) return;

    char *str[l];

    // Preenche a string com espaços e a string original
    start = 0;
    for (i = 0; i < l; i++)
    {
        end = start;
        while (end < _size && _str[end] != '\n') end++;

        str[i] = (char *) malloc ((_length + n + 3) * sizeof(char));

        for (j = 0; j <=  _length; j++)
           str[i][j] = ' ';
        for (j = 0; j < n; j++)
           str[i][_length + 1 + j] = (start + (unsigned int) j < end) ? (char) _str[start + j] : ' ';
        str[i][_length + 1 + n] = ' ';
        str[i][_length + 2 + n] = '\0';

        start = end + 1;
    }

    // Marca quais células da janela tinham glifo no quadro anterior, para só
    // restaurar o fundo onde realmente houve mudança (evita repintar tudo a cada quadro)
    char *prev_drawn = (char *) calloc((size_t) l * (size_t) _length, sizeof(char));

    // Acumula todo o quadro em memória e envia ao terminal em uma única escrita,
    // em vez de uma chamada printf/gotoxy por caractere (isso que deixava lento
    // quanto mais caracteres estavam visíveis)
    FrameBuf fb;
    fb_reserve(&fb, (size_t) l * (size_t) _length * 32 + 256);

    // Imprimir fazendo rolagem (não imprimir os primeiros caracteres - head)
    n = strlen(str[0]);
    for (head = 0; head < n;)
    {
        // Marca o início do quadro para manter a velocidade constante, já que a
        // quantidade de células redesenhadas varia conforme a densidade da arte
        clock_t frame_start = clock();

        for (i = 0; i < l; i++)
        {
            fb_textcolor(&fb, WHITE);
            int column = 0, bytes = 0;
            while (str[i][head + bytes] && column < _length)
            {
                unsigned char byte = (unsigned char) str[i][head + bytes];
                int character_bytes = (byte < 0x80) ? 1 : (byte < 0xE0) ? 2: (byte < 0xF0) ? 3 : 4;
                if (column + 1 > _length) break;
                char *cell = &prev_drawn[i * _length + column];
                if (byte != ' ')
                {
                    fb_gotoxy(&fb, base_x + column, base_y + i);
                    fb_putbytes(&fb, str[i] + head + bytes, (size_t) character_bytes);
                    *cell = 1;
                }
                // Espaço: só repinta o fundo se havia glifo aqui no quadro anterior
                else if (*cell && restore_bg)
                {
                    restore_bg(&fb, base_x + column, base_y + i);
                    *cell = 0;
                }
                bytes += character_bytes;
                column++;
            }
        }
        fb_flush(&fb);

        // Completa o restante dos 25ms do quadro, descontando o tempo já gasto desenhando,
        // para que a velocidade de rolagem não varie com a densidade da arte
        long elapsed_us = (long) ((clock() - frame_start) * 1000000L / CLOCKS_PER_SEC);
        long remaining_us = 25000L - elapsed_us;
        if (remaining_us > 0) usleep((int) remaining_us);

        unsigned char byte = (unsigned char) str[0][head];
        head += (byte < 0x80) ? 1 : (byte < 0xE0) ? 2: (byte < 0xF0) ? 3 : 4;
    }
    free(fb.buf);
    free(prev_drawn);
    for (i = 0; i < l; i++)
    {
        free(str[i]);
    }
}

/*
 * Mapeia os deslocamentos em bytes de cada caractere UTF-8 em uma string.
 * Retorna a quantidade de caracteres UTF-8 na string.
 */
static int get_utf8_offsets(const char *str, int *offsets)
{
    int char_count = 0;
    int byte_idx = 0;
    while (str[byte_idx] != '\0')
    {
        if (offsets)
            offsets[char_count] = byte_idx;
        char_count++;
        unsigned char byte = (unsigned char) str[byte_idx];
        int bytes = (byte < 0x80) ? 1 : (byte < 0xE0) ? 2 : (byte < 0xF0) ? 3 : 4;
        byte_idx += bytes;
    }
    if (offsets)
        offsets[char_count] = byte_idx;
    return char_count;
}

/*
 * Imprime texto com efeito rolagem dupla horizontal. Suporta UTF-8 e tamanhos pares/ímpares.
 *
 * direction = 0: <---->
 * direction = 1: --><--
 */
void textScrollingDoubleH(const int x, const int y, char *text, char direction, int color, int background)
{
    int total_bytes = (int) strlen(text);
    if (total_bytes == 0) return;

    int *offsets = (int *) malloc((total_bytes + 1) * sizeof(int));
    if (!offsets) return;

    int total_chars = get_utf8_offsets(text, offsets);
    if (total_chars == 0)
    {
        free(offsets);
        return;
    }

    int n1 = total_chars / 2;
    int n2 = total_chars - n1;
    int max_steps = (n1 > n2) ? n1 : n2;

    int i;
    if (direction == 0)
    {
        for (i = 1; i <= max_steps; i++)
        {
            textbackground(background);
            textcolor(color);

            // text1: últimos cur_i1 caracteres
            int cur_i1 = (i > n1) ? n1 : i;
            int start_char1 = n1 - cur_i1;
            int byte_start1 = offsets[start_char1];
            int byte_len1 = offsets[n1] - byte_start1;

            gotoxy(x, y);
            printf("%.*s", byte_len1, text + byte_start1);

            // text2: primeiros cur_i2 caracteres
            int cur_i2 = (i > n2) ? n2 : i;
            int byte_start2 = offsets[n1];
            int byte_len2 = offsets[n1 + cur_i2] - byte_start2;

            gotoxy(x + total_chars - cur_i2, y);
            printf("%.*s", byte_len2, text + byte_start2);

            textbackground(BLACK);
            textcolor(DARKGREY);
            gotoxy(1, 24);
            delay(15);
        }
    }
    else
    {
        for (i = 1; i <= max_steps; i++)
        {
            textbackground(background);
            textcolor(color);

            // text1: primeiros cur_i1 caracteres
            int cur_i1 = (i > n1) ? n1 : i;
            int byte_start1 = offsets[0];
            int byte_len1 = offsets[cur_i1] - byte_start1;

            gotoxy(x + n1 - cur_i1, y);
            printf("%.*s", byte_len1, text + byte_start1);

            // text2: últimos cur_i2 caracteres
            int cur_i2 = (i > n2) ? n2 : i;
            int start_char2 = total_chars - cur_i2;
            int byte_start2 = offsets[start_char2];
            int byte_len2 = offsets[total_chars] - byte_start2;

            gotoxy(x + n1, y);
            printf("%.*s", byte_len2, text + byte_start2);

            textbackground(BLACK);
            textcolor(DARKGREY);
            gotoxy(1, 24);
            delay(15);
        }
    }
    textattr(RESETATTR);
    free(offsets);
}

/*
 * Adiciona caracteres a uma string no console, um por vez, com efeito de animação.
 * @param x: Coordenada x inicial.
 * @param y: Coordenada y inicial.
 * @param text: Ponteiro para o array de caracteres a ser exibido.
 * @param txtcolor: Cor do texto.
 * @param bkgcolor: Cor de fundo do texto.
*/
void textAddChar(const int x, const int y, char *text, int txtcolor, int bkgcolor)
{
    textbackground(bkgcolor);
    textcolor(txtcolor);

    int total_bytes = (int) strlen(text);
    if (total_bytes == 0) return;

    int *offsets = (int *) malloc((total_bytes + 1) * sizeof(int));
    if (!offsets) return;

    int total_chars = get_utf8_offsets(text, offsets);
    if (total_chars == 0)
    {
        free(offsets);
        return;
    }

    char *print_string = (char *) malloc((total_bytes + 1) * sizeof(char));
    if (!print_string)
    {
        free(offsets);
        return;
    }

    const char **chars = (const char **) malloc(total_chars * sizeof(const char *));
    int *char_lens = (int *) malloc(total_chars * sizeof(int));

    int c;
    for (c = 0; c < total_chars; c++)
    {
        chars[c] = " ";
        char_lens[c] = 1;
    }

    int index, i;
    for (index = 0; index < total_chars; index++)
    {
        int src_byte_start = offsets[index];
        int src_byte_len = offsets[index + 1] - src_byte_start;

        if (src_byte_len == 1 && text[src_byte_start] == ' ')
        {
            chars[index] = " ";
            char_lens[index] = 1;
            continue;
        }

        const char *cur_char = text + src_byte_start;

        for (i = total_chars - 1; i >= index; i--)
        {
            chars[i] = cur_char;
            char_lens[i] = src_byte_len;

            int pos = 0;
            for (c = 0; c < total_chars; c++)
            {
                memcpy(print_string + pos, chars[c], char_lens[c]);
                pos += char_lens[c];
            }
            print_string[pos] = '\0';

            gotoxy(x, y);
            printf("%s\r", print_string);
            fflush(stdout);

            int delay_val = 15000 / (total_chars - index);
            if (delay_val < 500) delay_val = 500;
            delay_us(delay_val);

            if (i != index)
            {
                chars[i] = " ";
                char_lens[i] = 1;
            }
        }
    }

    int pos = 0;
    for (c = 0; c < total_chars; c++)
    {
        memcpy(print_string + pos, chars[c], char_lens[c]);
        pos += char_lens[c];
    }
    print_string[pos] = '\0';

    gotoxy(x, y);
    printf("%s\n", print_string);
    fflush(stdout);

    textattr(RESETATTR);
    free(print_string);
    free((void *)chars);
    free(char_lens);
    free(offsets);
}

/*
 * Função para exibir a animação de boas-vindas no console.
 * @return: Retorna 0 em caso de sucesso.
 */
int draw_welcome(void)
{
    int i;
    
    // limpa tela
    clrscr();

    // Cabeçalho
    textScrollingDoubleH(24,  4,  "UNIVERSIDADE FEDERAL DE  RORAIMA - UFRR", 1, RED, BLACK);
    textScrollingDoubleH(18,  5,  "PRÓ-REITORIA DE PESQUISA E DE PÓS-GRADUAÇÃO - PRPPG", 1, BLUE, BLACK);
    textScrollingDoubleH(16,  6,  "PROGRAMA DE PÓS-GRADUAÇÃO EM COMPUTAÇÃO APLICADA - PPCA", 1, YELLOW, BLACK);
    textScrollingDoubleH(19,  7,  "MESTRADO PROFISSIONAL EM INFORMÁTICA NA EDUCAÇÃO", 1, WHITE, BLACK);
    textScrollingDoubleH(32,  8,  "PROGRAMAÇÃO ESTRUTURADA", 1, RED, BLACK);
    textScrollingDoubleH(40, 20,  "Ago/2026", 1, YELLOW, BLACK);
    textScrollingDoubleH(37, 19,  "Boa Vista,  RR", 1, BLUE, BLACK);
    textScrollingDoubleH(33,  10,  "Prof.  Dr. Filipi Dwan", 0, WHITE, BLACK);
	textAddChar(28,  12, "Aluno:  Janderson Gomes da Silva", YELLOW, BLACK);
	textAddChar(36,  13, "Mat.: 2026102018", BLUE, BLACK);
	textAddChar(31,  15, "ATIVIDADES EM SALA DE AULA", RED, BLACK);
    textScrollingDoubleH(33, 17,  "  pense diferente :o) ", 0, WHITE, BLACK);
    delay(1000);
    for (i = 0; i < 22; i++)
	{
		gotoxy(33, 17);
		if (i % 2 == 0)
		{
            puts("  \x1B[31mp\x1B[33men\x1B[34ms\x1B[37me d\x1B[31mi\x1B[33mfe\x1B[34mr\x1B[37men\x1B[31mt\x1B[33me :\x1B[34mo\x1B[37m) ");
		} else {
            puts("  \x1B[34mpe\x1B[31mn\x1B[37mse\x1B[33m d\x1B[34mif\x1B[31me\x1B[37mre\x1B[33mn\x1B[34mte\x1B[31m :\x1B[33mo\x1B[37m) ");
		}
		delay(100);
	}

    // limpa tela
    clrscr();

    draw_cloudy_sky();
    textScrollingOverBg(
        ascii_ufrr, 
        ascii_ufrr_length,
        80, 1, 9, cloudy_sky_restore_cell);

    // UFRR Engenharia Elétrica    
    //textScrolling(
    //    ascii_ufrr,
    //    ascii_ufrr_length,
    //    80 /* coluna de origem do efeito de rolagem */
    //);

    // DJANGO: Imprime, efeito oscilante de cores
    textcolor(WHITE);textbackground(BLACK);clrscr();
    for (i = 0; i < 20; i++)
    {
        gotoxy(1, 8);
        // se o valor armazenado em i é número PAR
        if (i % 2 == 0)
        {
            // Mudar cor do texto -> VERMELHO
            textcolor(LIGHTRED);
        }
        else
        {
            // Mudar cor do texto -> AMARELO
            textcolor(RED);
        }
        // imprime o texto
        printf("%s", ascii_django);
        // aguarda 100 ms antes de prosseguir
        usleep(100000);
    }      
    
    // DJANGO: Imprime, efeito oscilante de cores
    for (i = 1; i < 8; i++)
    {
        gotoxy(1, 8 - i);        
        // imprime o texto
        printf("%s", ascii_django);
        // aguarda 100 ms antes de prosseguir
        delay(300);
    }      

    // Turn Down For What (One)
    //textScrolling(
    //    ascii_turndownforwhat1,
    //    ascii_turndownforwhat1_length,
    //    80 /* coluna de origem do efeito de rolagem */
    //);

    return (0);
}

/*
 * Função para exibir a animação de despedida no console.
 * @return: Retorna 0 em caso de sucesso.
 */
int draw_goodbye(void)
{
    int i;
    unsigned int idx;
    unsigned int size = (unsigned int) strlen((const char *) ascii_goodbye);
    int x, y;
    int base_x = 8, base_y = 8;    

    // pinta o fundo mondrian por trás da arte
    draw_mondrian();    

    // TCHAU: Imprime, efeito oscilante de cores    
    for (i = 0; i < 7; i++)
    {                
        textbackground(BLACK);
        // percorre a ascii art byte a byte, pulando espaços/quebras de linha
        // para não sobrescrever o fundo mondrian nas áreas "vazias"
        x = base_x;
        y = base_y;
        for (idx = 0; idx < size; )
        {
            unsigned char c = ascii_goodbye[idx];            
            if (c == '\n')
            {
                x = base_x;
                y++;
                idx++;
                continue;
            }
            if (c == ' ') // não imprime nada: mantém o fundo mondrian intacto
            {                
                x++;
                idx++;
                continue;
            }

            // caractere UTF-8 de 3 bytes (blocos ▓ ▒ ░ █ ▄ ▀ etc.)
            gotoxy(x, y);
            if (i % 2 == 0)
            {
                textcolor(YELLOW);
            } else {
                textcolor(CYAN);
            }
            // se o valor armazenado em i é número PAR                        
            putchar(c);
            putchar(ascii_goodbye[idx + 1]);
            putchar(ascii_goodbye[idx + 2]);
            idx += 3;
            x++;
        }
        fflush(stdout);

        // aguarda 100 ms antes de prosseguir
        usleep(100000);
    }       
    clrscr();
    gotoxy(SCREEN_WIDTH, SCREEN_HEIGHT);
    return (0);
}


void draw_mondrian(void)
{
    int i, j;
    clrscr();

    #define VX1  8
    #define VX2 18
    #define VX3 28
    #define VX4 42
    #define VX5 55
    #define VX6 65
    #define VX7 72

    #define HY1  3
    #define HY2  6
    #define HY3 10
    #define HY4 14
    #define HY5 17
    #define HY6 21

    for (i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (j = 0; j < SCREEN_WIDTH; j++)
        {
            // Linhas verticais pretas
            int v1 = (j == VX1 && i < HY5);
            int v2 = (j == VX2);
            int v3 = (j == VX3 && (i >= HY2 && i < HY6));
            int v4 = (j == VX4);
            int v5 = (j == VX5 && (i < HY3 || i >= HY4));
            int v6 = (j == VX6 && (i >= HY1 && i < HY5));
            int v7 = (j == VX7 && i >= HY3);

            // Linhas horizontais pretas
            int h1 = (i == HY1 && j >= VX2);
            int h2 = (i == HY2 && (j < VX4 || j >= VX5));
            int h3 = (i == HY3 && (j >= VX1 && j < VX6));
            int h4 = (i == HY4);
            int h5 = (i == HY5 && j < VX5);
            int h6 = (i == HY6 && j >= VX2);

            int is_black_line = (v1 || v2 || v3 || v4 || v5 || v6 || v7 ||
                                 h1 || h2 || h3 || h4 || h5 || h6);

            if (is_black_line)
            {
                textbackground(BLACK);
            }

            // BLOCOS VERMELHOS
            else if (j > VX2 && j < VX4 && i > HY2 && i < HY4) // bloco central média
            {
                textbackground(RED);
            }
            else if (j < VX1 && i > HY5) // Inf. Esquerdo Fundo
            {
                textbackground(RED);
            }
            else if (j > VX6 && j < VX7 && i > HY1 && i < HY3) // Sup. Dir. Pequeno
            {
                textbackground(RED);
            }

            // BLOCOS AMARELOS
            else if (j > VX4 && j < VX5 && i < HY1) // Topo Centro Fino
            {
                textbackground(YELLOW);
            }
            else if (j < VX1 && i < HY2) // Sup. Esq. Canto
            {
                textbackground(YELLOW);
            }
            else if (j > VX5 && j < VX6 && i > HY3 && i < HY4) // Miolo Direito
            {
                textbackground(YELLOW);
            }
            else if (j > VX7 && i > HY6) // Inf. Extremo Dir.
            {
                textbackground(YELLOW);
            }

            // BLOCOS AZUIS
            else if (j > VX1 && j < VX2 && i > HY3 && i < HY5) // Esquerda Médio
            {
                textbackground(BLUE);
            }
            else if (j > VX5 && j < VX6 && i < HY1) // Sup. Direita
            {
                textbackground(BLUE);
            }
            else if (j > VX4 && j < VX5 && i > HY5 && i < HY6) // Baixo Centro
            {
                textbackground(BLUE);
            }

            // BLOCOS BRANCOS
            else
            {
                textbackground(WHITE);
            }

            // Renderiza caractere em branco com a cor de fundo definida
            // Importante: +1 para compatibilidade conio/POSIX 1-based
            gotoxy(j + 1, i + 1); 
            printf(CHAR_BLANK);
        }
    }

    #undef VX1
    #undef VX2
    #undef VX3
    #undef VX4
    #undef VX5
    #undef VX6
    #undef VX7
    #undef HY1
    #undef HY2
    #undef HY3
    #undef HY4
    #undef HY5
    #undef HY6
}

/*
 * Calcula a cor de fundo do céu nublado para a célula (i, j) (0-based).
 * Compartilhada entre draw_cloudy_sky() e a restauração de células individuais.
 */
static int cloudy_sky_color_at(int i, int j)
{
    // 1. Definição da cor do céu
    int sky_bg = LIGHTBLUE;

    // 2. Equações lógicas simples para gerar os formatos das nuvens

    // Nuvem 1 (Canto superior esquerdo)
    int cloud1 = ( (i == 3 && j >= 8  && j <= 20) ||
                   (i == 4 && j >= 6  && j <= 24) ||
                   (i == 5 && j >= 10 && j <= 22) );

    // Nuvem 2 (Centro-direita, mais alta)
    int cloud2 = ( (i == 2 && j >= 48 && j <= 58) ||
                   (i == 3 && j >= 44 && j <= 64) ||
                   (i == 4 && j >= 42 && j <= 66) ||
                   (i == 5 && j >= 46 && j <= 60) );

    // Nuvem 3 (Lado esquerdo, menor)
    int cloud3 = ( (i == 7 && j >= 28 && j <= 38) ||
                   (i == 8 && j >= 26 && j <= 40) );

    // Nuvem 4 (Canto inferior direito)
    int cloud4 = ( (i == 9  && j >= 62 && j <= 74) ||
                   (i == 10 && j >= 60 && j <= 76) ||
                   (i == 11 && j >= 64 && j <= 72) );

    return (cloud1 || cloud2 || cloud3 || cloud4) ? YELLOW : sky_bg;
}

/*
 * Redesenha uma única célula do céu nublado na coluna/linha informadas (1-based).
 * Usado para "apagar" rastros deixados por animações sobrepostas ao fundo.
 */
static void cloudy_sky_restore_cell(FrameBuf *fb, int x, int y)
{
    fb_textbackground(fb, cloudy_sky_color_at(y - 1, x - 1));
    fb_textcolor(fb, WHITE);
    fb_gotoxy(fb, x, y);
    fb_putbytes(fb, CHAR_BLANK, 1);
}

void draw_cloudy_sky(void)
{
    int i, j;
    clrscr();

    for (i = 0; i < SCREEN_HEIGHT; i++)
    {
        for (j = 0; j < SCREEN_WIDTH; j++)
        {
            textbackground(cloudy_sky_color_at(i, j));
            textcolor(WHITE);

            // Renderiza o caractere de fundo
            gotoxy(j + 1, i + 1);
            printf(CHAR_BLANK);
        }
    }
}

/*
 * Função para exibir a animação do helicóptero (turndownforwhat1) sobre o
 * fundo de céu nublado, sem sobrescrever as áreas vazias da arte.
 * @return: Retorna 0 em caso de sucesso.
 */
int draw_turndownforwhat(void)
{
    // desenha o fundo antes da animação
    draw_cloudy_sky();

    textScrollingOverBg(
        ascii_turndownforwhat1,
        ascii_turndownforwhat1_length,
        80, /* coluna de origem do efeito de rolagem */
        1,  /* coluna inicial na tela */
        9,  /* linha inicial na tela */
        cloudy_sky_restore_cell
    );

    return (0);
}

/*
 * Função para exibir os itens do menu no console.
 * @param index: Índice do item do menu.
 * @param selected: Indica se o item está selecionado (1) ou não (0).
 */
void draw_menu_items(int index, int selected)
{
    int k;

    int max_index = MENU_MAIN_ITEMS - MENU_MAIN_VISIBLE + 1;
    if (max_index < 1) max_index = 1;

    if (index < 1) index = 1;
    if (index > max_index) index = max_index;

    for (k = 0; k < MENU_MAIN_VISIBLE; k++)
    {
        int item_idx = (index - 1) + k;
        if (item_idx >= MENU_MAIN_ITEMS) break;

        int item_num = item_idx + 1;

        gotoxy(15, 10 + k);
        if (selected == item_num)
        {
            if (item_num % 6 == 0) {        /* Múltiplo de 6 (2 e 3 ao mesmo tempo) */
                textbackground(RED);
                textcolor(WHITE);
            } else if (item_num % 3 == 0) { /* Múltiplo de 3 */
                textbackground(BLUE);
                textcolor(WHITE);
            } else if (item_num % 2 == 0) { /* Múltiplo de 2 (pares) */
                textbackground(YELLOW);
                textcolor(WHITE);
            } else {                        /* Ímpares / Outros */
                textbackground(RED);
                textcolor(WHITE);
            }
        }
        else
        {
            textbackground(BLACK);            
            textcolor(WHITE);
        }
        printf("%s", mainmenu[item_idx]);
    }

    textcolor(WHITE);
    textbackground(BLACK);
    gotoxy(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void draw_problem_screen(
    int selected, 
    const char *title)
{
    int x, y;
    char problem_title[64];
    const char *character = title;
    int row = 6;

    if (selected < 1) selected = 1;
    if (selected > MENU_MAIN_ITEMS) selected = MENU_MAIN_ITEMS;

    if (title != NULL && title[0] != '\0')
    {
        snprintf(problem_title, sizeof(problem_title), "%s", title);
    }
    else
    {
        snprintf(problem_title, sizeof(problem_title), "\e[37;40mPROBLEMA\e[33;40m %02d", selected);
    }

    draw_mondrian();

    // Área escura principal
    textbackground(BLACK);
    for (y = 6; y <= 22; y++)
    {
        for (x = 8; x <= 73; x++)
        {
            gotoxy(x, y);
            printf(" ");
        }
    }

    // Título do problema
    gotoxy(35,4);
    textbackground(BLACK);
    printf("\e[37;40mPROBLEMA\e[33;40m %02d", selected);
    while (*character  != '\0')
    {
        gotoxy(9, row);
        while (*character != '\0' && *character != '\n')
        {
            putchar(*character);
            character++;            
        }
        if (*character == '\n')
        {
            character++;
        }
        row++;
    }
    
    textcolor(WHITE); textbackground(BLACK);
    
    textcolor(YELLOW); textbackground(BLACK);
    gotoxy(22, 23);
    printf("\e[30;47m     [Esc] - Voltar a\e[37;40mo\e[30;47m Menu Principal          ");
    gotoxy(22, 24);
    printf("\e[30;47m                2026 \e[37;40m©\e[30;47m J. Caraumã              ");

    textcolor(WHITE); textbackground(BLACK);    
    gotoxy(SCREEN_WIDTH, SCREEN_HEIGHT);
}

void draw_content_placeholder(int selected)
{
    char title[64];
    snprintf(title, sizeof(title), "PROBLEMA %02d", selected);

    draw_problem_screen(
        selected,
        "Ops... Parece que não há nada aqui."
    );
}

void draw_menu(void)
{
    int i, j;

    // Padrão de fundo
    draw_mondrian();

    // Imprimir título	
	textbackground(BLACK); // plano de fundo do texto → PRETO
	textcolor(RED); // cor do texto → AZUL
	gotoxy(18, 2);
	printf("\e[47m \e[40m \e[30;47m     UNIVERSIDADE FEDER\e[37;40mA\e[34;43mL DE RORAIMA\e[33;40m \e[44m- UFRR   \e[47m  ");
	gotoxy(18, 3);
	printf(" \e[40m \e[30;47m  PÓS-GRADUAÇÃO EM COMP\e[37;40mU\e[34;43mTAÇÃO APLICA\e[37;40mD\e[33;44mA - PPCA \e[47m  \e[40m");
	gotoxy(18, 4);
	printf("\e[47m \e[40m \e[30;47mMESTRADO PROFISSIONAL E\e[37;40mM\e[30;47m INFORMÁTICA\e[40m \e[47mNA EDUCAÇ\e[37;40mÃO ");
	gotoxy(18, 5);
	printf("\e[47m \e[40m \e[30;47m            ESTRUTURA D\e[37;40mE\e[30;47m DADOS      \e[40m \e[47m         \e[40m \e[41m ");

    //gotoxy(71,15);
    //textcolor(WHITE); textbackground(BLACK);
    //printf("Caraumã");

    // Fundo externo dos itens do menu
    textbackground(BLACK); 
	for(i = 8; i <= SCREEN_HEIGHT - 4; i++)
	{
		for(j = 14; j <= SCREEN_WIDTH - 12; j++)
		{
			gotoxy(j, i);
			printf(CHAR_BLANK);
		}		
	};    

    // Fundo interno dos itens do menu
    textbackground(BLUE);
	for(i = 10; i <= SCREEN_HEIGHT - 5; i++)
	{
		for(j = 15; j <= SCREEN_WIDTH - 13; j++)
		{
			gotoxy(j, i);
			printf(CHAR_BLANK);
		}		
	};

    // Rodapé do menu
    textbackground(BLACK);
	for(i = 0; i < 68; i++)
	{		
		gotoxy(7 + i, 22);
		printf(CHAR_BLANK);
	}
	textcolor(RED); 
	gotoxy(7, 22);
	printf("\e[41m  \e[30;47m Dr. Filip\e[37;40mi\e[30;47m Dwan • A\e[37;40ml\e[30;47muno: \e[31;40mJan\e[34;40mder\e[37;40mson\e[33;40m Gomes\e[33;44m • Mat\e[37;40mr\e[30;47mícula: 202610201\e[37;40m8 ");

    textcolor(RED); 
	gotoxy(7, 23);
	printf("\e[41m  \e[30;47m     ↑, ↓ \e[37;40m•\e[30;47m Navegar \e[37;40m•\e[30;47m [ENTER] • Se\e[37;40ml\e[33;44mecionar • Ct\e[37;40mr\e[30;47ml + X • Sair    \e[40m \e[43m ");    

    //gotoxy(7, 24);    
	//printf("\e[41m  \e[30;47m     Copyr\e[37;40mi\e[30;47mght 2026 \e[37;40m©\e[30;47m J. Caraumã • https://cara\e[37;40mu\e[30;47mma.com • Roraima");    
    gotoxy(12,25);textcolor(WHITE);textbackground(BLACK);
    printf("Copyright 2026 © J. Caraumã • https://carauma.com • Roraima");
    
    gotoxy(SCREEN_WIDTH, SCREEN_HEIGHT);    
}