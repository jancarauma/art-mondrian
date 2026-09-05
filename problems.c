#include <stdio.h>
#include "utils.h"
#include "graphics.h"
#include "problems.h"
#include "problems/problem_0.h"
#include "problems/problem_1.h"
#include "problems/problem_2.h"
//#include "problems/problem_3.h"
#include "problems/problem_4.h"
#include "eastereggs/cobra/cobra.h"

void run_selected_problem(int selected)
{
    switch (selected)
    {
        case 0:
            problem_0();
            break;
        case 1:
            problem_1();
            break;
        case 2:
            problem_2();
            break;
        //case 3:
        //    problem_3();
        //    break;
        case 4:
            problem_4();
            break;
        case 14:
            cobraRun();
            break;
        default:
            draw_content_placeholder(selected);
            waitEsc();
            break;
    }
}