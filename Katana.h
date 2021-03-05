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

/* Controls */
#define TOP_LEFT_KATANA      'u'
#define TOP_RIGHT_KATANA     'i'
#define BOTTOM_LEFT_KATANA   'j'
#define BOTTOM_RIGHT_KATANA  'k'

#define TOP_LEFT_KATANA_SECONDARY      'U'
#define TOP_RIGHT_KATANA_SECONDARY     'I'
#define BOTTOM_LEFT_KATANA_SECONDARY   'J'
#define BOTTOM_RIGHT_KATANA_SECONDARY  'K'

#define BREAK_KATANA_KEY     'b'
#define HELP_KEY             'h'

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
#define KEY_ENTER  10

#define PLAYER_START_HEALTH  100

#define HISTORY_LENGTH 50


#define NUMBER_OF_KATANA_TYPES  6

#define KATANA_FIRE       0
#define KATANA_ICE        1
#define KATANA_WIND       2
#define KATANA_STONE      3
#define KATANA_LIGHTNING  4
#define KATANA_POSION     5

#define NUMBER_OF_MOVEMENT_TYPES 6

#define MOVEMENT_STRIKE   0 /* Move to strongest enemy                        */
#define MOVEMENT_BERSERK  1 /* Move to the closest enemy                      */
#define MOVEMENT_RETREAT  2 /* Move away from the most enemies                */
#define MOVEMENT_RETURN   3 /* Attempt to move to terrain maching katana type */
#define MOVEMENT_DEFEND   4 /* Don't move                                     */
#define MOVEMENT_RAND     5 /* Move in random direction                       */


#define NUMBER_OF_TERRAIN_TYPES 4

#define TERRAIN_GRASS  0 /* None */
#define TERRAIN_WATER  1 /* Ice & Lightning synergies */
#define TERRAIN_ROCK   2 /* Stone & Wind synergies    */
#define TERRAIN_MAGMA  3 /* Fire & Posion synergies   */

#define TERRAIN_SYNERGY_PERCENT 1.3

#define NUMBER_OF_ENEMY_TYPES 4

#define ENEMY_BASIC   0
#define ENEMY_ENERGY  1 /* Fire & Lightning resistant */
#define ENEMY_STRONG  2 /* Stone & Wind resistant     */
#define ENEMY_FLUID   3 /* Ice & Posion resistant     */

#define ENEMY_RESISTANCE_PERCENT 0.7



#define KATANA_MAX_DAMAGE 12

#define MAX_NUMBER_OF_ENEMIES 20


struct Vec2 {
    int y;
    int x;
};

struct Katana {
    char name[30];
    int type;

    /* 1 - 12 */
    int damage;
    /* 0 - 6 */
    int damageMod;

    int movementType;

    char katanaImage[3][MAP_WIDTH];

};

struct Player {
    char name[50];
    int health;

    struct Vec2 location;

    struct Katana katanas[4];
};

struct Enemy {
    int type;
    int maxHeath;
    int health;
    int damage;
    int speed; /* 1:normal - 6:slow */

    int level; /* health * power */



    int lastMovementTurn;
    struct Vec2 location;


};

struct Tile {
    int type;
    bool hasFallenKatana;
    struct Katana fallenKatana;
};

struct GameData {
    struct Player player;

    int currentNumberOfEnemies;
    struct Enemy enemies[MAX_NUMBER_OF_ENEMIES];


    struct Tile map[MAP_HEIGHT][MAP_WIDTH];

    int turn;

    /* [0]:type, [1]:location */
    int previousMoves[HISTORY_LENGTH][2];

    int currentEnemyToAttack;
};

/* Global Definitions */
struct GameData currentGameData;

#define NUMBER_OF_QUICK_HELP_LINES 17
const char quickHelpText[NUMBER_OF_QUICK_HELP_LINES][100] = {
"   Katana Types (icon):    Terrain types (icon):    Enemy Types (icon):       ",
"       Fire        (*)         Grass        (.)        Basic       (B)        ",
"       Ice         (+)         Water        (~)        Energy      (E)        ",
"       Wind        (=)         Rock         (*)        Strong      (S)        ",
"       Stone       (o)         Magma        (^)        Fluid       (F)        ",
"       Lightning   (^)                                                        ",
"       Posion      (~)                                                        ",
"                                                                              ",
"    Fire       works better on  Magma  but worse against  Energy  enemies     ",
"    Ice        works better on  Water  but worse against  Fluid   enemies     ",
"    Wind       works better on  Rock   but worse against  Strong  enemies     ",
"    Stone      works better on  Rock   but worse against  Strong  enemies     ",
"    Lightning  works better on  Magma  but worse against  Energy  enemies     ",
"    Posion     works better on  Water  but worse against  Fluid   enemies     ",
"                                                                              ",
"                       Lowercase enemies are weaker                           ",
"                      (!) represents a fallen katana                          "
};

const int enemyColor[NUMBER_OF_ENEMY_TYPES] = {
    COLOR_GREEN,  /* Basic  */
    COLOR_CYAN,   /* Energy */
    COLOR_RED,    /* Strong */
    COLOR_MAGENTA /* Fluid  */
};


const char terrainIcon[NUMBER_OF_TERRAIN_TYPES][2] = {
    ".", /* Grass  */
    "~", /* Water */
    "*", /* Rock  */
    "^"  /* Magma */
};

const int terrainColor[NUMBER_OF_TERRAIN_TYPES] = {
    COLOR_GREEN,  /* Grass  */
    COLOR_BLUE,   /* Water */
    COLOR_YELLOW, /* Rock  */
    COLOR_RED     /* Magma */
};

const char enemyIcon[NUMBER_OF_ENEMY_TYPES * 2][2] = {
    "B", /* Basic  */
    "E", /* Energy */
    "S", /* Strong */
    "F", /* Fluid  */
    "b", 
    "e",
    "s",
    "f"
};


const char katanaCornerIcon[NUMBER_OF_KATANA_TYPES][2] = {
    "*", /* Fire      */
    "+", /* Ice       */
    "=", /* Wind      */
    "o", /* Stone     */
    "^", /* Lightning */
    "~"  /* Posion    */
};

const int katanaColor[NUMBER_OF_KATANA_TYPES] = {
    COLOR_RED,     /* Fire      */
    COLOR_BLUE,    /* Ice       */
    COLOR_MAGENTA, /* Wind      */
    COLOR_YELLOW,  /* Stone     */
    COLOR_CYAN,    /* Lightning */
    COLOR_GREEN    /* Posion    */
};

const int katanaToTerrain[NUMBER_OF_KATANA_TYPES] = {
    3, /* Fire -> Magma      */
    1, /* Ice -> Water       */
    2, /* Wind -> Rock       */
    2, /* Stone -> Rock      */
    3, /* Lightning -> Magma */
    1  /* Posion -> Water    */
};

const int katanaToEnemyResistance[NUMBER_OF_KATANA_TYPES] = {
    1, /* Fire -> Energy is resistant      */
    3, /* Ice -> Fluid is resistant        */
    2, /* Wind -> Strong is resistant      */
    2, /* Stone -> Strong is resistant     */
    1, /* Lightning -> Energy is resistant */
    3  /* Posion -> Fluid is resistant     */
};

const char katanaMovementTypeNames[NUMBER_OF_MOVEMENT_TYPES][10] = {
    "Strike",
    "Berserk",
    "Retreat",
    "Return",
    "Defend",
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
    "kaeru",   /* Frog    */
    "sakana",  /* Fish    */
    "usagi",   /* Rabbit  */
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

#define NUMBER_OF_KATANA_GUARD_TYPES 5
const char katanaGuardTypes[NUMBER_OF_KATANA_GUARD_TYPES][3][5] = {
    {" ^ ", "|||", " v "},
    {" o ", "| |", " o "},
    {" ~~", "|:|", " ~~"},
    {" + ", "=|=", " + "},
    {" : ", "[|]", " : "}
};

#define NUMBER_OF_KATANA_HILT_TYPES 10
const char katanaHiltTypes[NUMBER_OF_KATANA_HILT_TYPES][5] = {
    "<===",
    "o===",
    ">---",
    ">>>>",
    "<<<<",
    "o->>",
    "-~<<",
    "-==[",
    ">==]",
    "---O"
};

#define NUMBER_OF_KATANA_BLADE_BODY_TYPES 5
const char katanaBladeBodyTypes[NUMBER_OF_KATANA_BLADE_BODY_TYPES][18] = {
    "================",
    "]]]]]]]]]]]]]]]]",
    "################",
    "++++++++++++++++",
    "::::::::::::::::"
};

#define NUMBER_OF_KATANA_BLADE_TIP_TYPES 3
const char katanaBladeTipTypes[NUMBER_OF_KATANA_BLADE_TIP_TYPES][2] = {
    ">",
    "+",
    ":"
};

/*==== Forward Function Decleration =======================================================================*/
void main();                                                         /* Main function                      */
void gameLoop();                                                     /* Game loop which runs every turn    */
/*---- Find Functions -------------------------------------------------------------------------------------*/
double findDistance(struct Vec2 start, struct Vec2 end);             /* Distance between two points        */
double findDistanceToClosestEnemy(struct Vec2 location);             /* Function used in retreat movement  */
struct Vec2 pathFinding(struct Vec2 start, struct Vec2 end);         /* Find next step from start to end   */
bool findNearbyEnemies(int (*enemies)[8], int *number);              /* Find enemies adjacent to player    */
/*---- Movement Functions ---------------------------------------------------------------------------------*/
void playerMove(struct Katana katana);                               /* Moves the player                   */
void enemyMovemet(int enemyIndex);                                   /* Moves an enemy                     */
/*---- Attacking Functions --------------------------------------------------------------------------------*/
void attackEnemy(int enemyIndex, struct Katana katana);              /* Attack an enemy                    */
void attackPlayer(int enemyIndex);                                   /* Attack the player                  */
/*---- Generator & Destructor Functions -------------------------------------------------------------------*/
void genEnemy();                                                     /* Generate a random enemy            */
void removeEnemy(int enemyIndex);                                    /* Removes an enemy                   */
void genKatana(struct Katana *katana);                               /* Generate a random Katana           */
void genFallenKatana();                                              /* Generate a random fallen Katana    */
void genMap();                                                       /* Generate random terrain for map    */
void terrainAreaMap(int terrain, struct Vec2 location, int radius);  /* Place circle of terrain on map     */
/*---- Misc Functions -------------------------------------------------------------------------------------*/
bool checkForEnemy(struct Vec2 location);                            /* Check if an enemy is at location   */
void replaceKatana(int slot, struct Katana katana);                  /* Replace katana with a new one      */
void pickUpFallenKatana();                                           /* Pick up a fallen katana            */
/*---- Utility Functions ----------------------------------------------------------------------------------*/
int myRand(int number);                                              /* Same as rand(), but works with 0   */
int dice(int number, int sides);                                     /* DnD style dice rolls               */
void printError(char* message, char* file, int line);                /* Error function used by ERROR()     */
void pushPreviousMove(int type, int location);                       /* Push values to previousMoves       */
void formatBlock(char* oldString, char* newString, int lineLength);  /* Format string for several lines    */
void sliceIncertString(char* expression, char* incert, int location, int replacmentLen); /* Insert string  */
/*---- Curses IO Functions --------------------------------------------------------------------------------*/
void initCurses();                                                   /* Initalise Curses library           */
void initColor();                                                    /* Initalise color for Curses         */
void stopCurses();                                                   /* Stop Curses library                */
int myGetch();                                                       /* Get keystroke                      */
void clearScreen();                                                  /* Clear screen                       */ 
void printHorizontalLine(int y, int start, int stop, char* toPrint); /* Print horizontal line              */ 
void printVerticalLine(int x, int start, int stop, char* toPrint);   /* Print vertical line                */
void update(char* message, bool pause);                              /* Print message to user              */ 
void printBox(int y, int x, int stopY, int stopX, char* toPrint);    /* Print a filled in box              */ 
void printBoarder(bool printMiddle);                                 /* Print boarder around screen        */
void printKatana(struct Katana katana, int position);                /* Print katana info                  */
void printKatanaDescription(struct Katana katana);                   /* Print more in-depth katana info    */
void printMap();                                                     /* Print map at top of screen         */
void printEntities();                                                /* Print player and enemies           */
/*---- End of File ----------------------------------------------------------------------------------------*/