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


/* Screen laylout def */
#define SCREEN_HEIGHT 24
#define SCREEN_WIDTH 80


#define MAP_HEIGHT (SCREEN_HEIGHT/3 - 1)
#define MAP_WIDTH (SCREEN_WIDTH - 2)

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
#define COMBO_REFERNCE_KEY   'c'
#define ENEMY_CHECK_KEY      ';'
#define QUIT_KEY             'Q'
#define PLAYER_DATA_KEY      '@'

#define ACCEPT_KEY       KEY_ENTER
#define UP_ARROW_KEY     KEY_UP
#define DOWN_ARROW_KEY   KEY_DOWN
#define LEFT_ARROW_KEY   KEY_LEFT
#define RIGHT_ARROW_KEY  KEY_RIGHT

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

#undef  KEY_BACKSPACE
#define KEY_BACKSPACE 8
#undef  KEY_ENTER
#define KEY_ENTER     10
#define KEY_ESC       27


/* Struct Info */
#define NUMBER_OF_KATANA_TYPES  6
#define KATANA_FIRE       0
#define KATANA_ICE        1
#define KATANA_WIND       2
#define KATANA_STONE      3
#define KATANA_LIGHTNING  4
#define KATANA_POSION     5

#define NUMBER_OF_MOVEMENT_TYPES 7
#define MOVEMENT_STRIKE   0 /* Move to strongest enemy                        */
#define MOVEMENT_BERSERK  1 /* Move to the closest enemy                      */
#define MOVEMENT_RETREAT  2 /* Move away from the most enemies                */
#define MOVEMENT_RETURN   3 /* Attempt to move to terrain maching katana type */
#define MOVEMENT_DEFEND   4 /* Don't move                                     */
#define MOVEMENT_FALLEN   5 /* Attempt to move to closest fallen katana       */
#define MOVEMENT_RAND     6 /* Move in random direction                       */

#define NUMBER_OF_TERRAIN_TYPES 4
#define TERRAIN_GRASS  0 /* None */
#define TERRAIN_WATER  1 /* Ice & Lightning synergies */
#define TERRAIN_ROCK   2 /* Stone & Wind synergies    */
#define TERRAIN_MAGMA  3 /* Fire & Posion synergies   */

#define NUMBER_OF_ENEMY_TYPES 4
#define ENEMY_BASIC   0 /* Not resistant              */
#define ENEMY_ENERGY  1 /* Fire & Lightning resistant */
#define ENEMY_STRONG  2 /* Stone & Wind resistant     */
#define ENEMY_FLUID   3 /* Ice & Posion resistant     */


/* Game Balance */
#define PLAYER_START_HEALTH               100

#define ENEMY_RESISTANCE_PERCENT          0.7
#define TERRAIN_SYNERGY_PERCENT           1.3

#define MAX_NUMBER_OF_ENEMIES             50
#define ENEMY_LEVEL_CAP                   50

#define MAX_NUMBER_OF_COMBOS              10

#define MIN_KATANA_SHARPNESS              25
#define CHANCE_FOR_SHARPNESS_DECREASE     10

#define MAX_NUMBER_OF_FALLEN_KATANAS      10
#define CHANCE_FOR_FALLEN_KATANA          50

#define CHANCE_FOR_HEALTH_INCREASE        3
#define MAX_HEALTH_INCREASE               5

#define TURNS_UNTIL_DIFFICULTY_INCREASE   200
#define TURNS_UNTIL_KATANA_POWER_INCREAS  120

#define ENEMY_SPAWN_NUMBER_BASE           (myRand(5) + 5)
#define ENEMY_GEN_PER_DIFFICULTY_INCREASE 3

#define ENEMY_SPEED_GEN                   (abs(dice(3, 4) - 6))
#define ENEMY_LEVEL_GEN                   (dice(((int) currentGameData.turn/TURNS_UNTIL_DIFFICULTY_INCREASE) + 1, 5))
#define ENEMY_MAX_HEALTH_GEN              ((myRand(currentEnemy->level) + 1) * 5)
#define ENEMY_DAMAGE_GEN                  (currentEnemy->level / 2)
           
#define KATANA_MAX_DAMAGE 12
//#define KATANA_DAMAGE_GEN                 (dice(1 + ((int) (currentGameData.turn/TURNS_UNTIL_KATANA_POWER_INCREAS)), 4) + 1)
#define KATANA_DAMAGE_MOD_GEN             (fmax(dice(4,3) - 6, 0) + ((int) (currentGameData.turn/TURNS_UNTIL_KATANA_POWER_INCREAS)))



/* Misc */
#define HISTORY_LENGTH 50
#define ERROR(x) printError(#x,__FILE__, __LINE__)



/* Bit Masks */
#define CHECK_BIT(var, mask) ((mask & var) == mask)


#define COMBO_FLAG_DOUBLE_ATTACK    0b0000000000000001
#define COMBO_FLAG_FORCE_ATTACK     0b0000000000000010
#define COMBO_FLAG_SCARE_ENEMIES    0b0000000000000100


#define WAVE_FLAG_ALL_AT_ONCE       0b0000000000000001
#define WAVE_FLAG_ONE_AT_A_TIME     0b0000000000000010
#define WAVE_FLAG_TWO_AT_A_TIME     0b0000000000000100
#define WAVE_FLAG_RAND_AT_A_TIME    0b0000000000001000

#define WAVE_FLAG_ONLY_ONE_TYPE     0b0000000000010000
#define WAVE_FLAG_ONLY_TWO_TYPES    0b0000000000100000
#define WAVE_FLAG_ONLY_THREE_TYPES  0b0000000001000000
#define WAVE_FLAG_ALL_TYPES         0b0000000010000000

#define WAVE_FLAG_2_TURN_DELAY      0b0000000100000000
#define WAVE_FLAG_5_TURN_DELAY      0b0000001000000000
#define WAVE_FLAG_7_TURN_DELAY      0b0000010000000000
#define WAVE_FLAG_10_TURN_DELAY     0b0000100000000000
 
#define WAVE_FLAG_ONE_SIDE_SPAWN    0b0001000000000000
#define WAVE_FLAG_TWO_SIDES_SPAWN   0b0010000000000000
#define WAVE_FLAG_THREE_SIDES_SPAWN 0b0100000000000000
#define WAVE_FLAG_FOUR_SIDES_SPAWN  0b1000000000000000

/* Structs */
struct Vec2 {
    int y;
    int x;
};

struct Katana {
    char name[20];
    int type;

    /* 1 - 12 */
    int damage;
    /* 0 - 6 */
    int damageMod;

    int movementType;

    /* 100 - 0 */
    int sharpness;


    int numberOfMoves;
    int numberOfStrikes;
    int numberOfKills;

    char katanaImage[3][MAP_WIDTH];

};

struct Player {
    char name[20];
    int health;

    int turnOfLastCombo;

    struct Vec2 location;

    struct Katana katanas[4];

    int enemiesKilled;
    int attacks;
    int moves;
    int katanasPickedUp;
    int katanasBroken;

    char DNA[MAP_HEIGHT][MAP_WIDTH + 1];
};

struct Enemy {
    int type;
    int maxHeath;
    int health;
    int damage;
    int speed; /* 1:normal - 6:slow */

    int level;



    int lastMovementTurn;
    struct Vec2 location;


};

struct Tile {
    int type;
    bool hasFallenKatana;
    struct Katana fallenKatana;
};

struct ProtoCombo {
    int action;
    int length;
    char title[12];
    char description[50];
};

struct Combo {
    int action;

    int length;
    int combo[10];

    int infoIndex;

    bool known;
};

struct Wave {
    int difficulty;

    unsigned short flags;

    int enemiesToSpawn[4];
};

struct GameData {
    struct Player player;
    int score;

    int currentNumberOfEnemies;
    struct Enemy enemies[MAX_NUMBER_OF_ENEMIES];


    struct Tile map[MAP_HEIGHT][MAP_WIDTH];

    int currentNumberOfFallenKatanas;

    int turn;

    /* [0]:type, [1]:location, [2]:movement */
    int previousMoves[HISTORY_LENGTH][3];

    int currentEnemyToAttack;

    int numberOfCombos;
    struct Combo combos[MAX_NUMBER_OF_COMBOS];

    int currentNumberOfDiscoveredCombos;
    int discoveredCombos[MAX_NUMBER_OF_COMBOS];

    int turnsToFreeze;

    unsigned short comboFlags;

    struct Wave currentWave;
    int currentNumberOfWaves;
    int turnOfLastEnemySpawn;

    int saveCheck;
};

/* Global Definitions */
struct GameData currentGameData;

#define NUMBER_OF_TITLE_CARD_LINES 8
const char titleCard[NUMBER_OF_TITLE_CARD_LINES][100] = {
    "                                              ",
    "     _   __  ___ _____ ___   _   _   ___      ",
    "    | | / / / _ \\_   _/ _ \\ | \\ | | / _ \\     ",
    "    | |/ / / /_\\ \\| |/ /_\\ \\|  \\| |/ /_\\ \\    ",
    "    |    \\ |  _  || ||  _  ||     ||  _  |    ",
    "    | |\\  \\| | | || || | | || |\\  || | | |    ",
    "    \\_| \\_/\\_| |_/\\_/\\_| |_/\\_| \\_/\\_| |_/    ",
    "                                              "
};

#define NUMBER_OF_MENU_OPTIONS 4
char menuOptions[NUMBER_OF_MENU_OPTIONS][50] = {
    "New Game   ",
    "Load Game  ",
    "Leaderboard",
    "Quit       "
};

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
    "Fallen",
    "Random"
};

const char katanaType[NUMBER_OF_KATANA_TYPES][10] = {
    "Fire",
    "Ice",
    "Wind",
    "Stone",
    "Lightning",
    "Posion"
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

#define NUMBER_OF_PROTO_COMBOS 9
struct ProtoCombo protoCombos[NUMBER_OF_PROTO_COMBOS] = {
    {0, 5, "Discover", "Discover a combo"},
    {1, 3, "Heal", "Gives a small health boost"},
    {2, 2, "Terriform", "Randomly changes the terrain under your feet"},
    {3, 4, "Freeze", "Freezes all enemies for 6 turns"},
    {4, 3, "Swap", "Randomly swaps the location of 2 katanas"},
    {5, 3, "Boost", "Double the damage of your next attack"},
    {6, 4, "Teleport", "Teleport to a random location"},
    {7, 3, "Force", "Force your next attack to ignore enemy resistance"},
    {8, 4, "Scare", "Scare enemies into taking a step back"},
};

/*
const char comboLocationToText[4][4] = {
    "TL-",
    "TR-",
    "BL-",
    "BR-"
};*/

const char comboLocationToText[4] = {
    TOP_LEFT_KATANA,
    TOP_RIGHT_KATANA,
    BOTTOM_LEFT_KATANA,
    BOTTOM_RIGHT_KATANA
};

/*==== Forward Function Decleration =======================================================================*/
void main();                                                         /* Main function                      */
void mainMenu();                                                     /* Handels main menu screen           */
void gameLoop();                                                     /* Game loop which runs every turn    */
/*---- Find Functions -------------------------------------------------------------------------------------*/
double findDistance(struct Vec2 start, struct Vec2 end, bool pythagoras); /* Distance between two points   */
int findClosestEnemy();                                              /* Return index of the closest enemy  */
double findDistanceToClosestEnemy(struct Vec2 location, bool pythagoras); /* Distance to closest enemy     */
struct Vec2 pathFinding(struct Vec2 start, struct Vec2 end, bool inverse); /* Next step from start to end  */
bool findNearbyEnemies(struct Vec2 loc, int (*enemies)[8], int *number); /* Find enemies adjacent to player*/
/*---- Movement Functions ---------------------------------------------------------------------------------*/
void playerMove(struct Katana katana);                               /* Moves the player                   */
void enemyMovement(int enemyIndex);                                  /* Moves an enemy                     */
/*---- Attacking Functions --------------------------------------------------------------------------------*/
void attackEnemy(int enemyIndex, struct Katana* katana);             /* Attack an enemy                    */
void attackPlayer(int enemyIndex);                                   /* Attack the player                  */
/*---- Generator & Destructor Functions -------------------------------------------------------------------*/
void genEnemy(int type, int sideLocation);                           /* Generate a random enemy            */
void removeEnemy(int enemyIndex);                                    /* Removes an enemy                   */
void genKatana(struct Katana *katana, int setMovementType);          /* Generate a random Katana           */
void genFallenKatana();                                              /* Generate a random fallen Katana    */
void genMap();                                                       /* Generate random terrain for map    */
void terrainAreaMap(int terrain, struct Vec2 location, int radius);  /* Place circle of terrain on map     */
void genCombo(int protoCombo);                                       /* Generate a combo from a protoCombo */
void genWave(struct Wave* wave);                                     /* Generate random enemy wave         */
void genPlayerDNA();                                                 /* Generate random strings            */
/*---- Combo Functions ------------------------------------------------------------------------------------*/
int checkForCombo();                                                 /* Checks if combo has been executed  */
bool isViableCombo(struct Combo combo);                              /* Checks for combo conflict          */
void activateCombo(int comboIndex);                                  /* Activate a combo                   */
/*---- Misc Functions -------------------------------------------------------------------------------------*/
int getStringInput(int y, int x, bool center, char* buffer);         /* Get string from player             */
bool doesEnemyMoveThisTurn(int enemy);                               /* Do speed calculations of an enemy  */
bool checkForEnemy(struct Vec2 location);                            /* Check if an enemy is at location   */
void replaceKatana(int slot, struct Katana katana);                  /* Replace katana with a new one      */
void pickUpFallenKatana();                                           /* Pick up a fallen katana            */
void enemyWave();                                                    /* Manage the current wave            */
int menu(char options[][50], int numberOfOptions, int yOffset, int xOffset); /* Menu function              */
void saveGame(FILE* fileToSaveTo);                                   /* Save current game to file          */
int loadGame(FILE* fileToLoadFrom);                                  /* Load a game from a file            */
void saveScore();                                                    /* Save game score to "Scores" file   */
/*---- Utility Functions ----------------------------------------------------------------------------------*/
int myRand(int number);                                              /* Same as rand(), but works with 0   */
int dice(int number, int sides);                                     /* DnD style dice rolls               */
void printError(char* message, char* file, int line);                /* Error function used by ERROR()     */
void pushPreviousMove(int type, int location, int movement);         /* Push values to previousMoves       */
void formatBlock(char* oldString, char* newString, int lineLength);  /* Format string for several lines    */
void sliceIncertString(char* expression, char* incert, int location, int replacmentLen); /* Insert string  */
FILE* writeFile(char* filePath);                                     /* Create writable file stream        */
FILE* readFile(char* filePath);                                      /* Open readable file stream          */
void genRandomName(char name[20], bool isSurname, bool gender);      /* Gen a random name                  */
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
void printKatanaDescription(struct Katana katana, bool fallenKatana); /* Print more in-depth katana info   */
void printPlayerData();                                              /* Print out exact info on the player */
void printMap();                                                     /* Print map at top of screen         */
void printHelp();                                                    /* Print out the help card            */
void printComboReference();                                          /* Print out known combos             */
void printEnemyInfo();                                               /* Info of the closest enemy          */
void printEntities();                                                /* Print player and enemies           */
void printTilecard();                                                /* Print titlecard for main menu      */
void printScoresFromScoresFile();                                    /* Print ordered list of scores       */
void printDeathScreen();                                             /* Print death screen showing stats   */
/*---- End of File ----------------------------------------------------------------------------------------*/