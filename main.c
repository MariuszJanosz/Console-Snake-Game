#include "menu.h"
#include "game.h"

int main()
{
    char choice;
    do
    {
        draw_menu();
        select_option(&choice); //1=play, 2=exit.
        if(choice=='1')
        {
            play_snake();
        }
    }while(choice!='2');

    return 0;
}
