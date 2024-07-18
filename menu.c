#include "menu.h"
#include <stdio.h>

void draw_menu(void)
{
    printf("Console Snake Game.\n");
    printf("1. Start.\n");
    printf("2. Exit.\n");
    printf("Enter your choice: ");
}

void select_option(char *p_choice)
{
    int option;
    while(1)
    {
        option=getchar();
        fflush(stdin);
        if(option<'1'||option>'2')
            printf("Invalid choice! Choose again.\n");
        else
        {
            *p_choice=option;
            break;
        }
    }
}
