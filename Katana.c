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

    currentGameData.turn = 0;

    initCurses();
    printBoarder();


    genMap();
    printMap();

    genMap(); 


    strcpy(currentGameData.player.name, "Test");
    currentGameData.player.location = (struct Vec2) {MAP_HEIGHT/2, MAP_WIDTH/2};
    currentGameData.player.health = PLAYER_START_HEALTH;
    genKatana(&currentGameData.player.katanas[0]);
    genKatana(&currentGameData.player.katanas[1]);
    genKatana(&currentGameData.player.katanas[2]);
    genKatana(&currentGameData.player.katanas[3]);

    for (int i = 0; i < 10; i++) {
        genEnemy();
    }

    gameLoop();

    stopCurses();
}

void gameLoop() {
    int command = 0;
    int selectedKatana;

    bool validMove;

    do {
        selectedKatana = -1;
        validMove = true;
        
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
                validMove = false;
            }
        }
        
        if (validMove) {
            if (selectedKatana != -1) {
                int nearByEnemies[8];
                int number;
                if (findNearbyEnemies(&nearByEnemies, &number)) {
                    /* Right now attack lowest level enemy */
                    int lowestLevelIndex = 0;
                    int lowestLevel = 100;
                    for (int i = 0; i < number; i++) {
                        if (currentGameData.enemies[nearByEnemies[i]].level < lowestLevel) {
                            lowestLevelIndex = i;
                            lowestLevel = currentGameData.enemies[nearByEnemies[i]].level;
                        }
                    }
                    attackEnemy(nearByEnemies[lowestLevelIndex], currentGameData.player.katanas[selectedKatana]);
                } else {
                    playerMove(currentGameData.player.katanas[selectedKatana]);
                }
            }
            
            
            for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
                if (currentGameData.turn - currentGameData.enemies[i].lastMovementTurn >= currentGameData.enemies[i].speed) {
                    if (findDistance(currentGameData.enemies[i].location, currentGameData.player.location) == 1) {
                        attackPlayer(i);
                    } else {
                        enemyMovemet(i);
                    }
                }
            }

            currentGameData.turn++;
        }


        printBoarder();
        printMap();
        for (int i = 0; i < 4; i++) {
            printKatana(currentGameData.player.katanas[i], i);
        }
        printEntities();


        command = myGetch();
    } while (command != 'q' && currentGameData.player.health > 0);
}

/* Find functions */


double findDistance(struct Vec2 start, struct Vec2 end) {
    return fmax(abs(end.y - start.y), abs(end.x - start.x));
}

double findDistanceToClosestEnemy(struct Vec2 location) {
    int closestEnemy = 0;
    int closestEnemyDistance = MAP_HEIGHT + MAP_WIDTH;
    for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
        int currentEnemyDistance = findDistance(location, currentGameData.enemies[i].location);
        if (currentEnemyDistance < closestEnemyDistance) {
            closestEnemy = i;
            closestEnemyDistance = currentEnemyDistance;
        }
    }
    return closestEnemyDistance;
}

struct Vec2 pathFinding(struct Vec2 start, struct Vec2 end) {
    struct Vec2 step = start;

    if (abs(end.x - start.x) < MAP_HEIGHT) {
        if (end.y > start.y) {
            step.y += 1;
        } else if (end.y < start.y) {
            step.y -= 1;
        }
    }

    if (abs(end.y - start.y) < MAP_WIDTH) {
        if (end.x > start.x) {
            step.x += 1;
        } else if (end.x < start.x) {
            step.x -= 1;
        }
    }

    if ((step.y < 0 || step.y > MAP_HEIGHT) || (step.x < 0 || step.x > MAP_WIDTH)) {
        return start;
    }

    return step;
}  

bool findNearbyEnemies(int (*enemies)[8], int *number) {
    *number = 0;
    for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
        if (findDistance(currentGameData.player.location, currentGameData.enemies[i].location) == 1) {
            (*enemies)[(*number)++] = i;
        }
    }

    return (*number != 0);
}

/* Movement */

void playerMove(struct Katana katana) {
    struct Vec2 newLocation;

    switch (katana.movementType) {
        case MOVEMENT_STRIKE: {
            int strongestEnemy = 0;
            for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
                if (currentGameData.enemies[i].level > currentGameData.enemies[strongestEnemy].level) {
                    strongestEnemy = i;
                }
            }
            newLocation = pathFinding(currentGameData.player.location, currentGameData.enemies[strongestEnemy].location);
            break;
        }

        case MOVEMENT_BERSERK: {
            int closestEnemy = 0;
            int closestEnemyDistance = MAP_HEIGHT + MAP_WIDTH;
            for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
                int currentEnemyDistance = findDistance(currentGameData.player.location, currentGameData.enemies[i].location);
                if (currentEnemyDistance < closestEnemyDistance) {
                    closestEnemy = i;
                    closestEnemyDistance = currentEnemyDistance;
                }
            }
            newLocation = pathFinding(currentGameData.player.location, currentGameData.enemies[closestEnemy].location);
            break;
        }
        
        case MOVEMENT_RETREAT: { /* I can't think of a better way to do this */
            double distances[9];
            distances[0] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y + 1, currentGameData.player.location.x + 1});
            distances[1] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y + 1, currentGameData.player.location.x + 0});
            distances[2] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y + 1, currentGameData.player.location.x - 1});
            distances[3] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y + 0, currentGameData.player.location.x + 1});
            distances[4] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y + 0, currentGameData.player.location.x + 0});
            distances[5] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y + 0, currentGameData.player.location.x - 1});
            distances[6] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y - 1, currentGameData.player.location.x + 1});
            distances[7] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y - 1, currentGameData.player.location.x + 0});
            distances[8] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y - 1, currentGameData.player.location.x - 1});

            int farthestDistanceIndex = 0;
            do {
                newLocation = currentGameData.player.location;
                
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
                    if (currentGameData.map[y][x].type == katanaToTerrain[katana.type]) {
                        int currentTerrainDistance = findDistance(currentGameData.player.location, (struct Vec2) {y, x});
                        if (currentTerrainDistance < closestTerrainDistance) {
                            closestTerrain = (struct Vec2) {y, x};
                            closestTerrainDistance = currentTerrainDistance;
                        }
                    }
                }
            }
            newLocation = pathFinding(currentGameData.player.location, closestTerrain);
            break;
        }
        
        case MOVEMENT_DEFEND: {
            newLocation = currentGameData.player.location;
            break;
        }
        
        case MOVEMENT_RAND: {
            do {
                newLocation = currentGameData.player.location;
                newLocation.y += myRand(3) - 1;
                newLocation.x += myRand(3) - 1;
            } while ((newLocation.y == currentGameData.player.location.y && newLocation.x == currentGameData.player.location.x) || ((newLocation.y < 0 || newLocation.y >= MAP_HEIGHT) || (newLocation.x < 0 || newLocation.x >= MAP_WIDTH)));
            break;
        }

        default: {
            ERROR("Invalid movementType argument");
            break;
        }
    }

    currentGameData.player.location = newLocation;
}

void enemyMovemet(int enemyIndex) {
    struct Enemy* currentEnemy = &currentGameData.enemies[enemyIndex];

    struct Vec2 newLocation;
    
    newLocation = pathFinding(currentEnemy->location, currentGameData.player.location);
    if (checkForEnemy(newLocation) == false) {

        currentEnemy->location = newLocation;

        currentEnemy->lastMovementTurn = currentGameData.turn;
    }
}

/* Attacking */

void attackEnemy(int enemyIndex, struct Katana katana) {
    currentGameData.enemies[enemyIndex].health -= katana.damage + myRand(katana.damageMod);
    if (currentGameData.enemies[enemyIndex].health <= 0) {
        removeEnemy(enemyIndex);
    }
}

void attackPlayer(int enemyIndex) {
    currentGameData.player.health -= currentGameData.enemies[enemyIndex].damage;
}


/* Generators & Destructors */

void genEnemy() {
    if (currentGameData.currentNumberOfEnemies < MAX_NUMBER_OF_ENEMIES) {
        struct Enemy *currentEnemy = &currentGameData.enemies[currentGameData.currentNumberOfEnemies];

        currentEnemy->type = myRand(NUMBER_OF_ENEMY_TYPES);

        currentEnemy->speed = abs(dice(3, 4) - 6);

        currentEnemy->health = dice(4, 5);
        currentEnemy->damage = dice(2, 5);
        currentEnemy->level = currentGameData.enemies[currentGameData.currentNumberOfEnemies].health * currentGameData.enemies[currentGameData.currentNumberOfEnemies].damage;


        struct Vec2 location = (struct Vec2) {0, 0};
        do {
            switch (myRand(4)) {
                case 0: {/* Right */
                    location.y = myRand(MAP_HEIGHT);
                    location.x = 0;
                    break;
                }
                case 1: {/* Left */
                    location.y = myRand(MAP_HEIGHT);
                    location.x = MAP_WIDTH - 1;
                    break;
                }
                case 2: {/* Top */
                    location.y = 0;
                    location.x = myRand(MAP_WIDTH);
                    break;
                }
                case 3: {/* Bottom */
                    location.y = MAP_HEIGHT - 1;
                    location.x = myRand(MAP_WIDTH);
                    break;
                }
                default: {
                    ERROR("myRand has failed me");
                }
            }
        } while (checkForEnemy(location) != 0);

        currentEnemy->location = location;
        currentGameData.currentNumberOfEnemies++;
    }
}

void removeEnemy(int enemyIndex) {
    for (int i = enemyIndex; i < currentGameData.currentNumberOfEnemies - 1; i++) {
        currentGameData.enemies[i] = currentGameData.enemies[i + 1];
    }
    currentGameData.currentNumberOfEnemies--;
}

void genKatana(struct Katana *katana) {
    katana->type = myRand(NUMBER_OF_KATANA_TYPES);

    /* 1 - 12 */
    katana->damage = dice(3, 4) + 1;

    /* 0 - 6 */
    katana->damageMod = fmax(dice(4,3) - 6, 0);

    katana->movementType = myRand(NUMBER_OF_MOVEMENT_TYPES);

    sprintf(katana->name, "%s%s", katanaNameType[katana->type], katanaNameDamage[katana->damage - 1]);
}

void genMap() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            currentGameData.map[y][x].type = TERRAIN_GRASS;
        }
    }
    for (int i = 0; i < 20; i++) {
        terrainAreaMap(myRand(NUMBER_OF_TERRAIN_TYPES), (struct Vec2) {myRand(MAP_HEIGHT), myRand(MAP_WIDTH)}, myRand(5) + 2);
    }
}

void terrainAreaMap(int terrain, struct Vec2 location, int radius) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if ( sqrt(pow(y - location.y, 2)) + (pow(x - location.x, 2)/10) <= radius) {
                currentGameData.map[y][x].type = terrain;
            }
        }
    }
}

/* Misc Functions */
bool checkForEnemy(struct Vec2 location) {
    for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
        if (currentGameData.enemies[i].location.y == location.y && currentGameData.enemies[i].location.x == location.x) {
            return 1;
        }
    }
    return 0;
}

/* Utility Functions */
int myRand(int number) {
    if (number > 0) {
        return rand() % number;
    }
    return 0;
}

int dice(int number, int sides) {
    sides++;
    int total = 0;
    for (int i = 0; i < number; i++) {
        total += myRand(sides);
    }
    
    return total;
}

void printError(char *message, char *file, int line){
    stopCurses();
    printf("In file: %s, line: %i\n", file, line);
    printf("   Error: %s\n", message);
    exit(-1);
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


    /* Health */
    attron(COLOR_PAIR(COLOR_MAGENTA));
    int healthAsIcons = (( ((float) currentGameData.player.health) / ((float) PLAYER_START_HEALTH)) * ((float) SCREEN_WIDTH)) - 1;
    printHorizontalLine(SCREEN_HEIGHT / 3, 0, SCREEN_WIDTH - 1, "-");
    printHorizontalLine(SCREEN_HEIGHT / 3, 0, healthAsIcons, "=");
    attrset(A_NORMAL);

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

    attron(COLOR_PAIR(katanaColor[katana.type]));

    printVerticalLine(topLeftCorner.x, topLeftCorner.y, bottomRightCorner.y, "|");
    printVerticalLine(topRightCorner.x, topLeftCorner.y, bottomRightCorner.y, "|");

    printHorizontalLine(topLeftCorner.y, topLeftCorner.x, topRightCorner.x, "-");
    printHorizontalLine(bottomLeftCorner.y, topLeftCorner.x, topRightCorner.x, "-");

    
    mvprintw(topLeftCorner.y, topLeftCorner.x, &katanaCornerIcon[katana.type][0]);
    mvprintw(topRightCorner.y, topRightCorner.x, &katanaCornerIcon[katana.type][0]);
    mvprintw(bottomRightCorner.y, bottomRightCorner.x, &katanaCornerIcon[katana.type][0]);
    mvprintw(bottomLeftCorner.y, bottomLeftCorner.x, &katanaCornerIcon[katana.type][0]);

    attrset(A_NORMAL);


    char buffer[katanaInfoBoxWidth - 2];

    
    mvprintw(topLeftCorner.y + 1, (topLeftCorner.x + (katanaInfoBoxWidth/2)) - (strlen(katana.name)/2), katana.name);
    
    sprintf(buffer,"Dammage: %i", katana.damage);
    mvprintw(topLeftCorner.y + 3, (topLeftCorner.x + (katanaInfoBoxWidth/2)) - (strlen(buffer)/2), buffer);

    if (katana.damageMod != 0) {
        sprintf(buffer, "Damage Mod: %i", katana.damageMod);
        mvprintw(topLeftCorner.y + 4, (topLeftCorner.x + (katanaInfoBoxWidth/2)) - (strlen(buffer)/2), buffer);
    }

    sprintf(buffer, "Movement Type: %s", katanaMovementTypeNames[katana.movementType]);
    mvprintw(topLeftCorner.y + 5, (topLeftCorner.x + (katanaInfoBoxWidth/2)) - (strlen(buffer)/2), buffer);
    
    
}

void printMap() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            attron(COLOR_PAIR(terrainColor[currentGameData.map[y][x].type]));
            mvprintw(y + 1, x + 1, terrainIcon[currentGameData.map[y][x].type]);
        }
    }
    attrset(A_NORMAL);
}

void printEntities(){
    for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
        attron(COLOR_PAIR(enemyColor[currentGameData.enemies[i].type]));
        mvprintw(currentGameData.enemies[i].location.y + 1, currentGameData.enemies[i].location.x + 1, enemyIcon[currentGameData.enemies[i].type]);
    }

    attron(COLOR_PAIR(COLOR_WHITE));
    mvprintw(currentGameData.player.location.y + 1, currentGameData.player.location.x + 1, "@");
    attrset(A_NORMAL);
}