#include <stdio.h>
#include <stdbool.h>

#include "main.h"
#include "graphics.h"
#include "utils.h"
#include "problems.h"

 /*****************************************************************
 * FUNÇÃO PRINCIPAL
 *****************************************************************/

 int main()
 {
   // Variáveis auxiliares
   bool WE_ARE_ALIVE = true;

   // Estrutura para capturar o estado das teclas pressionadas
   KEY keypressed;
   
   int MenuIndex = 1;    // Índice do item do menu a ser exibido
   int MenuSelected = 1; // Índice do item selecionado no menu   

   // Configura a codificação do console para UTF-8
   init_console();

   // Oculta o cursor do console
   hidecursor();
       
   #ifdef RELEASE_VERSION   
   // Animação de boas-vindas
   draw_welcome();
   #endif

   // Menu principal
   draw_menu();
   // Imprime os itens do menu (versão inicial)
   draw_menu_items(1, 1);

   // Rotina principal
   while (WE_ARE_ALIVE)
   {
      keypressed = getKeypressed();

      // Pressionou setas direcionais
      if (keypressed.x == 27 && keypressed.y == 91)
      {
         switch (keypressed.z)
         {
            case 65: // Seta para cima
               MenuSelected--;
               break;
            case 66: // Seta para baixo
               MenuSelected++;
               break;
         }

         // Os itens do menu são selecionados ciclicamente
         if (MenuSelected < 1)
         {
            MenuSelected = MENU_MAIN_ITEMS;
            MenuIndex = MENU_MAIN_ITEMS - MENU_MAIN_VISIBLE + 1;
         }
         else if (MenuSelected > MENU_MAIN_ITEMS)
         {
            MenuSelected = 1;
            MenuIndex = 1;
         }
         else if (MenuSelected > (MenuIndex + MENU_MAIN_VISIBLE - 1))
         {
            MenuIndex++;         
         }
         else if (MenuSelected < MenuIndex)
         {
            MenuIndex--;
         }

         draw_menu_items(MenuIndex, MenuSelected);
      }
      
      // Usuário pressionou a tecla ENTER
      if (keypressed.x == 10)
      {
         run_selected_problem(MenuSelected);
         draw_menu();
         draw_menu_items(MenuIndex, MenuSelected);
      }

      // Usuário pressionou a tecla "Ctrl + X" (Sair)
      if (keypressed.x == 24)
      {
         WE_ARE_ALIVE = false;
      }
   }  
   
   // Imprime animação de finalização
   #ifdef RELEASE_VERSION
   draw_goodbye();
   #endif

   return (0);
 }