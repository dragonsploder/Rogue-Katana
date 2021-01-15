/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/*  File    :   katana.h                                                                                   */
/*  Author  :   Joshua Bourgeot                                                                            */
/*  Date    :   14th January 2021                                                                          */
/*  Function:   Declarations and function prototypes for Katana                                            */
/*                                                                                                         */
/*---------------------------------------------------------------------------------------------------------*/



#include <stdlib.h>
#include <time.h>


#define ERROR(x) printError(#x,__FILE__, __LINE__)

/* Screen laylout def */
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24


// Uncomment if using VS code cmd terminal
/*
#if defined(_WIN32)
    #undef  KEY_DOWN
    #define KEY_DOWN      456
    #undef  KEY_UP
    #define KEY_UP        450
    #undef  KEY_LEFT
    #define KEY_LEFT      452
    #undef  KEY_RIGHT
    #define KEY_RIGHT     454
#endif
*/

#undef KEY_ENTER
#define KEY_ENTER 10

#define NUMBER_OF_KATANA_TYPES 4

#define KATANA_FIRE         1
#define KATANA_ICE          2
#define KATANA_LIGHTNING    3
#define KATANA_POSION       4

#define NUMBER_OF_MOVEMENT_TYPES 4

#define MOVEMENT_ATTACK     1
#define MOVEMENT_RETREAT    2
#define MOVEMENT_WAIT       3
#define MOVEMENT_RAND       4



struct Vec2 {
    int x;
    int y;
};

struct Katana {
    char name[10];
    int katanaType;
    int damageAmount;

    int movementType;

    /* 1 - 100 */
    int hitChance;
};

struct Player {
    char name[50];
    int health;

    struct Vec2 location;

    struct Katana katanas[4];
};

struct Enemy {
    int health;
    int power;

    struct Vec2 location;

    char icon;

};

struct Tile {
    int type;
    char icon;
};


const char katanaCornerIcon[NUMBER_OF_KATANA_TYPES][2] = {
    "*",
    "+",
    "^",
    "~"
};

const char katanaMovementTypeNames[NUMBER_OF_MOVEMENT_TYPES][10] = {
    "Attack",
    "Defend",
    "Wait",
    "Random"
};


/*==== Forward Function Decleration =======================================================================*/
int main         (void);                                             /* Main function                      */
/*---- Generator Functions --------------------------------------------------------------------------------*/
void genKatana(struct Katana *katana);                               /* Generate a random Katana           */
/*---- Utility Functions ----------------------------------------------------------------------------------*/
int dice(int number, int sides);                                     /* DnD style dice rolls               */
void printError(char* message, char* file, int line);                /* Error function used by ERROR()     */
/*---- Miscellaneous Functions ----------------------------------------------------------------------------*/
/*---- Curses IO Functions --------------------------------------------------------------------------------*/
void initCurses();                                                   /* Initalise Curses library           */
void initColor();                                                    /* Initalise color for Curses         */
void stopCurses();                                                   /* Stop Curses library                */
wchar_t myGetch();                                                   /* Get keystroke                      */
void clearScreen();                                                  /* Clear screen                       */ 
void printHorizontalLine(int y, int start, int stop, char* toPrint); /* Print horizontal line              */ 
void printVerticalLine(int x, int start, int stop, char* toPrint);   /* Print vertical line                */ 
void printBox(int y, int x, int stopY, int stopX, char* toPrint);    /* Print a filled in box              */ 
void printBoarder();                                                 /* Print boarder around screen        */
void printKatana(struct Katana katana, int position);                /* Print katana info                  */
/*---- End of File ----------------------------------------------------------------------------------------*/