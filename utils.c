#include <windows.h>
#include <conio.h>
#include <time.h>
#include "utils.h"
#include "graphics.h"

// Função para inicializar o console com suporte a UTF-8 e processamento de terminal virtual
void init_console(void)
{
    DWORD mode;
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    if (GetConsoleMode(output, &mode)) 
    {
        SetConsoleMode(output, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }    
}

// Função para capturar o estado das teclas pressionadas
void delay_us(int microseconds)
{
    if (microseconds <= 0) return;
    LARGE_INTEGER frequency, start, current;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    LONGLONG target = start.QuadPart + (microseconds * frequency.QuadPart) / 1000000;
    do {
        QueryPerformanceCounter(&current);
    } while (current.QuadPart < target);
}

// Função para capturar a tecla pressionada no teclado
int getch(void)
{
    static int pending_first  = 0;
    static int pending_second = 0;
    int key;

    if (pending_first) {
        key = pending_first;
        pending_first = pending_second;
        pending_second = 0;
        return key;
    }

    key = _getch();

    if (key == 0 || key == 224) {
        key = _getch();
        if (key == 72) {
            pending_first = 91;
            pending_second = 65;
        } else if (key == 80) {
            pending_first = 91;
            pending_second = 66;        
        } else if (key == 77) {
            pending_first = 91;
            pending_second = 67;
        } else if (key == 75) {
            pending_first = 91;
            pending_second = 68;
        } else {
            return key;
        }
        return 27;
    }
    return key == 13 ? 10 : key;
}

KEY getKeypressed(void)
{
    KEY tmp;
    tmp.x = tmp.y = tmp.z = 0;
    
    int ch = _getch();
    if (ch == 0 || ch == 224)
    {
        int ext = _getch();
        switch (ext)
        {
            case 72: tmp.x = 27; tmp.y = 91; tmp.z = 65; break; // Up arrow
            case 80: tmp.x = 27; tmp.y = 91; tmp.z = 66; break; // Down arrow
            case 77: tmp.x = 27; tmp.y = 91; tmp.z = 67; break; // Right arrow
            case 75: tmp.x = 27; tmp.y = 91; tmp.z = 68; break; // Left arrow
            default: tmp.x = ch; tmp.y = ext; break;
        }
    }
    else
    {
        tmp.x = (ch == 13) ? 10 : ch; // Enter key
    }

    return tmp;
}   

/*
* Função para aguardar até que o usuário pressione a tecla ESC ou Ctrl+X.
* @return: Retorna 1 quando a tecla ESC ou Ctrl+X for pressionada.
*/
int waitEsc(void)
{    
    textcolor(YELLOW);textbackground(BLACK);
    gotoxy(9, 21);
    printf("Pressione ESC para voltar ao menu...                            ");
    gotoxy(45, 21);
    int ch = 0;

    do
    {
        ch = _getch();

        if (ch == 0 || ch == 224)
        {
            _getch();
            ch = 0;
        }
    } while (ch != 27);

    return (1);
}