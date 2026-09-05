#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <conio.h>
#include "cobra.h"
#include "../../graphics.h"
#include "../../utils.h"
#include "../../sound.h"
#include "../../ascii/54321.h"

// Tabuleiro
int board[FIELD_SIZE];

// Cobras
#define MAX_SNAKES 8

int snakes[MAX_SNAKES][FIELD_SIZE + 1];
int snake_sizes[MAX_SNAKES];
int snake_alive[MAX_SNAKES];
int snake_count = 1;
int active_snake_idx = 0;

#define snake (snakes[active_snake_idx])
#define snake_size (snake_sizes[active_snake_idx])

// Tabuleiro temporário
int tmpboard[FIELD_SIZE];

// Cobra temporária
int tmpsnake[FIELD_SIZE + 1];

// Tamanho temporário
int tmpsnake_size = 1;

// Posições dos alimentos
int foods[MAX_SNAKES];
int food_count = 1;
int target_food = 3 * WIDTH + 3;

#define food target_food

// Melhor movimento
int best_move = ERR;

// Lista dos quatro movimentos
int mov[4] = {
    LEFT,
    RIGHT,
    UP,
    DOWN
};

// Tecla recebida
int key = ERR;

// Indica se a tecla recebida foi uma seta do teclado.
int key_is_arrow = 0;

// Pontuação
int score = 0;

// Direção do último movimento efetivo da cobra (usada para orientar o tiro)
int last_moves[MAX_SNAKES];

#define last_move (last_moves[active_snake_idx])

// Estado do tiro: ativo, posição atual e direção
int bullet_active = 0;
int bullet_pos = ERR;
int bullet_dir = ERR;
int bullet_owner = ERR;

// Paleta estilo Mondrian aplicada ao corpo da cobra.
// BLACK vira DARKGREY porque o fundo da arena já é preto.
static const int mondrian_palette[] = {
    LIGHTRED,
    WHITE,
    YELLOW,
    WHITE,
    DARKGREY,
    WHITE,
    LIGHTBLUE
};

#define MONDRIAN_COLORS ( (int)( sizeof(mondrian_palette) / sizeof(mondrian_palette[0]) ) )

// Células pintadas com a mesma cor antes de trocar de faixa
#define MONDRIAN_BAND 2

// Contador de células já pintadas
static int paint_step = 0;

// Cor com que cada célula do corpo foi pintada
static int cell_color[FIELD_SIZE];

void draw_head(void);
void kill_active_snake(void);
void draw_food(int food_idx);
void new_food(int food_idx);

// Cor da próxima célula do corpo, avançando a faixa
static int next_body_color(void)
{
    int color =
        mondrian_palette[
            (paint_step / MONDRIAN_BAND) % MONDRIAN_COLORS
        ];

    paint_step++;

    return color;
}

// Verifica se uma posição está livre (sem a cobra)
int is_cell_free(
    int idx,
    int psize,
    int *psnake
)
{
    int i;

    for (i = 0; i < psize; i++)
    {
        if (idx == psnake[i])
        {
            return 0;
        }
    }

    return 1;
}

// Verifica se uma posição está livre em relação a todas as cobras vivas.
int is_cell_free_all(int idx)
{
    int i;

    for (i = 0; i < snake_count; i++)
    {
        if (
            snake_alive[i] &&
            !is_cell_free(idx, snake_sizes[i], snakes[i])
        )
        {
            return 0;
        }
    }

    return 1;
}

int food_at(int idx)
{
    int i;

    for (i = 0; i < food_count; i++)
    {
        if (foods[i] == idx)
        {
            return i;
        }
    }

    return ERR;
}

int is_cell_free_food(int idx)
{
    return food_at(idx) == ERR;
}

int is_cell_free_food_except(int idx, int ignored_food_idx)
{
    int i;

    for (i = 0; i < food_count; i++)
    {
        if (i != ignored_food_idx && foods[i] == idx)
        {
            return 0;
        }
    }

    return 1;
}

int choose_target_food(void)
{
    int i;
    int best_food_idx = 0;
    int best_distance = FIELD_SIZE;
    int distance;
    int head_x = snake[HEAD] % WIDTH;
    int head_y = snake[HEAD] / WIDTH;

    for (i = 0; i < food_count; i++)
    {
        distance =
            abs(head_x - foods[i] % WIDTH) +
            abs(head_y - foods[i] / WIDTH);

        if (distance < best_distance)
        {
            best_distance = distance;
            best_food_idx = i;
        }
    }

    target_food = foods[best_food_idx];

    return best_food_idx;
}

int active_snake_count(void)
{
    int i;
    int count = 0;

    for (i = 0; i < snake_count; i++)
    {
        if (snake_alive[i])
        {
            count++;
        }
    }

    return count;
}

int total_snake_size(void)
{
    int i;
    int total = 0;

    for (i = 0; i < snake_count; i++)
    {
        if (snake_alive[i])
        {
            total += snake_sizes[i];
        }
    }

    return total;
}

void activate_first_alive_snake(void)
{
    int i;

    for (i = 0; i < snake_count; i++)
    {
        if (snake_alive[i])
        {
            active_snake_idx = i;
            return;
        }
    }
}

// Verifica se o movimento é possível (não sai da arena)
int is_move_possible(int idx, int move)
{
    if (move == LEFT)
    {
        return idx % WIDTH > 1;
    }

    else if (move == RIGHT)
    {
        return idx % WIDTH < WIDTH - 2;
    }

    else if (move == UP)
    {
        return idx > 2 * WIDTH - 1;
    }

    else if (move == DOWN)
    {
        return idx < FIELD_SIZE - 2 * WIDTH;
    }

    return 0;
}

// Redefine o tabuleiro com base na posição da cobra e do alimento
void board_reset(
    int *psnake,
    int psize,
    int *pboard
)
{
    int i;
    int j;

    for (i = 0; i < FIELD_SIZE; i++)
    {
        if (is_cell_free(i, psize, psnake))
        {
            pboard[i] = UNDEFINED;
        }

        else
        {
            pboard[i] = SNAKE;
        }
    }

    pboard[food] = FOOD;

    for (j = 0; j < snake_count; j++)
    {
        if (!snake_alive[j] || j == active_snake_idx)
        {
            continue;
        }

        for (i = 0; i < snake_sizes[j]; i++)
        {
            pboard[snakes[j][i]] = SNAKE;
        }
    }
}

// Atualiza os dados do tabuleiro com base na posição da cobra e do alimento, preenchendo as distâncias
int board_refresh(
    int pfood,
    int *psnake,
    int *pboard
)
{
    // Uma célula pode ser enfileirada por cada um dos 4 vizinhos antes de ser processada.
    static int queue[4 * FIELD_SIZE];
    static int inqueue[FIELD_SIZE];

    int queue_start = 0;
    int queue_end = 0;

    int found = 0;

    int i;
    int idx;
    int next_idx;

    for (i = 0; i < FIELD_SIZE; i++)
    {
        inqueue[i] = 0;
    }

    queue[queue_end++] = pfood;

    while (queue_start < queue_end)
    {
        idx = queue[queue_start++];

        if (inqueue[idx] == 1)
        {
            continue;
        }

        inqueue[idx] = 1;

        for (i = 0; i < 4; i++)
        {
            if (is_move_possible(idx, mov[i]))
            {
                next_idx = idx + mov[i];

                if (next_idx == psnake[HEAD])
                {
                    found = 1;
                }

                if (pboard[next_idx] < SNAKE)
                {
                    if (pboard[next_idx] > pboard[idx] + 1)
                    {
                        pboard[next_idx] =
                            pboard[idx] + 1;
                    }

                    if (
                        inqueue[next_idx] == 0 &&
                        queue_end < (int)(sizeof(queue) / sizeof(queue[0]))
                    )
                    {
                        queue[queue_end++] = next_idx;
                    }
                }
            }
        }
    }

    return found;
}

// Escolhe o movimento mais curto (melhor), mas seguro
int choose_shortest_safe_move(
    int *psnake,
    int *pboard
)
{
    int best_move_local = ERR;
    int minimum_distance = SNAKE;

    int i;
    int next_idx;

    for (i = 0; i < 4; i++)
    {
        if (is_move_possible(psnake[HEAD], mov[i]))
        {
            next_idx =
                psnake[HEAD] + mov[i];

            if (pboard[next_idx] < minimum_distance)
            {
                minimum_distance =
                    pboard[next_idx];

                best_move_local =
                    mov[i];
            }
        }
    }

    return best_move_local;
}

// Escolhe o movimento mais longo (pior), mas seguro
int choose_longest_safe_move(
    int *psnake,
    int *pboard
)
{
    int best_move_local = ERR;
    int maximum_distance = -1;

    int i;
    int next_idx;

    for (i = 0; i < 4; i++)
    {
        if (is_move_possible(psnake[HEAD], mov[i]))
        {
            next_idx =
                psnake[HEAD] + mov[i];

            if (
                pboard[next_idx] < UNDEFINED &&
                pboard[next_idx] > maximum_distance
            )
            {
                maximum_distance =
                    pboard[next_idx];

                best_move_local =
                    mov[i];
            }
        }
    }

    return best_move_local;
}

// Verifica se há algum caminho até a cauda
int is_tail_inside(void)
{
    int result;

    tmpboard[
        tmpsnake[tmpsnake_size - 1]
    ] = FOOD;

    tmpboard[food] = SNAKE;

    result = board_refresh(
        tmpsnake[tmpsnake_size - 1],
        tmpsnake,
        tmpboard
    );

    {
        int i;

        for (i = 0; i < 4; i++)
        {
            if (
                is_move_possible(
                    tmpsnake[HEAD],
                    mov[i]
                )
                &&
                tmpsnake[HEAD] + mov[i]
                    ==
                tmpsnake[
                    tmpsnake_size - 1
                ]
                &&
                tmpsnake_size > 3
            )
            {
                result = 0;
            }
        }
    }

    return result;
}

// Segue a própria cauda
int follow_tail(void)
{
    int i;

    tmpsnake_size = snake_size;

    for (i = 0; i < FIELD_SIZE + 1; i++)
    {
        tmpsnake[i] = snake[i];
    }

    board_reset(
        tmpsnake,
        tmpsnake_size,
        tmpboard
    );

    tmpboard[
        tmpsnake[tmpsnake_size - 1]
    ] = FOOD;

    tmpboard[food] = SNAKE;

    board_refresh(
        tmpsnake[tmpsnake_size - 1],
        tmpsnake,
        tmpboard
    );

    tmpboard[
        tmpsnake[tmpsnake_size - 1]
    ] = SNAKE;

    return choose_longest_safe_move(
        tmpsnake,
        tmpboard
    );
}

// Qualquer movimento possível, mesmo que não seja seguro
int any_possible_move(void)
{
    int best_move_local = ERR;
    int minimum_distance = SNAKE;

    int i;
    int next_idx;

    choose_target_food();

    board_reset(
        snake,
        snake_size,
        board
    );

    board_refresh(
        food,
        snake,
        board
    );

    for (i = 0; i < 4; i++)
    {
        if (
            is_move_possible(
                snake[HEAD],
                mov[i]
            )
        )
        {
            next_idx =
                snake[HEAD] + mov[i];

            if (
                board[next_idx]
                <
                minimum_distance
            )
            {
                minimum_distance =
                    board[next_idx];

                best_move_local =
                    mov[i];
            }
        }
    }

    return best_move_local;
}

// Desloca o array para a direita, abrindo espaço no início
void shift_array(
    int *arr,
    int size
)
{
    int i;

    for (i = size; i > 0; i--)
    {
        arr[i] = arr[i - 1];
    }
}

// Desenha o alimento no tabuleiro
void draw_food(int food_idx)
{
    textcolor(LIGHTRED);

    arena_gotoxy(
        foods[food_idx] % WIDTH,
        foods[food_idx] / WIDTH
    );

    putstr(CHAR_FOOD);

    textcolor(WHITE);
}

// Cria um novo alimento em uma posição aleatória, 
// garantindo que não esteja ocupada pela cobra
void new_food(int food_idx)
{
    int cell_free = 0;

    int w;
    int h;

    while (!cell_free)
    {
        w =
            1 +
            rand() %
            (WIDTH - 2);

        h =
            1 +
            rand() %
            (HEIGHT - 2);

        foods[food_idx] =
            h * WIDTH + w;

        cell_free =
            is_cell_free_all(foods[food_idx]) &&
            is_cell_free_food_except(foods[food_idx], food_idx) &&
            (!bullet_active || foods[food_idx] != bullet_pos);
    }

    // Somente a nova posição é desenhada
    draw_food(food_idx);
}

int create_random_snake(void)
{
    int i;
    int w;
    int h;
    int pos;
    int previous_active_snake_idx;
    int new_snake_idx;

    if (snake_count >= MAX_SNAKES)
    {
        return 0;
    }

    for (i = 0; i < FIELD_SIZE; i++)
    {
        w =
            1 +
            rand() %
            (WIDTH - 2);

        h =
            1 +
            rand() %
            (HEIGHT - 2);

        pos =
            h * WIDTH + w;

        if (
            is_cell_free_food(pos) &&
            (!bullet_active || pos != bullet_pos) &&
            is_cell_free_all(pos)
        )
        {
            previous_active_snake_idx = active_snake_idx;
            new_snake_idx = snake_count;

            snakes[new_snake_idx][HEAD] = pos;
            snake_sizes[new_snake_idx] = 1;
            snake_alive[new_snake_idx] = 1;
            last_moves[new_snake_idx] = ERR;
            active_snake_idx = new_snake_idx;
            snake_count++;
            food_count++;

            draw_head();
            new_food(food_count - 1);

            active_snake_idx = previous_active_snake_idx;

            return 1;
        }
    }

    return 0;
}

// Desenha a pontuação na parte inferior da arena
void draw_score(void)
{
    textcolor(RED);

    arena_gotoxy(WIDTH / 2 - 8, HEIGHT - 1);

    printf(
        " Pontuação\e[37;40m: \e[33;40m%d ",
        score
    );

    textcolor(WHITE);
}

// Escolhe o caractere do tiro de acordo com a direção
static const char *bullet_char(int dir)
{
    if (dir == UP)
    {
        return CHAR_BULLET_UP;
    }

    else if (dir == DOWN)
    {
        return CHAR_BULLET_DOWN;
    }

    else if (dir == LEFT)
    {
        return CHAR_BULLET_LEFT;
    }

    return CHAR_BULLET_RIGHT;
}

// Desenha o tiro na posição atual
static void draw_bullet(void)
{
    textcolor(BULLET_COLOR);

    arena_gotoxy(
        bullet_pos % WIDTH,
        bullet_pos / WIDTH
    );

    putstr(bullet_char(bullet_dir));

    textcolor(WHITE);
}

// Apaga o tiro da posição atual (célula sempre vazia nesse momento)
static void erase_bullet(void)
{
    arena_gotoxy(
        bullet_pos % WIDTH,
        bullet_pos / WIDTH
    );

    putstr(CHAR_EMPTY);
}

// Dispara um tiro a partir da cabeça, no sentido do último movimento da cobra.
// Importante: 
// - Cada tiro custa um ponto 
// - Encolhe a cauda em uma célula.
void fire_bullet(void)
{
    int old_tail;

    // Só pode atirar com mais de três células de corpo
    if (bullet_active || last_move == ERR || snake_size <= 3)
    {
        return;
    }

    bullet_active = 1;
    bullet_dir = last_move;
    bullet_pos = snake[HEAD];
    bullet_owner = active_snake_idx;

    audio_play(&audio_effectchannel, AUDIO_SHOOT, FALSE);

    old_tail = snake[snake_size - 1];
    snake_size--;

    arena_gotoxy(
        old_tail % WIDTH,
        old_tail / WIDTH
    );

    putstr(CHAR_EMPTY);

    score--;

    draw_score();
}

// Avança o tiro em uma célula, tratando colisões.
// @return: 1 se o tiro atingiu a própria cobra (fim de jogo), 0 caso contrário.
int update_bullet(void)
{
    int i;
    int food_idx;
    int next_pos;
    int previous_active_snake_idx;
    int was_drawn;

    if (!bullet_active)
    {
        return 0;
    }

    if (
        bullet_owner == ERR ||
        bullet_owner >= snake_count ||
        !snake_alive[bullet_owner]
    )
    {
        bullet_active = 0;
        bullet_owner = ERR;

        return 0;
    }

    previous_active_snake_idx = active_snake_idx;
    active_snake_idx = bullet_owner;

    // Nada foi desenhado ainda na primeira célula (é a cabeça da cobra)
    was_drawn = bullet_pos != snake[HEAD];

    if (!is_move_possible(bullet_pos, bullet_dir))
    {
        if (was_drawn)
        {
            erase_bullet();
        }

        bullet_active = 0;
        bullet_owner = ERR;
        active_snake_idx = previous_active_snake_idx;

        return 0;
    }

    next_pos = bullet_pos + bullet_dir;

    // Atinge a comida: mesmo efeito de capturá-la, mas com bônus de 3 pontos
    food_idx = food_at(next_pos);

    if (food_idx != ERR)
    {
        audio_play(&audio_effectchannel, AUDIO_SHOOT_FOOD, FALSE);

        if (was_drawn)
        {
            erase_bullet();
        }

        // Apaga a comida atingida antes de sortear a próxima
        arena_gotoxy(
            foods[food_idx] % WIDTH,
            foods[food_idx] / WIDTH
        );

        putstr(CHAR_EMPTY);

        // A cauda se duplica, assim como ocorre ao capturar o alimento normalmente
        snake[snake_size] = snake[snake_size - 1];
        snake_size++;

        score += 3;

        draw_score();

        bullet_active = 0;
        bullet_owner = ERR;

        if (total_snake_size() < (WIDTH - 2) * (HEIGHT - 2))
        {
            new_food(food_idx);
        }

        active_snake_idx = previous_active_snake_idx;

        return 0;
    }

    // Atinge uma cobra: ela morre; só é fim de jogo se não restar nenhuma.
    for (i = 0; i < snake_count; i++)
    {
        if (
            snake_alive[i] &&
            !is_cell_free(next_pos, snake_sizes[i], snakes[i])
        )
        {
            if (was_drawn)
            {
                erase_bullet();
            }

            bullet_active = 0;
            bullet_owner = ERR;
            active_snake_idx = i;
            kill_active_snake();
            active_snake_idx = previous_active_snake_idx;

            return active_snake_count() == 0;
        }
    }

    if (was_drawn)
    {
        erase_bullet();
    }

    bullet_pos = next_pos;

    draw_bullet();

    active_snake_idx = previous_active_snake_idx;

    return 0;
}

// Desenha a borda do tabuleiro (chamada uma única vez)
void draw_border(void)
{
    int x;
    int y;

    textcolor(WHITE);

    // Linha superior
    for (x = 0; x < WIDTH; x++)
    {
        arena_gotoxy(x, 0);
        putstr(CHAR_WALL_H);
    }

    // Linha inferior
    for (x = 0; x < WIDTH; x++)
    {
        arena_gotoxy(x, HEIGHT - 1);
        putstr(CHAR_WALL_H);
    }

    // Laterais
    for (y = 1; y < HEIGHT - 1; y++)
    {
        arena_gotoxy(0, y);
        putstr(CHAR_WALL_V);

        arena_gotoxy(WIDTH - 1, y);
        putstr(CHAR_WALL_V);
    }

    // Cantos
    arena_gotoxy(0, 0);
    putstr(CHAR_CORNER_TL);

    arena_gotoxy(WIDTH - 1, 0);
    putstr(CHAR_CORNER_TR);

    arena_gotoxy(0, HEIGHT - 1);
    putstr(CHAR_CORNER_BL);

    arena_gotoxy(WIDTH - 1, HEIGHT - 1);
    putstr(CHAR_CORNER_BR);

    textcolor(WHITE);
}

// Desenha a cabeça da cobra no tabuleiro
void draw_head(void)
{
    int p;

    p = snake[HEAD];

    cell_color[p] = next_body_color();

    textcolor(cell_color[p]);

    arena_gotoxy(
        p % WIDTH,
        p / WIDTH
    );

    putstr(CHAR_SNAKE_HEAD);

    textcolor(WHITE);
}

// Aplica o movimento e redesenha apenas o que mudou:
// 1. nova cabeça  2. antiga cauda  3. alimento  4. pontuação
void make_move(int pbest_move)
{
    int food_idx;
    int p;
    int old_head;
    int old_tail;

    // Guarda a antiga cauda antes de deslocar o array.
    old_tail =
        snake[snake_size - 1];

    old_head = snake[HEAD];

    // Desloca o corpo.
    shift_array(
        snake,
        snake_size
    );

    // Move cabeça.
    snake[HEAD] += pbest_move;

    // Nova posição da cabeça.
    p = snake[HEAD];

    // A antiga cabeça vira corpo (a célula é apagada adiante se virar cauda).
    textcolor(cell_color[old_head]);

    arena_gotoxy(
        old_head % WIDTH,
        old_head / WIDTH
    );

    putstr(CHAR_SNAKE_BODY);

    // Desenha SOMENTE a nova cabeça.
    cell_color[p] = next_body_color();

    textcolor(cell_color[p]);

    arena_gotoxy(
        p % WIDTH,
        p / WIDTH
    );

    putstr(CHAR_SNAKE_HEAD);

    textcolor(WHITE);

    // Verifica o alimento.
    food_idx = food_at(snake[HEAD]);

    if (food_idx != ERR)
    {
        audio_play(&audio_effectchannel, AUDIO_FOOD1, FALSE);

        board[
            snake[HEAD]
        ] = SNAKE;

        snake_size++;

        score++;

        // A pontuação só muda aqui.
        draw_score();

        // Cria novo alimento.
        if (total_snake_size() < (WIDTH - 2) * (HEIGHT - 2))
        {
            new_food(food_idx);
        }
    }

    else
    {
        // Marca nova cabeça.
        board[
            snake[HEAD]
        ] = SNAKE;

        // Libera antiga cauda.
        board[old_tail] = UNDEFINED;

        // Apaga SOMENTE a antiga cauda.
        arena_gotoxy(
            old_tail % WIDTH,
            old_tail / WIDTH
        );

        putstr(CHAR_EMPTY);
    }
}

// Simula o movimento da cobra até o alimento, 
// sem alterar o estado real do jogo
void virtual_shortest_move(void)
{
    int food_eaten = 0;

    int move;

    int i;

    tmpsnake_size = snake_size;

    // Copia cobra.
    for (i = 0; i < FIELD_SIZE + 1; i++)
    {
        tmpsnake[i] = snake[i];
    }

    // Copia tabuleiro.
    for (i = 0; i < FIELD_SIZE; i++)
    {
        tmpboard[i] = board[i];
    }

    // Reinicializa.
    board_reset(
        tmpsnake,
        tmpsnake_size,
        tmpboard
    );

    while (!food_eaten)
    {
        // Caminho até alimento.
        board_refresh(
            food,
            tmpsnake,
            tmpboard
        );

        // Menor caminho.
        move =
            choose_shortest_safe_move(
                tmpsnake,
                tmpboard
            );

        // Nenhum movimento.
        if (move == ERR)
        {
            return;
        }

        // Desloca corpo virtual.
        shift_array(
            tmpsnake,
            tmpsnake_size
        );

        // Move cabeça virtual.
        tmpsnake[HEAD] += move;

        // Verifica alimento.
        if (tmpsnake[HEAD] == food)
        {
            tmpsnake_size++;

            board_reset(
                tmpsnake,
                tmpsnake_size,
                tmpboard
            );

            tmpboard[food] = SNAKE;

            food_eaten = 1;
        }

        else
        {
            // Nova cabeça vira corpo.
            tmpboard[
                tmpsnake[HEAD]
            ] = SNAKE;

            // Libera antiga cauda.
            tmpboard[
                tmpsnake[tmpsnake_size]
            ] = UNDEFINED;
        }
    }
}

// Verifica se há algum caminho seguro até o alimento
int find_safe_way(void)
{
    int safe_move = ERR;

    // Simulação virtual.
    virtual_shortest_move();

    // Verifica caminho até a cauda.
    if (is_tail_inside())
    {
        return choose_shortest_safe_move(
            snake,
            board
        );
    }

    // Caso contrário, segue a cauda.
    safe_move = follow_tail();

    return safe_move;
}

// Lê a tecla pressionada no teclado e atualiza a variável global 'key'
void read_keyboard(void)
{
    int first;
    int second;

    if (!_kbhit())
    {
        return;
    }

    first = _getch();

    // Setas no Windows:
    // 0 ou 224 + código da tecla.
    if (first == 0 || first == 224)
    {
        second = _getch();
        key_is_arrow = 1;

        switch (second)
        {
            case KEY_LEFT_CODE:
                key = KEY_LEFT_CODE;
                break;

            case KEY_RIGHT_CODE:
                key = KEY_RIGHT_CODE;
                break;

            case KEY_UP_CODE:
                key = KEY_UP_CODE;
                break;

            case KEY_DOWN_CODE:
                key = KEY_DOWN_CODE;
                break;

            default:
                key = second;
                break;
        }
    }
    else
    {
        key = first;
        key_is_arrow = 0;
    }
}

// Inicializa o estado do jogo, incluindo 
// o tabuleiro, a cobra, o alimento, a direção inicial e a pontuação
void initialize_game(void)
{
    int i;
    int j;

    // Tabuleiro
    for (i = 0; i < FIELD_SIZE; i++)
    {
        board[i] = 0;
        tmpboard[i] = 0;
    }

    // Cobras
    for (j = 0; j < MAX_SNAKES; j++)
    {
        snake_sizes[j] = 0;
        snake_alive[j] = 0;
        last_moves[j] = ERR;

        for (i = 0; i < FIELD_SIZE + 1; i++)
        {
            snakes[j][i] = 0;
        }
    }

    for (i = 0; i < FIELD_SIZE + 1; i++)
    {
        tmpsnake[i] = 0;
    }

    snake_count = 1;
    active_snake_idx = 0;

    // Cabeça inicial
    snake[HEAD] =
        1 * WIDTH + 1;

    snake_size = 1;
    snake_alive[0] = 1;

    // Cobra temporária
    tmpsnake[HEAD] =
        1 * WIDTH + 1;

    tmpsnake_size = 1;

    // Alimento inicial
    food_count = 1;
    foods[0] =
        3 * WIDTH + 3;
    target_food = foods[0];

    // Direção inicial
    best_move = ERR;

    // Tecla inicial
    key = ERR;
    key_is_arrow = 0;

    // Pontuação
    score = 0;

    // Tiro
    last_moves[0] = ERR;
    bullet_active = 0;
    bullet_pos = ERR;
    bullet_dir = ERR;
    bullet_owner = ERR;

    // Faixa de cor inicial
    paint_step = 0;

    for (i = 0; i < FIELD_SIZE; i++)
    {
        cell_color[i] = WHITE;
    }
}

// Verifica se o movimento leva a cabeça para fora da arena ou sobre o corpo.
// A última célula é ignorada porque a cauda se desloca no mesmo instante.
static int is_move_fatal(int move)
{
    int i;
    int j;
    int next_idx;
    int limit;
    int eating;

    if (
        move == ERR ||
        !is_move_possible(snake[HEAD], move)
    )
    {
        return 1;
    }

    next_idx = snake[HEAD] + move;
    eating = food_at(next_idx) != ERR;

    for (i = 0; i < snake_count; i++)
    {
        if (!snake_alive[i])
        {
            continue;
        }

        limit = snake_sizes[i];

        if (i == active_snake_idx && !eating)
        {
            limit--;
        }

        for (j = 0; j < limit; j++)
        {
            if (next_idx == snakes[i][j])
            {
                return 1;
            }
        }
    }

    return 0;
}

void kill_active_snake(void)
{
    int i;
    int remaining_snakes;

    for (i = 0; i < snake_size; i++)
    {
        arena_gotoxy(
            snake[i] % WIDTH,
            snake[i] / WIDTH
        );

        putstr(CHAR_EMPTY);
    }

    snake_alive[active_snake_idx] = 0;
    remaining_snakes = active_snake_count();

    if (remaining_snakes > 0)
    {
        audio_play(&audio_effectchannel, AUDIO_EVILLAUGH, FALSE);
    }

    if (food_count > remaining_snakes)
    {
        food_count--;

        arena_gotoxy(
            foods[food_count] % WIDTH,
            foods[food_count] / WIDTH
        );

        putstr(CHAR_EMPTY);

        if (food_count > 0)
        {
            target_food = foods[0];
        }
    }

    if (bullet_owner == active_snake_idx)
    {
        bullet_active = 0;
        bullet_owner = ERR;
    }
}

// Converte uma seta pressionada no deslocamento correspondente.
static int move_from_key(int pressed_key)
{
    switch (pressed_key)
    {
        case KEY_LEFT_CODE:
            return LEFT;

        case KEY_RIGHT_CODE:
            return RIGHT;

        case KEY_UP_CODE:
            return UP;

        case KEY_DOWN_CODE:
            return DOWN;

        default:
            return ERR;
    }
}

// Impede o retorno instantâneo em sentido contrário.
static int is_reverse_move(int previous_move, int requested_move)
{
    return previous_move != ERR &&
           requested_move != ERR &&
           previous_move == -requested_move;
}

// Mostra o modo atual na última linha reservada para a arena.
static void draw_control_mode(int manual_mode, int paused)
{
    const char *mode = "[Setas] Mudar direção • [P] Pause                 ";

    if (paused)
    {
        mode = "[P] Pausado                                       ";
    }
    else if (manual_mode)
    {
        mode = "[Setas] Mudar direção • [P] Pause • [A] Automático";
    }

    textcolor(YELLOW);
    arena_gotoxy(0, HEIGHT + 1);
    printf("Modo: %-11s", mode);
    textcolor(WHITE);
}

// Exibe a contagem inicial no centro da arena antes do primeiro movimento.
static void draw_start_countdown(void)
{
    const unsigned char *art = ascii_54321;
    int value;

    for (value = 5; value > 0; value--)
    {
        int line;
        int countdown_color;

        switch (value)
        {
            case 5:
                countdown_color = RED;
                break;
            case 4:
            case 2:
                countdown_color = WHITE;
                break;
            case 3:
                countdown_color = YELLOW;
                break;
            default:
                countdown_color = BLUE;
                break;
        }

        textcolor(countdown_color);

        for (line = 0; line < 7; line++)
        {
            int line_width = 0;
            const unsigned char *line_start = art;
            unsigned int bytes_remaining =
                ascii_54321_length - (unsigned int)(art - ascii_54321);

            while ((unsigned int)line_width < bytes_remaining &&
                   art[line_width] != '\n')
            {
                line_width++;
            }

            arena_gotoxy(WIDTH / 2 - 8, HEIGHT / 2 - 3 + line);
            printf("%-16s", "");
            arena_gotoxy(WIDTH / 2 - line_width / 2, HEIGHT / 2 - 3 + line);
            fwrite(line_start, 1, (size_t) line_width, stdout);
            art += line_width + 1;
        }

        textcolor(WHITE);
        fflush(stdout);

        delay(1000);

        if (value > 1)
        {
            if (art[0] == '\n')
            {
                art++;
            }
        }
    }

    for (value = 0; value < 7; value++)
    {
        arena_gotoxy(WIDTH / 2 - 8, HEIGHT / 2 - 3 + value);
        printf("%-16s", "");
    }

    fflush(stdout);
}

// Mensagem final, dentro da área do problema
static void draw_game_over(const char *message)
{
    textcolor(LIGHTRED);

    arena_gotoxy(WIDTH / 4 - 1, HEIGHT - 1);

    printf(
        "%s Pontuação final\e[37;40m:\e[31;40m \e[33;40m%d ",
        message,
        score
    );

    textcolor(WHITE);
}

int cobraRun(void)
{
    // Número de células jogáveis (sem a borda)
    const int playable_cells =
        (WIDTH - 2) * (HEIGHT - 2);

    const char *message = " Fim de jogo!";
    int paused = 0;
    int manual_mode = 0;
    int manual_move = ERR;
    int requested_move;
    int snake_died = 1;
    int i;

    // A arena é desenhada na área reservada ao problema
    draw_problem_screen(COBRA_PROBLEM, "");

    hidecursor();

    srand(
        (unsigned int)time(NULL)
    );
    
    // Inicialização    
    initialize_game();

    // Inicializa tabuleiro lógico
    board_reset(
        snake,
        snake_size,
        board
    );

    // Borda
    draw_border();

    // Cabeça inicial
    draw_head();

    // Comida
    draw_food(0);

    // Pontuação
    draw_score();

    // Modo de controle inicial.
    draw_control_mode(manual_mode, paused);

    audio_stop(); audio_resume();
    audio_play(&audio_mainchannel, AUDIO_MAP1, TRUE);

    draw_start_countdown();

    // Loop principal do jogo
    while (key != KEY_ESC_CODE)
    {
        // Entrada do teclado
        key = ERR;
        key_is_arrow = 0;
        read_keyboard();

        if (key == KEY_ESC_CODE)
        {
            message = " Interrompido.";
            snake_died = 0;
            break;
        }

        if (!key_is_arrow && (key == 'p' || key == 'P'))
        {
            paused = !paused;
            draw_control_mode(manual_mode, paused);
        }
        else if (key == 'a' || key == 'A')
        {
            manual_mode = 0;
            manual_move = ERR;
            draw_control_mode(manual_mode, paused);
        }
        else if (!key_is_arrow && (key == 'c' || key == 'C'))
        {
            create_random_snake();
        }
        else if (key == KEY_SPACE_CODE)
        {
            activate_first_alive_snake();
            fire_bullet();
        }
        else
        {
            requested_move = key_is_arrow ? move_from_key(key) : ERR;

            if (requested_move != ERR)
            {
                activate_first_alive_snake();

                if (is_reverse_move(last_move, requested_move))
                {
                    continue;
                }

                manual_mode = 1;
                manual_move = requested_move;
                draw_control_mode(manual_mode, paused);
            }
        }

        if (paused)
        {
            delay(60);
            continue;
        }

        // Avança o tiro em curso (usa as cobras ainda não movidas neste tick),
        // encerra o jogo se ele eliminar a última cobra.
        if (update_bullet())
        {
            message = " Eita, levou chumbo!";
            break;
        }

        for (i = 0; i < snake_count; i++)
        {
            if (!snake_alive[i])
            {
                continue;
            }

            active_snake_idx = i;
            choose_target_food();

            // Atualiza o tabuleiro lógico
            board_reset(
                snake,
                snake_size,
                board
            );

            // Atualiza o tabuleiro lógico com distâncias
            best_move =
                manual_mode && i == 0 ? manual_move : ERR;

            if (best_move == ERR &&
                board_refresh(
                    food,
                    snake,
                    board
                ))
            {
                // Há caminho até o alimento: escolhe o melhor movimento
                best_move =
                    find_safe_way();
            }

            else if (best_move == ERR)
            {
                // Não há caminho direto: segue a cauda.
                best_move =
                    follow_tail();
            }

            // Sem movimento seguro: tenta qualquer movimento possível
            if (best_move == ERR)
            {
                best_move =
                    any_possible_move();
            }

            // Sem saída ou colisão: remove esta cobra.
            if (is_move_fatal(best_move))
            {
                kill_active_snake();

                if (active_snake_count() == 0)
                {
                    break;
                }

                continue;
            }

            // Executa movimento
            make_move(best_move);

            // Guarda a direção para orientar o próximo tiro
            last_move = best_move;
        }

        if (active_snake_count() == 0)
        {
            break;
        }

        // Arena completamente preenchida
        if (total_snake_size() >= playable_cells)
        {
            message = "Voce venceu!";
            snake_died = 0;
            break;
        }

        delay(60);

        fflush(stdout);
    }

    draw_game_over(message);

    if (snake_died)
    {
        // Fundo (dead) no canal principal e voz (evil laugh) no canal de efeitos, tocando ao mesmo tempo
        audio_play(&audio_mainchannel, AUDIO_DEAD, FALSE);
        delay(500);
        audio_play(&audio_effectchannel, AUDIO_EVILLAUGH, FALSE);
    }
    else
    {
        audio_stop();
    }

    showcursor();

    waitEsc();

    audio_terminate();

    return 0;
}