/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/*  File    :   katana.h                                                                                   */
/*  Author  :   Joshua Bourgeot                                                                            */
/*  Date    :   14th January 2021                                                                          */
/*  Function:   Katana, simple arcade-like game                                                            */
/*                                                                                                         */
/*---- Program Summary ------------------------------------------------------------------------------------*/
/*                                                                                                         */
/*     TBD                                                                                                 */
/*                                                                                                         */
/*---- Include Files --------------------------------------------------------------------------------------*/
#include <stdlib.h>          /* General functions                                                          */
#include <stdbool.h>         /* Definitions for bool type                                                  */
#include <time.h>            /* Functions for date and time                                                */
#include <string.h>          /* Functions for string manipulation                                          */
#include <math.h>            /* General math functions                                                     */
#include <pdcurses/curses.h> /* Libraray for terminal manipulation                                         */
#include "Katana.h"          /* Katana variables, arrays and structures                                    */
/*---- Main Function --------------------------------------------------------------------------------------*/


void main(){
    long seed = time(NULL);
    srand(seed);

    initCurses();
    printBoarder();


    genMap();
    printMap();

    genMap(); 


    strcpy(player.name, "Test");
    player.location = (struct Vec2) {MAP_HEIGHT/2, MAP_WIDTH/2};
    player.health = 100;
    genKatana(&player.katanas[0]);
    genKatana(&player.katanas[1]);
    genKatana(&player.katanas[2]);
    genKatana(&player.katanas[3]);

    genEnemy();
    genEnemy();
    genEnemy();

    gameLoop();

    stopCurses();
}

void gameLoop() {
    int command = 0;
    int selectedKatana = -1;

    do {
        switch (command) {
            case TOP_LEFT_KATANA: {
                selectedKatana = 0;
                break;
            }
            case TOP_RIGHT_KATANA: {
                selectedKatana = 1;
                break;
            }
            case BOTTOM_LEFT_KATANA: {
                selectedKatana = 2;
                break;
            }
            case BOTTOM_RIGHT_KATANA: {
                selectedKatana = 3;
                break;
            }
            
            default: {
                selectedKatana = -1;
            }
        }

        if (selectedKatana != -1) {
            playerMove(player.katanas[selectedKatana]);
        }


        printBoarder();
        printMap();
        for (int i = 0; i < 4; i++) {
            printKatana(player.katanas[i], i);
        }
        printEntities();

        command = myGetch();
    } while (command != 'q');
}

/* Movement */

/*
#define MOVEMENT_STRIKE   1 // Move to strongest enemy                        
#define MOVEMENT_BERSERK  2 // Move to the closest enemy                      
#define MOVEMENT_RETREAT  3 // Move away from most enemies                    
#define MOVEMENT_RETURN   4 // Attempt to move to terrain maching katana type 
#define MOVEMENT_DEFEND   5 // Don't move                                     
#define MOVEMENT_RAND     6 // Move in random newLocation                       
*/

void playerMove(struct Katana katana) {
    struct Vec2 newLocation;

    switch (katana.movementType) {
        case MOVEMENT_STRIKE: {
            int strongestEnemy = 0;
            for (int i = 0; i < currentNumberOfEnemies; i++) {
                if (enemies[i].level > enemies[strongestEnemy].level) {
                    strongestEnemy = i;
                }
            }
            newLocation = pathFinding(player.location, enemies[strongestEnemy].location);
            break;
        }

        case MOVEMENT_BERSERK: {
            int closestEnemy = 0;
            int closestEnemyDistance = MAP_HEIGHT + MAP_WIDTH;
            for (int i = 0; i < currentNumberOfEnemies; i++) {
                int currentEnemyDistance = findDistance(player.location, enemies[i].location);
                if (currentEnemyDistance < closestEnemyDistance) {
                    closestEnemy = i;
                    closestEnemyDistance = currentEnemyDistance;
                }
            }
            newLocation = pathFinding(player.location, enemies[closestEnemy].location);
            break;
        }
        
        case MOVEMENT_RETREAT: { /* I can't think of a better way to do this */
            double distances[9];
            distances[0] = findDistanceToClosestEnemy((struct Vec2) {player.location.y + 1, player.location.x + 1});
            distances[1] = findDistanceToClosestEnemy((struct Vec2) {player.location.y + 1, player.location.x + 0});
            distances[2] = findDistanceToClosestEnemy((struct Vec2) {player.location.y + 1, player.location.x - 1});
            distances[3] = findDistanceToClosestEnemy((struct Vec2) {player.location.y + 0, player.location.x + 1});
            distances[4] = findDistanceToClosestEnemy((struct Vec2) {player.location.y + 0, player.location.x + 0});
            distances[5] = findDistanceToClosestEnemy((struct Vec2) {player.location.y + 0, player.location.x - 1});
            distances[6] = findDistanceToClosestEnemy((struct Vec2) {player.location.y - 1, player.location.x + 1});
            distances[7] = findDistanceToClosestEnemy((struct Vec2) {player.location.y - 1, player.location.x + 0});
            distances[8] = findDistanceToClosestEnemy((struct Vec2) {player.location.y - 1, player.location.x - 1});

            int farthestDistanceIndex = 0;
            do {
                newLocation = player.location;
                
                for (int i = 0; i < 9; i++) {
                    if (distances[i] > distances[farthestDistanceIndex]) {
                        farthestDistanceIndex = i;
                    }
                }

                switch (farthestDistanceIndex) {
                    case 0: { newLocation.y += 1; newLocation.x += 1; break; }
                    case 1: { newLocation.y += 1; newLocation.x += 0; break; }
                    case 2: { newLocation.y += 1; newLocation.x -= 1; break; }
                    case 3: { newLocation.y += 0; newLocation.x += 1; break; }
                    case 4: { newLocation.y += 0; newLocation.x += 0; break; }
                    case 5: { newLocation.y += 0; newLocation.x -= 1; break; }
                    case 6: { newLocation.y -= 1; newLocation.x += 1; break; }
                    case 7: { newLocation.y -= 1; newLocation.x += 0; break; }
                    case 8: { newLocation.y -= 1; newLocation.x -= 1; break; }

                    default:{ ERROR("This shouldn't be possible");           }
                }

                distances[farthestDistanceIndex] = 0; /* If this option is out of bounds, make sure we don't pick it again */
            } while ((newLocation.y < 0 || newLocation.y >= MAP_HEIGHT) || (newLocation.x < 0 || newLocation.x >= MAP_WIDTH));
            
            
            break;
        }
        
        case MOVEMENT_RETURN: { /* This could be more efficient, I know */
            struct Vec2 closestTerrain = (struct Vec2) {0, 0};
            int closestTerrainDistance = MAP_HEIGHT + MAP_WIDTH;
            for (int y = 0; y < MAP_HEIGHT; y++) {
                for (int x = 0; x < MAP_WIDTH; x++) {
                    if (map[y][x].type == katanaToTerrain[katana.katanaType]) {
                        int currentTerrainDistance = findDistance(player.location, (struct Vec2) {y, x});
                        if (currentTerrainDistance < closestTerrainDistance) {
                            closestTerrain = (struct Vec2) {y, x};
                            closestTerrainDistance = currentTerrainDistance;
                        }
                    }
                }
            }
            newLocation = pathFinding(player.location, closestTerrain);
            break;
        }
        
        case MOVEMENT_DEFEND: {
            newLocation = player.location;
            break;
        }
        
        case MOVEMENT_RAND: {
            do {
                newLocation = player.location;
                newLocation.y += (rand() % 3) - 1;
                newLocation.x += (rand() % 3) - 1;
            } while ((newLocation.y == player.location.y && newLocation.x == player.location.x) || ((newLocation.y < 0 || newLocation.y >= MAP_HEIGHT) || (newLocation.x < 0 || newLocation.x >= MAP_WIDTH)));
            break;
        }

        default: {
            ERROR("Invalid movementType argument");
            break;
        }
    }

    player.location = newLocation;
}


/* Generators */

void genEnemy() {
    if (currentNumberOfEnemies < MAX_NUMBER_OF_ENEMIES) {
        enemies[currentNumberOfEnemies].type = rand() % NUMBER_OF_ENEMY_TYPES;

        enemies[currentNumberOfEnemies].health = dice(2, 5);
        enemies[currentNumberOfEnemies].power = dice(2, 5);
        enemies[currentNumberOfEnemies].level = enemies[currentNumberOfEnemies].health * enemies[currentNumberOfEnemies].power;


        struct Vec2 location = (struct Vec2) {0, 0};
        do {
            switch (rand() % 4) {
                case 0: {/* Right */
                    location.y = rand() % MAP_HEIGHT;
                    location.x = 0;
                    break;
                }
                case 1: {/* Left */
                    location.y = rand() % MAP_HEIGHT;
                    location.x = MAP_WIDTH - 1;
                    break;
                }
                case 2: {/* Top */
                    location.y = 0;
                    location.x = rand() % MAP_WIDTH;
                    break;
                }
                case 3: {/* Bottom */
                    location.y = MAP_HEIGHT - 1;
                    location.x = rand() % MAP_WIDTH;
                    break;
                }
                default: {
                    ERROR("Rand has failed me");
                }
            }
        } while (checkForEnemy(location) != 0);

        enemies[currentNumberOfEnemies].location = location;
        currentNumberOfEnemies++;
    }
}

void genKatana(struct Katana *katana) {
    katana->katanaType = (rand() % NUMBER_OF_KATANA_TYPES);
    /* 1 - 12 */
    katana->damageAmount = dice(3, 4) + 1;

    katana->movementType = (rand() % NUMBER_OF_MOVEMENT_TYPES);

    /* 20 - 100 */
    katana->hitChance = dice(4, 20) + 20;

    sprintf(katana->name, "%s%s", katanaNameType[katana->katanaType], katanaNameDamage[katana->damageAmount - 1]);
}

void genMap() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x].type = TERRAIN_DIRT;
        }
    }
    for (int i = 0; i < 20; i++) {
        terrainAreaMap(rand() % NUMBER_OF_TERRAIN_TYPES, (struct Vec2) {rand() % MAP_HEIGHT, rand() % MAP_WIDTH}, (rand() % 5) + 2);
    }
}

void terrainAreaMap(int terrain, struct Vec2 location, int radius) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if ( sqrt(pow(y - location.y, 2)) + (pow(x - location.x, 2)/10) <= radius) {
                map[y][x].type = terrain;
            }
        }
    }
}

/* Misc Functions */
bool checkForEnemy(struct Vec2 location) {
    for (int i = 0; i < currentNumberOfEnemies; i++) {
        if (enemies[i].location.y == location.y && enemies[i].location.x == location.x) {
            return 1;
        }
    }
    return 0;
}




/* Utility Functions */
int dice(int number, int sides) {
    sides++;
    int total = 0;
    for (int i = 0; i < number; i++) {
        total += (rand() % sides);
    }
    
    return total;
}

void printError(char *message, char *file, int line){
    stopCurses();
    printf("In file: %s, line: %i\n", file, line);
    printf("   Error: %s\n", message);
    exit(-1);
}

double findDistance(struct Vec2 start, struct Vec2 end) {
    return fmax(abs(end.y - start.y), abs(end.x - start.x));
}

double findDistanceToClosestEnemy(struct Vec2 location) {
    int closestEnemy = 0;
    int closestEnemyDistance = MAP_HEIGHT + MAP_WIDTH;
    for (int i = 0; i < currentNumberOfEnemies; i++) {
        int currentEnemyDistance = findDistance(location, enemies[i].location);
        if (currentEnemyDistance < closestEnemyDistance) {
            closestEnemy = i;
            closestEnemyDistance = currentEnemyDistance;
        }
    }
    return closestEnemyDistance;
}

struct Vec2 pathFinding(struct Vec2 start, struct Vec2 end) {
    struct Vec2 step = start;

    if (end.y > start.y) {
        step.y += 1;
    } else if (end.y < start.y) {
        step.y -= 1;
    }

    if (end.x > start.x) {
        step.x += 1;
    } else if (end.x < start.x) {
        step.x -= 1;
    }

    if ((step.y < 0 || step.y > MAP_HEIGHT) || (step.x < 0 || step.x > MAP_WIDTH)) {
        return start;
    }

    return step;
}   


/* Curses IO Functions */


void initColor(){
    /* Alow the use of color */
    use_default_colors();
    start_color();
    init_pair(COLOR_BLACK, COLOR_BLACK, COLOR_BLACK);
    init_pair(COLOR_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(COLOR_RED, COLOR_RED, COLOR_BLACK);
    init_pair(COLOR_CYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(COLOR_WHITE, COLOR_WHITE, COLOR_BLACK);
    init_pair(COLOR_MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(COLOR_BLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(COLOR_YELLOW, COLOR_YELLOW, COLOR_BLACK);
}


void initCurses(){
    /* Init curses */
    initscr(); 
    /* Disable buffering and get one character-at-a-time input */
    cbreak(); 
    /* Get Special keys */
    keypad(stdscr, TRUE);
    /* Suppress echoing of typed keys */
    noecho();
    /* Hide cursor */
    curs_set(0);

    initColor();

    /* nodelay(stdscr, true); */
}


void stopCurses(){
    // Gracefully stop curses
    endwin();
}


int myGetch(){
    return getch();
}


void clearScreen(){
    erase();
}


void printHorizontalLine(int y, int start, int stop, char* toPrint){
    for (int i = start; i <= stop; i++){
        mvprintw(y, i, toPrint);
    }
}


void printVerticalLine(int x, int start, int stop, char* toPrint){
    for (int i = start; i <= stop; i++){
        mvprintw(i, x, toPrint);
    }
}


void printBox(int y, int x, int stopY, int stopX, char* toPrint){
    for (int i = y; i < stopY; i++){
        printHorizontalLine(i, x, stopX, toPrint);
    }
}


void printBoarder(){
    printVerticalLine(0, 0, SCREEN_HEIGHT - 1, "|");
    printVerticalLine(SCREEN_WIDTH - 1, 0, SCREEN_HEIGHT - 1, "|");

    printHorizontalLine(0, 0, SCREEN_WIDTH - 1, "-");
    printHorizontalLine(SCREEN_HEIGHT / 3, 0,  SCREEN_WIDTH - 1, "-");
    printHorizontalLine(SCREEN_HEIGHT - 1, 0, SCREEN_WIDTH - 1, "-");

    mvprintw(0, 0, "+");
    mvprintw(0, SCREEN_WIDTH - 1, "+");

    mvprintw(SCREEN_HEIGHT / 3, 0, "+");
    mvprintw(SCREEN_HEIGHT / 3, SCREEN_WIDTH - 1, "+");

    mvprintw(SCREEN_HEIGHT - 1, 0, "+");
    mvprintw(SCREEN_HEIGHT - 1, SCREEN_WIDTH - 1, "+");
}


void printKatana(struct Katana katana, int position) {
    int katanaInfoBoxHeight = (SCREEN_HEIGHT / 3) - 2;
    int katanaInfoBoxWidth = (SCREEN_WIDTH / 2) - 2;

    struct Vec2 topRightCorner = {0, 0};
    struct Vec2 topLeftCorner = {0, 0};
    struct Vec2 bottomRightCorner = {0, 0};
    struct Vec2 bottomLeftCorner = {0, 0};
    
    switch(position) {
        case 0: {/* Top left katana */
            topLeftCorner.y = (SCREEN_HEIGHT / 3) + 1;
            topLeftCorner.x = 1;
            break;
        }
        case 1: {/* Top right katana */
            topLeftCorner.y = (SCREEN_HEIGHT / 3) + 1;
            topLeftCorner.x = (SCREEN_WIDTH - 2) - katanaInfoBoxWidth;
            break;
        }
        case 2: {/* Bottom left katana */
            topLeftCorner.y = (SCREEN_HEIGHT / 3) + 2 + katanaInfoBoxHeight;
            topLeftCorner.x = 1;
            break;
        }
        case 3: {/* Bottom right katana */
            topLeftCorner.y = (SCREEN_HEIGHT / 3) + 2 + katanaInfoBoxHeight;
            topLeftCorner.x = (SCREEN_WIDTH - 2) - katanaInfoBoxWidth;
            break;
        }
        default: {
            ERROR("Invalid position argument");
        }
    }

    topRightCorner.y = topLeftCorner.y;
    topRightCorner.x = topLeftCorner.x + katanaInfoBoxWidth;

    bottomRightCorner.y = topLeftCorner.y + katanaInfoBoxHeight;
    bottomRightCorner.x = topLeftCorner.x;

    bottomLeftCorner.y = topLeftCorner.y + katanaInfoBoxHeight;
    bottomLeftCorner.x = topLeftCorner.x + katanaInfoBoxWidth;

    
    printVerticalLine(topLeftCorner.x, topLeftCorner.y, bottomRightCorner.y, "|");
    printVerticalLine(topRightCorner.x, topLeftCorner.y, bottomRightCorner.y, "|");

    printHorizontalLine(topLeftCorner.y, topLeftCorner.x, topRightCorner.x, "-");
    printHorizontalLine(bottomLeftCorner.y, topLeftCorner.x, topRightCorner.x, "-");

    
    mvprintw(topLeftCorner.y, topLeftCorner.x, &katanaCornerIcon[katana.katanaType][0]);
    mvprintw(topRightCorner.y, topRightCorner.x, &katanaCornerIcon[katana.katanaType][0]);
    mvprintw(bottomRightCorner.y, bottomRightCorner.x, &katanaCornerIcon[katana.katanaType][0]);
    mvprintw(bottomLeftCorner.y, bottomLeftCorner.x, &katanaCornerIcon[katana.katanaType][0]);

    char buffer[katanaInfoBoxWidth - 2];

    
    mvprintw(topLeftCorner.y + 1, (topLeftCorner.x + (katanaInfoBoxWidth/2)) - (strlen(katana.name)/2), katana.name);
    
    sprintf(buffer,"Dammage: %i", katana.damageAmount);
    mvprintw(topLeftCorner.y + 3, (topLeftCorner.x + (katanaInfoBoxWidth/2)) - (strlen(buffer)/2), buffer);

    sprintf(buffer, "Hit Chance: %i", katana.hitChance);
    mvprintw(topLeftCorner.y + 4, (topLeftCorner.x + (katanaInfoBoxWidth/2)) - (strlen(buffer)/2), buffer);

    sprintf(buffer, "Movement Type: %s", katanaMovementTypeNames[katana.movementType]);
    mvprintw(topLeftCorner.y + 5, (topLeftCorner.x + (katanaInfoBoxWidth/2)) - (strlen(buffer)/2), buffer);
    
    
}

void printMap() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            mvprintw(y + 1, x + 1, terrainIcon[map[y][x].type]);
        }
    }
}

void printEntities(){
    for (int i = 0; i < currentNumberOfEnemies; i++) {
        enemies[i].location;
        mvprintw(enemies[i].location.y + 1, enemies[i].location.x + 1, enemyIcon[enemies[i].type]);
    }

    mvprintw(player.location.y + 1, player.location.x + 1, "@");
}