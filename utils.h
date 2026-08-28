#ifndef UTILS_H
#define UTILS_H

#include <windows.h>

#define usleep(microseconds) Sleep(microseconds / 1000)
#define delay(milliseconds) Sleep((DWORD)(milliseconds));

// *******
// TECLADO
// *******

#define KEY_UP_CODE 72
#define KEY_DOWN_CODE 80
#define KEY_LEFT_CODE 75
#define KEY_RIGHT_CODE 77

typedef struct tagKeypressed {
    int x;
    int y;
    int z;
} KEY;

void delay_us(int microseconds);

/**
 * Inicializa o console.
 */
void init_console(void);

/**
 * Captura a tecla pressionada no teclado.
 * @return O código da tecla pressionada.
 */
int getch(void);

/**
 * Captura o estado das teclas pressionadas.
 * @return Estrutura KEY contendo os estados das teclas.
 */
KEY getKeypressed(void);

/**
 * Aguarda até que o usuário pressione a tecla ESC ou Ctrl+X.
 * @return Retorna 1 quando a tecla ESC ou Ctrl+X for pressionada.
 */
int waitEsc(void);

#endif /* UTILS_H */