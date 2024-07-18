#include "game.h"
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <time.h>
#include <conio.h>

typedef enum direction
{
    UP,
    RIGHT,
    DOWN,
    LEFT
} direction;

typedef struct Board
{
    int width;
    int height;
    char *grid;
} Board;

typedef struct snake_segment
{
    int x;
    int y;
    direction direction;
    struct snake_segment *next;
    struct snake_segment *previous;
} snake_segment;

typedef struct Snake
{
    snake_segment *Head;
    snake_segment *Tail;
} Snake;

typedef struct Fruit
{
    int x;
    int y;
} Fruit;

void move_cursor_home(void)
{
    HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
    COORD homeCoords={0,0};

    if(hConsole==INVALID_HANDLE_VALUE) return;

    //Move the cursor home.
    SetConsoleCursorPosition(hConsole,homeCoords);
}

void create_board(Board *p_Board)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
    (p_Board->width)=csbi.srWindow.Right-csbi.srWindow.Left+1;
    (p_Board->height)=csbi.srWindow.Bottom-csbi.srWindow.Top-1;
    (p_Board->grid)=malloc((p_Board->width)*(p_Board->height)*sizeof(char));
    if(!(p_Board->grid))
    {
        printf("Couldn't allocate memory for the board!");
        exit(1);
    }

    for(int i=0;i<(p_Board->height);i++)
    {
        for(int j=0;j<(p_Board->width);j++)
        {
            if((i==0)||(i==p_Board->height-1)||(j==0)||(j==p_Board->width-1))
            {
                (p_Board->grid)[(p_Board->width)*i+j]='#';
            }
            else
                (p_Board->grid)[(p_Board->width)*i+j]=' ';
        }
    }
}

void create_snake(Snake *p_Snake,Board *p_Board)
{
    snake_segment *temp=malloc(sizeof(snake_segment));
    if(!temp)
    {
        printf("Couldn't allocate memory for the snake segment!");
        exit(1);
    }
    (temp->x)=(p_Board->width)/2-1;
    (temp->y)=(p_Board->height)/2-1;
    (temp->direction)=RIGHT;
    (temp->next)=NULL;
    (temp->previous)=NULL;
    (p_Board->grid)[(p_Board->width)*(temp->y)+(temp->x)]='>';
    (p_Snake->Head)=temp;
    (p_Snake->Tail)=temp;
}

void create_fruit(Fruit *p_Fruit,Board *p_Board)
{
    do
    {
        (p_Fruit->x)=1+rand()%(p_Board->width-2);
        (p_Fruit->y)=1+rand()%(p_Board->height-2);
    }while((p_Board->grid)[(p_Board->width)*(p_Fruit->y)+(p_Fruit->x)]!=' ');
    (p_Board->grid)[(p_Board->width)*(p_Fruit->y)+(p_Fruit->x)]='%';
}

void create_board_print_buffer(Board *p_Board, char **buffer)
{
    //Create a buffer to hold the entire board.
    int buffer_size=(p_Board->width+1)*p_Board->height+1; //+1 for new line and +1 for null terminator.
    *buffer=malloc(buffer_size*sizeof(char));
    if (!buffer)
    {
        printf("Couldn't allocate memory for the draw buffer!");
        exit(1);
    }
}

void draw_board(Board *p_Board,char **buffer,int score)
{
    printf("Your score %u.\n",score);

    //Fill the buffer with the board representation.
    int index=0;
    for (int i=0;i<p_Board->height;i++)
    {
        for (int j=0;j<p_Board->width;j++)
        {
            (*buffer)[index++]=p_Board->grid[p_Board->width*i+j];
        }
        (*buffer)[index++]='\n';
    }
    (*buffer)[index]='\0'; //Null-terminate the string.

    //Print the buffer all at once.
    printf("%s",*buffer);
}

void move_snake(Snake *p_Snake, Board *p_Board)
{
    //Erase the tail from the grid.
    p_Board->grid[p_Board->width*p_Snake->Tail->y+p_Snake->Tail->x]=' ';
    snake_segment *temp=p_Snake->Tail;

    //Move the tail segment to the head.
    while (temp->previous)
    {
        temp->x=temp->previous->x;
        temp->y=temp->previous->y;
        temp->direction=temp->previous->direction;
        temp=temp->previous;
    }

    //Update the head's position.
    switch (p_Snake->Head->direction)
    {
    case UP:
        p_Snake->Head->y--;
        p_Board->grid[p_Board->width*p_Snake->Head->y+p_Snake->Head->x]='^';
        break;
    case DOWN:
        p_Snake->Head->y++;
        p_Board->grid[p_Board->width*p_Snake->Head->y+p_Snake->Head->x]='!';
        break;
    case RIGHT:
        p_Snake->Head->x++;
        p_Board->grid[p_Board->width*p_Snake->Head->y+p_Snake->Head->x]='>';
        break;
    case LEFT:
        p_Snake->Head->x--;
        p_Board->grid[p_Board->width*p_Snake->Head->y+p_Snake->Head->x]='<';
        break;
    }

    temp=p_Snake->Head->next;
    if(temp)
        p_Board->grid[p_Board->width*temp->y+temp->x]='#';
}

int collided(Snake *p_Snake,Board *p_Board)
{
    if((p_Snake->Head->x==0)||(p_Snake->Head->x==p_Board->width-1)||(p_Snake->Head->y==0)||(p_Snake->Head->y==p_Board->height-1))
        return 1;
    int x=(p_Snake->Head->x);
    int y=(p_Snake->Head->y);
    snake_segment *temp=(p_Snake->Head->next);
    while(temp)
    {
        if((temp->x==x)&&(temp->y==y))
            return 1;
        temp=(temp->next);
    }
    return 0;
}

int hit_fruit(Snake *p_Snake,Fruit *p_Fruit)
{
    if((p_Snake->Head->x==p_Fruit->x)&&(p_Snake->Head->y==p_Fruit->y))
        return 1;
    return 0;
}

void grow_snake(Snake *p_Snake,Board *p_Board)
{
    snake_segment *temp=malloc(sizeof(snake_segment));
    if(!temp)
    {
        printf("Couldn't allocate memory for the snake segment!");
        exit(1);
    }

    switch (p_Snake->Tail->direction)
    {
    case UP:
        (temp->x)=(p_Snake->Tail->x);
        (temp->y)=(p_Snake->Tail->y)+1;
        break;
    case DOWN:
        (temp->x)=(p_Snake->Tail->x);
        (temp->y)=(p_Snake->Tail->y)-1;
        break;
    case RIGHT:
        (temp->x)=(p_Snake->Tail->x)-1;
        (temp->y)=(p_Snake->Tail->y);
        break;
    case LEFT:
        (temp->x)=(p_Snake->Tail->x)+1;
        (temp->y)=(p_Snake->Tail->y);
        break;
    }

    (temp->direction)=(p_Snake->Tail->direction);
    (temp->previous)=(p_Snake->Tail);
    (temp->next)=NULL;
    (p_Snake->Tail->next)=temp;
    (p_Snake->Tail)=temp;
    (p_Board->grid)[(p_Board->width)*(temp->y)+(temp->x)]='#';
}

void draw_game_over_screen(int score)
{
    system("cls");
    printf("GAME OVER! Your score: %u\n\n",score);
    printf("Press ENTER to return to starting screen.");
    getchar();
    fflush(stdin);
}

void free_snake(Snake *p_Snake)
{
    snake_segment *temp1,*temp2;
    temp1=(p_Snake->Head);
    temp2=(p_Snake->Head->next);
    free(temp1);
    while(temp2)
    {
        temp1=temp2;
        temp2=temp2->next;
        free(temp1);
    }
}

void play_snake(void)
{
    Board Board;
    create_board(&Board);

    char *buffer;
    create_board_print_buffer(&Board,&buffer);

    Snake Snake;
    create_snake(&Snake,&Board);

    Fruit Fruit;
    srand(time(NULL));
    create_fruit(&Fruit,&Board);

    int score=0;

    //Gameloop
    char move=0;
    while(1)
    {
        move_snake(&Snake,&Board);
        if(hit_fruit(&Snake,&Fruit))
        {
            score++;
            grow_snake(&Snake,&Board);
            create_fruit(&Fruit,&Board);
        }
        if(collided(&Snake,&Board))
        {
            draw_game_over_screen(score);
            break;
        }

        const int snake_speed=15;//characters/second.
        draw_board(&Board,&buffer,score);
        move_cursor_home();
        Sleep(1000/snake_speed);

        if (_kbhit())
        {
            move=_getch();
            switch (move)
            {
            case 'w':
            case 'W':
                if(Snake.Head->direction==LEFT||Snake.Head->direction==RIGHT)
                    Snake.Head->direction=UP;
                break;
            case 's':
            case 'S':
                if(Snake.Head->direction==LEFT||Snake.Head->direction==RIGHT)
                    Snake.Head->direction=DOWN;
                break;
            case 'd':
            case 'D':
                if(Snake.Head->direction==UP||Snake.Head->direction==DOWN)
                    Snake.Head->direction=RIGHT;
                break;
            case 'a':
            case 'A':
                if(Snake.Head->direction==UP||Snake.Head->direction==DOWN)
                    Snake.Head->direction=LEFT;
                break;
            }
        }

    }

    free_snake(&Snake);

    free(Board.grid);

    free(buffer);
}
