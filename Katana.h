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
#define SCREEN_HEIGHT 24
#define SCREEN_WIDTH 80


#define MAP_HEIGHT (SCREEN_HEIGHT/3 - 1)
#define MAP_WIDTH (SCREEN_WIDTH - 2)


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

#define NUMBER_OF_KATANA_TYPES 6

#define KATANA_FIRE      1
#define KATANA_ICE       2
#define KATANA_WIND      3
#define KATANA_STONE     4
#define KATANA_LIGHTNING 5
#define KATANA_POSION    6

#define NUMBER_OF_MOVEMENT_TYPES 4

#define MOVEMENT_ATTACK  1
#define MOVEMENT_RETREAT 2
#define MOVEMENT_WAIT    3
#define MOVEMENT_RAND    4


#define NUMBER_OF_TERRAIN_TYPES 4

#define TERRAIN_DIRT  0
#define TERRAIN_WATER 1
#define TERRAIN_ROCK  2
#define TERRAIN_MAGMA 3

#define NUMBER_OF_ENEMY_TYPES 4

#define ENEMY_BASIC  0
#define ENEMY_ENERGY  1 /* Fire & Lightning resistant */
#define ENEMY_STRONG  2 /* Stone & Wind resistant */
#define ENEMY_FLUID   3 /* Ice and Posion resistant */



#define KATANA_MAX_DAMAGE 12

#define MAX_NUMBER_OF_ENEMIES 20


struct Vec2 {
    int y;
    int x;
};

struct Katana {
    char name[30];
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
    int type;
    int health;
    int power;

    struct Vec2 location;


};

struct Tile {
    int type;
};

/* Global Definitions */

struct Player player;

int currentNumberOfEnemies;
struct Enemy enemies[MAX_NUMBER_OF_ENEMIES];


struct Tile map[MAP_HEIGHT][MAP_WIDTH];

const char terrainIcon[NUMBER_OF_TERRAIN_TYPES][2] = {
    ".", /* Dirt  */
    "~", /* Water */
    "*", /* Rock  */
    "^"  /* Magma */
};

const char enemyIcon[NUMBER_OF_ENEMY_TYPES][2] = {
    "B", /* Basic  */
    "E", /* Energy */
    "S", /* Strong */
    "F"  /* Fluid  */
};


const char katanaCornerIcon[NUMBER_OF_KATANA_TYPES][2] = {
    "*", /* Fire      */
    "+", /* Ice       */
    "=", /* Wind      */
    "o", /* Stone     */
    "^", /* Lightning */
    "~"  /* Posion    */
};

const char katanaMovementTypeNames[NUMBER_OF_MOVEMENT_TYPES][10] = {
    "Attack",
    "Defend",
    "Wait",
    "Random"
};

const char katanaNameType[NUMBER_OF_KATANA_TYPES][10] = {
    "Hi",      /* Fire      */
    "Kori",    /* Ice       */
    "Kaze",    /* Wind      */
    "Iwa",     /* Stone     */
    "Inazuma", /* Lightning */
    "Doku"     /* Posion    */
};

const char katanaNameDamage[KATANA_MAX_DAMAGE][10] = {
    "usagi",   /* Rabbit  */
    "kaeru",   /* Frog    */
    "sakana",  /* Fish    */
    "tori",    /* Bird    */
    "neko",    /* Cat     */
    "inu",     /* Dog     */
    "buta",    /* Hog     */
    "hebi",    /* Serpent */
    "uma",     /* Horse   */
    "unagi",   /* Eel     */
    "bura",    /* Bull    */
    "kitsune"  /* Fox     */
};

/*==== Forward Function Decleration =======================================================================*/
void main();                                                         /* Main function                      */
void gameLoop();                                                     /* Game loop which runs every turn    */
/*---- Generator Functions --------------------------------------------------------------------------------*/
void genEnemy();                                                     /* Generate a random enemy            */
void genKatana(struct Katana *katana);                               /* Generate a random Katana           */
void genMap();                                                       /* Generate random terrain for map    */
void terrainAreaMap(int terrain, struct Vec2 location, int radius);  /* Place circle of terrain on map     */
/*---- Misc Functions -------------------------------------------------------------------------------------*/
bool checkForEnemy(struct Vec2 location);                            /* Check if an enemy is at location   */
/*---- Utility Functions ----------------------------------------------------------------------------------*/
int dice(int number, int sides);                                     /* DnD style dice rolls               */
void printError(char* message, char* file, int line);                /* Error function used by ERROR()     */
/*---- Miscellaneous Functions ----------------------------------------------------------------------------*/
/*---- Curses IO Functions --------------------------------------------------------------------------------*/
void initCurses();                                                   /* Initalise Curses library           */
void initColor();                                                    /* Initalise color for Curses         */
void stopCurses();                                                   /* Stop Curses library                */
int  myGetch();                                                      /* Get keystroke                      */
void clearScreen();                                                  /* Clear screen                       */ 
void printHorizontalLine(int y, int start, int stop, char* toPrint); /* Print horizontal line              */ 
void printVerticalLine(int x, int start, int stop, char* toPrint);   /* Print vertical line                */ 
void printBox(int y, int x, int stopY, int stopX, char* toPrint);    /* Print a filled in box              */ 
void printBoarder();                                                 /* Print boarder around screen        */
void printKatana(struct Katana katana, int position);                /* Print katana info                  */
void printMap();                                                     /* Print map at top of screen         */
void printEntities();                                                /* Print player and enemies           */
/*---- End of File ----------------------------------------------------------------------------------------*/