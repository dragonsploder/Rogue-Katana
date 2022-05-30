/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/*  File    :   katana.h                                                                                   */
/*  Author  :   Joshua Bourgeot                                                                            */
/*  Date    :   14th January 2021                                                                          */
/*  Function:   Declarations and function prototypes for Katana                                            */
/*                                                                                                         */
/*---- Include Files --------------------------------------------------------------------------------------*/
#include <stdlib.h>          /* General functions                                                          */
#include <time.h>            /* Functions for date and time                                                */
/*---------------------------------------------------------------------------------------------------------*/



/* Screen def */
#define SCREEN_HEIGHT 24
#define SCREEN_WIDTH  80

#define MAP_HEIGHT    (SCREEN_HEIGHT/3 - 1)
#define MAP_WIDTH     (SCREEN_WIDTH - 2)

#define USE_COLOR true

/* Controls */
#define TOP_LEFT_KATANA                'u'
#define TOP_RIGHT_KATANA               'i'
#define BOTTOM_LEFT_KATANA             'j'
#define BOTTOM_RIGHT_KATANA            'k'

#define TOP_LEFT_KATANA_SECONDARY      'U'
#define TOP_RIGHT_KATANA_SECONDARY     'I'
#define BOTTOM_LEFT_KATANA_SECONDARY   'J'
#define BOTTOM_RIGHT_KATANA_SECONDARY  'K'

#define BREAK_KATANA_KEY               'b'
#define HELP_KEY                       'h'
#define COMBO_REFERENCE_KEY             'c'
#define ENEMY_CHECK_KEY                ';'
#define QUIT_KEY                       'Q'
#define PLAYER_DATA_KEY                '@'

#define ACCEPT_KEY                     KEY_ENTER
#define UP_ARROW_KEY                   KEY_UP
#define DOWN_ARROW_KEY                 KEY_DOWN
#define LEFT_ARROW_KEY                 KEY_LEFT
#define RIGHT_ARROW_KEY                KEY_RIGHT

#undef  KEY_BACKSPACE                  
#define KEY_BACKSPACE                  8
#undef  KEY_ENTER                 
#define KEY_ENTER                      10
#define KEY_ESC                        27


/* Struct Info */
#define NUMBER_OF_KATANA_TYPES    6
#define KATANA_FIRE               0
#define KATANA_ICE                1
#define KATANA_WIND               2
#define KATANA_STONE              3
#define KATANA_LIGHTNING          4
#define KATANA_POISON             5
 
#define NUMBER_OF_MOVEMENT_TYPES  7
#define MOVEMENT_STRIKE           0 /* Move to strongest enemy                         */
#define MOVEMENT_BERSERK          1 /* Move to the closest enemy                       */
#define MOVEMENT_RETREAT          2 /* Move away from the most enemies                 */
#define MOVEMENT_RETURN           3 /* Attempt to move to terrain matching katana type */
#define MOVEMENT_DEFEND           4 /* Don't move                                      */
#define MOVEMENT_FALLEN           5 /* Attempt to move to closest fallen katana        */
#define MOVEMENT_RAND             6 /* Move in random direction                        */
 
#define NUMBER_OF_TERRAIN_TYPES   4
#define TERRAIN_GRASS             0 /* None */
#define TERRAIN_WATER             1 /* Ice & Lightning synergies */
#define TERRAIN_ROCK              2 /* Stone & Wind synergies    */
#define TERRAIN_MAGMA             3 /* Fire & Poison synergies   */
 
#define NUMBER_OF_ENEMY_TYPES     4
#define ENEMY_BASIC               0 /* Not resistant              */
#define ENEMY_ENERGY              1 /* Fire & Lightning resistant */
#define ENEMY_STRONG              2 /* Stone & Wind resistant     */
#define ENEMY_FLUID               3 /* Ice & Poison resistant     */


/* Game Balance */
#define PLAYER_START_HEALTH                100

#define ENEMY_RESISTANCE_PERCENT           0.7
#define TERRAIN_SYNERGY_PERCENT            1.3

//#define MAX_NUMBER_OF_ENEMIES              25
#define MAX_NUMBER_OF_ENEMIES              20
#define ENEMY_LEVEL_CAP                    20

#define MAX_NUMBER_OF_COMBOS               10

#define MIN_KATANA_SHARPNESS               25
#define CHANCE_FOR_SHARPNESS_DECREASE      10

#define MAX_NUMBER_OF_FALLEN_KATANAS       15
#define CHANCE_FOR_FALLEN_KATANA           100

#define CHANCE_FOR_HEALTH_INCREASE         3
#define MAX_HEALTH_INCREASE                5

#define TURNS_UNTIL_DIFFICULTY_INCREASE    70
#define TURNS_UNTIL_KATANA_POWER_INCREASE  120

#define ENEMY_SPAWN_NUMBER_BASE            (myRand(3) + 3)
#define ENEMY_GEN_PER_DIFFICULTY_INCREASE  2

#define ENEMY_SPEED_GEN                    (abs(dice(3, 4) - 6))
#define ENEMY_LEVEL_GEN                    (dice(((int) currentGameData.turn/TURNS_UNTIL_DIFFICULTY_INCREASE) + 1, 4) + 1)
//#define ENEMY_MAX_HEALTH_GEN               ((myRand(currentEnemy->level) + 1) * 2)
#define ENEMY_MAX_HEALTH_GEN               ((int)((myRand(currentEnemy->level) + 4) * 1.5))
//#define ENEMY_DAMAGE_GEN                   (currentEnemy->level / (myRand(3) + 1))
#define ENEMY_DAMAGE_GEN                   ((int)(currentEnemy->level / (myRand(3) + 2)))
           
#define KATANA_MAX_DAMAGE                  12
#define KATANA_DAMAGE_GEN                  (dice(3, 4))
#define KATANA_DAMAGE_MOD_GEN              (fmax(dice(4,3) - 6, 0) + ((int) (currentGameData.turn/TURNS_UNTIL_KATANA_POWER_INCREASE)))
#define MAX_KATANA_DAMAGE_MOD              12  


/* Misc */
#define HISTORY_LENGTH        50
#define ERROR(x)              printError(#x,__FILE__, __LINE__)
#define CHECK_BIT(var, mask)  ((mask & var) == mask)


/* Bit Masks */
#define COMBO_FLAG_DOUBLE_ATTACK     1
#define COMBO_FLAG_FORCE_ATTACK      2
#define COMBO_FLAG_SCARE_ENEMIES     4
 
#define WAVE_FLAG_ALL_AT_ONCE        1
#define WAVE_FLAG_ONE_AT_A_TIME      2
#define WAVE_FLAG_TWO_AT_A_TIME      4
#define WAVE_FLAG_RAND_AT_A_TIME     8
 
#define WAVE_FLAG_ONLY_ONE_TYPE      16
#define WAVE_FLAG_ONLY_TWO_TYPES     32
#define WAVE_FLAG_ONLY_THREE_TYPES   64
#define WAVE_FLAG_ALL_TYPES          128
 
#define WAVE_FLAG_2_TURN_DELAY       256
#define WAVE_FLAG_5_TURN_DELAY       512
#define WAVE_FLAG_7_TURN_DELAY       1024
#define WAVE_FLAG_10_TURN_DELAY      2048
  
#define WAVE_FLAG_ONE_SIDE_SPAWN     4096
#define WAVE_FLAG_TWO_SIDES_SPAWN    8192
#define WAVE_FLAG_THREE_SIDES_SPAWN  16384
#define WAVE_FLAG_FOUR_SIDES_SPAWN   32768

/* Structs */
struct Vec2 {
    int y;
    int x;
};

struct Katana {
    char name[20];                   /* Generated katana name (type + damage) */
    int type;                        /* Fire, Ice, etc                        */
    int damage;                      /* 1 - 12                                */
    int damageMod;                   /* 0 - 12                                */
    int movementType;                /* Strike, Berserk, etc                  */
    int sharpness;                   /* 100 - 25                              */
    int numberOfMoves;               /* Number movement uses with this katana */
    int numberOfStrikes;             /* Number of attacks with this katana    */
    int numberOfKills;               /* Number of kills with this katana      */
    char katanaImage[3][MAP_WIDTH];  /* Generated ASCII art of this katana    */
};

struct Player {
    char name[20];                        /* Player name                        */
    int health;                           /* Current health                     */
    int turnOfLastCombo;                  /* Last turn player used combo        */
    struct Vec2 location;                 /* Current player location            */
    struct Katana katanas[4];             /* Currently held katanas             */
    int enemiesKilled;                    /* Number of enemies killed by player */
    int attacks;                          /* Number of katana attacks           */
    int moves;                            /* Number of player moves (movement)  */
    int katanasPickedUp;                  /* Number of fallen katanas picked up */
    int katanasBroken;                    /* Number of fallen katanas broken    */
    char DNA[MAP_HEIGHT][MAP_WIDTH + 1];  /* Random ASCII "DNA" mess            */
};

struct Enemy {
    int type;              /* Basic, Energy, etc               */
    int level;             /* Level (used to gen other values) */
    int maxHeath;          /* Starting health                  */
    int health;            /* Current health                   */
    int damage;            /* Damage to player each attack     */
    int speed;             /* Speed (1:fast-6:slow)            */
    int lastMovementTurn;  /* Last turn enemy moved            */
    struct Vec2 location;  /* Current location                 */
};

/* Map tile */
struct Tile {
    int type;                    /* Grass, Water, etc                     */
    bool hasFallenKatana;        /* Is there a fallen katana on this tile */
    struct Katana fallenKatana;  /* The fallen katana on this tile        */
};

/* Used to generate combos */
struct ProtoCombo {
    int action;            /* Action index (0-8)                  */
    int length;            /* Length of combo (turns to complete) */
    char title[12];        /* Title of combo                      */
    char description[50];  /* Description of combo                */
};

/* Actual combo struct used in-game */
struct Combo {
    int action;    /* Action index (0-8)                           */
    int length;    /* Length of combo (turns to complete)          */
    int combo[10]; /* Inputs to activate combo (based on length)   */
    int infoIndex; /* Index to ProtoCombo with title/description   */
    bool known;    /* Has this combo been discovered by the player */
};

/* Outlines enemy waves */
struct Wave {
    int difficulty;        /* Difficulty of this wave (higher=more enemies)                       */
    unsigned short flags;  /* Flags for the wave (FLAG_ALL_AT_ONCE, WAVE_FLAG_ONLY_ONE_TYPE, etc) */
    int enemiesToSpawn[4]; /* Number of enemies to spawn 0:Basic, 1:Strong, etc                   */
};

/* Holds all data which changes during gameplay          */
/* This is the struct which gets dumped to the save file */
struct GameData {
    struct Player player;                         /* All player data                                                                             */
    int score;                                    /* Current player score                                                                        */
    int currentNumberOfEnemies;                   /* Current number of enemies in-game                                                           */
    struct Enemy enemies[MAX_NUMBER_OF_ENEMIES];  /* Data for enemies in-game                                                                    */
    struct Tile map[MAP_HEIGHT][MAP_WIDTH];       /* Data for current map                                                                        */
    int currentNumberOfFallenKatanas;             /* Current number of Fallen katanas on map                                                     */
    int turn;                                     /* Current game turn                                                                           */
    int previousMoves[HISTORY_LENGTH][3];         /* History of previous moves ([0]:type, [1]:location, [2]:movement)                            */
    int currentEnemyToAttack;                     /* Index to enemy play will/is attack(ing)                                                     */
    int numberOfCombos;                           /* Number of combos in this game                                                               */
    struct Combo combos[MAX_NUMBER_OF_COMBOS];    /* Data for the combos in-game                                                                 */
    int currentNumberOfDiscoveredCombos;          /* Current number of combos discovered by the player                                           */
    int discoveredCombos[MAX_NUMBER_OF_COMBOS];   /* Indexes to discovered combos                                                                */
    int turnsToFreeze;                            /* Turns for enemies to freeze (Freeze combo)                                                  */
    unsigned short comboFlags;                    /* Flags flipped after certain combos (COMBO_FLAG_DOUBLE_ATTACK, COMBO_FLAG_FORCE_ATTACK, etc) */
    struct Wave currentWave;                      /* Data for the current wave                                                                   */
    int currentNumberOfWaves;                     /* Number of the wave the game is on                                                           */
    int turnOfLastEnemySpawn;                     /* Turns since an enemy spawn                                                                  */
    int saveCheck;                                /* Number used to check if file is save file                                                   */
};

/* Global Definitions */
struct GameData currentGameData;

#define NUMBER_OF_TITLE_CARD_LINES 9
const char titleCard[NUMBER_OF_TITLE_CARD_LINES][MAP_WIDTH] = {
    "                                              ",
    "     _   __  ___ _____ ___   _   _   ___      ",
    "    | | / / / _ \\_   _/ _ \\ | \\ | | / _ \\     ",
    "    | |/ / / /_\\ \\| |/ /_\\ \\|  \\| |/ /_\\ \\    ",
    "    |    \\ |  _  || ||  _  ||     ||  _  |    ",
    "    | |\\  \\| | | || || | | || |\\  || | | |    ",
    "    \\_| \\_/\\_| |_/\\_/\\_| |_/\\_| \\_/\\_| |_/    ",
    "              by Joshua Bourgeot              ",
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
    "       Poison      (~)                                                        ",
    "                                                                              ",
    "    Fire       works better on  Magma  but worse against  Energy  enemies     ",
    "    Ice        works better on  Water  but worse against  Fluid   enemies     ",
    "    Wind       works better on  Rock   but worse against  Strong  enemies     ",
    "    Stone      works better on  Rock   but worse against  Strong  enemies     ",
    "    Lightning  works better on  Magma  but worse against  Energy  enemies     ",
    "    Poison     works better on  Water  but worse against  Fluid   enemies     ",
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
    "~"  /* Poison    */
};

const int katanaColor[NUMBER_OF_KATANA_TYPES] = {
    COLOR_RED,     /* Fire      */
    COLOR_BLUE,    /* Ice       */
    COLOR_MAGENTA, /* Wind      */
    COLOR_YELLOW,  /* Stone     */
    COLOR_CYAN,    /* Lightning */
    COLOR_GREEN    /* Poison    */
};

const int katanaToTerrain[NUMBER_OF_KATANA_TYPES] = {
    3, /* Fire -> Magma      */
    1, /* Ice -> Water       */
    2, /* Wind -> Rock       */
    2, /* Stone -> Rock      */
    3, /* Lightning -> Magma */
    1  /* Poison -> Water    */
};

const int katanaToEnemyResistance[NUMBER_OF_KATANA_TYPES] = {
    1, /* Fire -> Energy is resistant      */
    3, /* Ice -> Fluid is resistant        */
    2, /* Wind -> Strong is resistant      */
    2, /* Stone -> Strong is resistant     */
    1, /* Lightning -> Energy is resistant */
    3  /* Poison -> Fluid is resistant     */
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
    "Poison"
};

const char katanaNameType[NUMBER_OF_KATANA_TYPES][10] = {
    "Hi",      /* Fire      */
    "Kori",    /* Ice       */
    "Kaze",    /* Wind      */
    "Iwa",     /* Stone     */
    "Inazuma", /* Lightning */
    "Doku"     /* Poison    */
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
    {2, 2, "Terraform", "Randomly changes the terrain under your feet"},
    {3, 4, "Freeze", "Freezes all enemies for six turns"},
    {4, 3, "Swap", "Randomly swaps the location of two katanas"},
    {5, 3, "Boost", "Double the damage of your next attack"},
    {6, 5, "Teleport", "Teleport to a random location"},
    {7, 3, "Force", "Force your next attack to ignore enemy resistance"},
    {8, 2, "Scare", "Scare enemies into taking a step back"},
};

const char comboLocationToText[4] = {
    TOP_LEFT_KATANA,
    TOP_RIGHT_KATANA,
    BOTTOM_LEFT_KATANA,
    BOTTOM_RIGHT_KATANA
};

/*==== Forward Function Declaration =======================================================================*/
int main();                                                          /* Main function                      */
void mainMenu();                                                     /* Handles main menu screen           */
void gameLoop();                                                     /* Game loop which runs every turn    */
/*---- Find Functions -------------------------------------------------------------------------------------*/
double findDistance(struct Vec2 start, struct Vec2 end, bool pythagoras); /* Distance between two points   */
int findClosestEnemy();                                              /* Return index of the closest enemy  */
double findDistanceToClosestEnemy(struct Vec2 location, bool pythagoras); /* Distance to closest enemy     */
struct Vec2 pathfinding(struct Vec2 start, struct Vec2 end, bool inverse); /* Next step from start to end  */
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
int getStringInput(int y, int x, bool center, int maxBufferSize, char* buffer); /* Get string from player  */
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
void sliceInsertString(char* expression, char* insert, int location, int replacementLen); /* Insert string */
FILE* writeFile(char* filePath);                                     /* Create writable file stream        */
FILE* readFile(char* filePath);                                      /* Open readable file stream          */
void genRandomName(char name[20], bool isSurname, bool gender);      /* Gen a random name                  */
/*---- Curses IO Functions --------------------------------------------------------------------------------*/
void initCurses();                                                   /* Initialise Curses library          */
void initColor();                                                    /* Initialise color for Curses        */
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
void printTileCard();                                                /* Print title card for main menu     */
void printScoresFromScoresFile();                                    /* Print ordered list of scores       */
void printDeathScreen();                                             /* Print death screen showing stats   */
/*---- End of File ----------------------------------------------------------------------------------------*/