/*---------------------------------------------------------------------------------------------------------*/
/*                                                                                                         */
/*  File    :   katana.c                                                                                   */
/*  Author  :   Joshua Bourgeot                                                                            */
/*  Date    :   14th January 2021                                                                          */
/*  Function:   Katana, simple arcade-like game                                                            */
/*                                                                                                         */
/*---- Include Files --------------------------------------------------------------------------------------*/
#include <stdlib.h>              /* General functions                                                      */
#include <stdbool.h>             /* Definitions for bool type                                              */
#include <time.h>                /* Functions for date and time                                            */
#include <string.h>              /* Functions for string manipulation                                      */
#include <math.h>                /* General math functions                                                 */
#include <ctype.h>               /* Char manipulation                                                      */
#include <unistd.h>              /* Misc                                                                   */
#ifdef _WIN32                    /*                                                                        */
    #include <pdcurses/curses.h> /* Libraray for terminal manipulation (Windows)                           */
#else                            /*                                                                        */
    #include <ncurses.h>         /* Libraray for terminal manipulation (Unix based systems)                */
#endif                           /*                                                                        */
#include "Katana.h"              /* Katana variables, arrays, and structures                               */
/*---- Main Function --------------------------------------------------------------------------------------*/


int main(){
    long seed = time(NULL);
    srand(seed);

    initCurses();

    /* Init data values to defaults */
    currentGameData.score = 0;
    currentGameData.turn = 0;
    currentGameData.turnsToFreeze = 0;
    currentGameData.numberOfCombos = 0;
    currentGameData.comboFlags = 0;
    currentGameData.currentNumberOfFallenKatanas = 0;
    currentGameData.currentNumberOfWaves = 0;
    currentGameData.currentNumberOfEnemies = 0;
    currentGameData.player.enemiesKilled = 0;
    currentGameData.player.attacks = 0;
    currentGameData.player.moves = 0;
    currentGameData.player.katanasPickedUp = 0;
    currentGameData.player.katanasBroken = 0;

    currentGameData.saveCheck = 572;

    currentGameData.currentWave = (struct Wave) {0, 0, {0, 0, 0, 0}};

    for (int i = 0; i < HISTORY_LENGTH; i++) {
        currentGameData.previousMoves[i][0] = -1;
        currentGameData.previousMoves[i][1] = -1;
        currentGameData.previousMoves[i][2] = -1;
    }

    strcpy(currentGameData.player.name, "Tanjiro"); /* Gotta have that Demon Slayer reference */
    currentGameData.player.location = (struct Vec2) {MAP_HEIGHT/2, MAP_WIDTH/2};
    currentGameData.player.health = PLAYER_START_HEALTH;
    currentGameData.player.turnOfLastCombo = 0;
    genKatana(&currentGameData.player.katanas[0], MOVEMENT_FALLEN);
    genKatana(&currentGameData.player.katanas[1], -1);
    genKatana(&currentGameData.player.katanas[2], -1);
    genKatana(&currentGameData.player.katanas[3], -1);

    genPlayerDNA();


    genCombo(0);
    currentGameData.currentNumberOfDiscoveredCombos = 1;
    currentGameData.discoveredCombos[0] = 0;

    int numberOfCombosInGame = 2 + myRand(2);
    for (int i = 1; i < numberOfCombosInGame; i++) {
        int newCombo;
        bool alreadyCombo;
        do {
            newCombo = myRand(NUMBER_OF_PROTO_COMBOS);
            alreadyCombo = false;
            for (int i = 0; i < currentGameData.numberOfCombos; i++) {
                if (newCombo == currentGameData.combos[i].action){
                    alreadyCombo = true;
                }
            }
        } while (alreadyCombo);

        genCombo(newCombo);
    }

    genMap();


    mainMenu();


    /* Exit gracefully */
    stopCurses();
    return 1;
}

void mainMenu(){
    clearScreen();
    printBoarder(false);
    printTileCard();

    int choice = menu(menuOptions, NUMBER_OF_MENU_OPTIONS, NUMBER_OF_TITLE_CARD_LINES + 2, (SCREEN_WIDTH/2) - (strlen(menuOptions[0])/2));

    switch (choice) {
        case 0: { /* New game */
            char buffer[20];
            clearScreen();
            mvprintw(MAP_HEIGHT/2, (MAP_WIDTH/2) - 27, "Please enter your name. Leave blank for a random name");
            if (getStringInput((MAP_HEIGHT/2) + 1, 0, true, 20, &buffer[0]) == -1) {
                mainMenu();
                return;
            }

            strncpy(currentGameData.player.name, &buffer[0], 20);

            if (currentGameData.player.name[0] == '\0') {
                genRandomName(currentGameData.player.name, false, myRand(1));
            }

            gameLoop();
            break;
        }
        case 1: { /* Load game */
            char buffer[100];
            clearScreen();
            do {
                mvprintw(MAP_HEIGHT/2, (MAP_WIDTH/2) - 10, "Enter valid filepath");
                if (getStringInput((MAP_HEIGHT/2) + 1, 0, true, 100, &buffer[0]) == -1) {
                    mainMenu();
                    return;
                }

                mvprintw(MAP_HEIGHT/2 + 4, (MAP_WIDTH/2) - 14, "File is not valid save file");
            } while (loadGame(readFile(buffer)) != 1);

            clearScreen();

            gameLoop();
            break;
        }
        case 2: { /* Leaderboard */
            printScoresFromScoresFile();
            mainMenu();
            return;
        }
    }
}

void gameLoop() {
    int command = 0;
    int selectedKatana;

    bool validMove = false;;
    bool infoKey = false;
    bool noActionCommand = false;;

    do {
        noActionCommand = infoKey;
        selectedKatana = -1;
        validMove = true;
        infoKey = false;
        
        switch (command) {
            case TOP_LEFT_KATANA: { /* Use katana */
                selectedKatana = 0;
                break;
            }
            case TOP_LEFT_KATANA_SECONDARY: { /* View info on katana */
                selectedKatana = 0;
                infoKey = true;
                break;
            }
            case TOP_RIGHT_KATANA: { /* Use katana */
                selectedKatana = 1;
                break;
            }
            case TOP_RIGHT_KATANA_SECONDARY: { /* View info on katana */
                selectedKatana = 1;
                infoKey = true;
                break;
            }
            case BOTTOM_LEFT_KATANA: { /* Use katana */
                selectedKatana = 2;
                break;
            }
            case BOTTOM_LEFT_KATANA_SECONDARY: { /* View info on katana */
                selectedKatana = 2;
                infoKey = true;
                break;
            }
            case BOTTOM_RIGHT_KATANA: { /* Use katana */
                selectedKatana = 3;
                break;
            }
            case BOTTOM_RIGHT_KATANA_SECONDARY: { /* View info on katana */
                selectedKatana = 3;
                infoKey = true;
                break;
            }

            case HELP_KEY: { /* Print quick help screen */
                infoKey = true;
                break;
            } 

            case COMBO_REFERENCE_KEY: { /* Print combo screen */
                infoKey = true;
                break;
            }

            case ENEMY_CHECK_KEY: { /* Info on closest enemy */
                infoKey = true;
                break;
            }

            case PLAYER_DATA_KEY: { /* Print player data screen */
                infoKey = true;
                break;
            }

            default: {
                validMove = false;
            }
        }

        if (validMove && !noActionCommand && !infoKey) { /* Game turn */
            pushPreviousMove(currentGameData.player.katanas[selectedKatana].type, selectedKatana, currentGameData.player.katanas[selectedKatana].movementType);
            
            if (myRand(CHANCE_FOR_FALLEN_KATANA) == 0) {
                genFallenKatana();
            }

            enemyWave();


            if (CHECK_BIT(currentGameData.comboFlags, COMBO_FLAG_SCARE_ENEMIES)) {
                currentGameData.comboFlags ^= COMBO_FLAG_SCARE_ENEMIES;
            }

            activateCombo(checkForCombo());

            currentGameData.currentEnemyToAttack = -1;
            if (selectedKatana != -1) {
                int nearByEnemies[8];
                int number;

                /* If enemy is nearby and a retreat katana has not been used twice in a row */
                if (findNearbyEnemies(currentGameData.player.location, &nearByEnemies, &number) && !(currentGameData.previousMoves[1][2] == MOVEMENT_RETREAT && currentGameData.previousMoves[0][2] == MOVEMENT_RETREAT)) {
                
                    /* Right now attack lowest level enemy */
                    int lowestLevelIndex = 0;
                    int lowestLevel = 100;
                    for (int i = 0; i < number; i++) {
                        if (currentGameData.enemies[nearByEnemies[i]].level < lowestLevel) {
                            lowestLevelIndex = i;
                            lowestLevel = currentGameData.enemies[nearByEnemies[i]].level;
                        }
                    }
                    attackEnemy(nearByEnemies[lowestLevelIndex], &currentGameData.player.katanas[selectedKatana]);
                    currentGameData.currentEnemyToAttack = nearByEnemies[lowestLevelIndex];
                    currentGameData.player.katanas[selectedKatana].numberOfStrikes++;
                } else {
                    playerMove(currentGameData.player.katanas[selectedKatana]);
                    if (currentGameData.map[currentGameData.player.location.y][currentGameData.player.location.x].hasFallenKatana) {
                        pickUpFallenKatana();
                    }
                    currentGameData.player.katanas[selectedKatana].numberOfMoves++;

                    /* Increase health when not attacking */
                    if (myRand(CHANCE_FOR_HEALTH_INCREASE) == 0) {
                        currentGameData.player.health += myRand(MAX_HEALTH_INCREASE - 1) + 1;
                    }
                    if (currentGameData.player.health > PLAYER_START_HEALTH) {
                        currentGameData.player.health = PLAYER_START_HEALTH;
                    }

                }
            }

            if (currentGameData.turnsToFreeze == 0) {
                for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
                    if (doesEnemyMoveThisTurn(i)) {
                        if (findDistance(currentGameData.enemies[i].location, currentGameData.player.location, false) == 1) {
                            attackPlayer(i);
                        } else {
                            enemyMovement(i);
                        }
                    }
                }
            } else {
                currentGameData.turnsToFreeze--;
            }

            currentGameData.turn++;
        }

        clear();
        printBoarder(true);
        printMap();
        for (int i = 0; i < 4; i++) {
            printKatana(currentGameData.player.katanas[i], i);
        }
        printEntities();

        if (infoKey) { /* Non-game turn */
            if (selectedKatana != -1) {
                printKatanaDescription(currentGameData.player.katanas[selectedKatana], false);
            } else if (command == HELP_KEY) {
                printHelp();
            } else if (command == COMBO_REFERENCE_KEY) {
                printComboReference();
            } else if (command == ENEMY_CHECK_KEY) {
                printEnemyInfo();
            } else if (command == PLAYER_DATA_KEY) {
                printPlayerData();
            } else {
                ERROR("Invalid info key");
            }
        }

        command = myGetch();

        if (command == KEY_ESC) {
            printBox((MAP_HEIGHT/2) - 1, (MAP_WIDTH/2) - 13, (MAP_HEIGHT/2) + 1, (MAP_WIDTH/2) + 12, "*");
            mvprintw((MAP_HEIGHT/2), (MAP_WIDTH/2) - 12, "Do you want to quit? (y)");
            if (myGetch() == 'y') {
                command = QUIT_KEY;
            }
        }
    } while (command != QUIT_KEY && currentGameData.player.health > 0);
    
    clearScreen();
    if (currentGameData.player.health > 0) { /* Option to save game if player still alive */
        printBoarder(false);
        mvprintw(MAP_HEIGHT/2, (MAP_WIDTH/2) - 7, "Save game? (y)");
        if (myGetch() == 'y') {
            char buffer[100];
            printBoarder(false);
            mvprintw(MAP_HEIGHT/2, (MAP_WIDTH/2) - 10, "Enter save filename");
            
            if (getStringInput((MAP_HEIGHT/2) + 1, 0, true, 100, &buffer[0]) == -1) {
                return;
            }

            saveGame(writeFile(buffer));
        }
    } else {
        printDeathScreen();

        saveScore();
        printScoresFromScoresFile();
    }
}

/* Find functions */
double findDistance(struct Vec2 start, struct Vec2 end, bool pythagoras) {
    /* Pythagoras is only used in the player retreat movement code as it gives better results. Chebyshev distance used everywhere else */
    if (pythagoras) {
        return sqrt(pow(end.y - start.y, 2) + pow(end.x - start.x, 2));
    } else { 
        return fmax(abs(end.y - start.y), abs(end.x - start.x));
    }
}

int findClosestEnemy(){
    int closestEnemy = 0;
    int closestEnemyDistance = MAP_HEIGHT + MAP_WIDTH;
    for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
        int currentEnemyDistance = findDistance(currentGameData.player.location, currentGameData.enemies[i].location, false);
        if (currentEnemyDistance < closestEnemyDistance) {
            closestEnemy = i;
            closestEnemyDistance = currentEnemyDistance;
        }
    }
    return closestEnemy;
}

double findDistanceToClosestEnemy(struct Vec2 location, bool pythagoras) {
    double closestEnemyDistance = MAP_HEIGHT + MAP_WIDTH;
    for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
        double currentEnemyDistance = findDistance(location, currentGameData.enemies[i].location, pythagoras);
        if (currentEnemyDistance < closestEnemyDistance) {
            closestEnemyDistance = currentEnemyDistance;
        }
    }
    return closestEnemyDistance;
}

struct Vec2 pathfinding(struct Vec2 start, struct Vec2 end, bool inverse) {
    /* Extremely simple pathfinding */
    struct Vec2 step = start;

    if (abs(end.x - start.x) < MAP_HEIGHT) {
        if (end.y > start.y) {
            step.y += inverse?-1:1;
        } else if (end.y < start.y) {
            step.y += inverse?1:-1;
        }
    }

    if (abs(end.y - start.y) < MAP_WIDTH) {
        if (end.x > start.x) {
            step.x += inverse?-1:1;
        } else if (end.x < start.x) {
            step.x += inverse?1:-1;
        }
    }

    if ((step.y < 0 || step.y > MAP_HEIGHT) || (step.x < 0 || step.x > MAP_WIDTH)) {
        return start;
    }

    return step;
}  

bool findNearbyEnemies(struct Vec2 loc, int (*enemies)[8], int *number) {
    /* Find enemies on adjacent tiles */
    *number = 0;
    for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
        if (findDistance(loc, currentGameData.enemies[i].location, false) == 1) {
            (*enemies)[(*number)++] = i;
        }
    }

    return (*number != 0);
}

/* Movement */
void playerMove(struct Katana katana) {
    struct Vec2 newLocation = currentGameData.player.location;

    switch (katana.movementType) {
        case MOVEMENT_STRIKE: { /* Move to strongest enemy */
            if (currentGameData.currentNumberOfEnemies == 0) {
                break;
            }

            int strongestEnemy = 0;
            for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
                if (currentGameData.enemies[i].level > currentGameData.enemies[strongestEnemy].level) {
                    strongestEnemy = i;
                }
            }
            newLocation = pathfinding(currentGameData.player.location, currentGameData.enemies[strongestEnemy].location, false);
            break;
        }

        case MOVEMENT_BERSERK: { /* Move to the closest enemy */
            if (currentGameData.currentNumberOfEnemies == 0) {
                break;
            }

            int closestEnemy = 0;
            int closestEnemyDistance = MAP_HEIGHT + MAP_WIDTH;
            for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
                int currentEnemyDistance = findDistance(currentGameData.player.location, currentGameData.enemies[i].location, false);
                if (currentEnemyDistance < closestEnemyDistance) {
                    closestEnemy = i;
                    closestEnemyDistance = currentEnemyDistance;
                }
            }
            newLocation = pathfinding(currentGameData.player.location, currentGameData.enemies[closestEnemy].location, false);
            break;
        }
        
        case MOVEMENT_RETREAT: { /* Move away from the most enemies */
            /* I can't think of a better way to do this */
            if (currentGameData.currentNumberOfEnemies == 0) {
                break;
            }

            double distances[9];
            distances[0] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y + 1, currentGameData.player.location.x + 1}, true);
            distances[1] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y + 1, currentGameData.player.location.x + 0}, true);
            distances[2] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y + 1, currentGameData.player.location.x - 1}, true);
            distances[3] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y + 0, currentGameData.player.location.x + 1}, true);
            distances[4] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y + 0, currentGameData.player.location.x + 0}, true);
            distances[5] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y + 0, currentGameData.player.location.x - 1}, true);
            distances[6] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y - 1, currentGameData.player.location.x + 1}, true);
            distances[7] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y - 1, currentGameData.player.location.x + 0}, true);
            distances[8] = findDistanceToClosestEnemy((struct Vec2) {currentGameData.player.location.y - 1, currentGameData.player.location.x - 1}, true);

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
            
            /* If moving would not lead to farther distances, move to tile touching least enemies */
            /* It's just the same algorithm again, sorry */
            if (farthestDistanceIndex == 4) {
                int numberOfEnemies[9];
                int nearByEnemies[8];
                
                findNearbyEnemies((struct Vec2) {currentGameData.player.location.y + 1, currentGameData.player.location.x + 1}, &nearByEnemies, &numberOfEnemies[0]);
                findNearbyEnemies((struct Vec2) {currentGameData.player.location.y + 1, currentGameData.player.location.x + 0}, &nearByEnemies, &numberOfEnemies[1]);
                findNearbyEnemies((struct Vec2) {currentGameData.player.location.y + 1, currentGameData.player.location.x - 1}, &nearByEnemies, &numberOfEnemies[2]);
                findNearbyEnemies((struct Vec2) {currentGameData.player.location.y + 0, currentGameData.player.location.x + 1}, &nearByEnemies, &numberOfEnemies[3]);
                findNearbyEnemies((struct Vec2) {currentGameData.player.location.y + 0, currentGameData.player.location.x + 0}, &nearByEnemies, &numberOfEnemies[4]);
                findNearbyEnemies((struct Vec2) {currentGameData.player.location.y + 0, currentGameData.player.location.x - 1}, &nearByEnemies, &numberOfEnemies[5]);
                findNearbyEnemies((struct Vec2) {currentGameData.player.location.y - 1, currentGameData.player.location.x + 1}, &nearByEnemies, &numberOfEnemies[6]);
                findNearbyEnemies((struct Vec2) {currentGameData.player.location.y - 1, currentGameData.player.location.x + 0}, &nearByEnemies, &numberOfEnemies[7]);
                findNearbyEnemies((struct Vec2) {currentGameData.player.location.y - 1, currentGameData.player.location.x - 1}, &nearByEnemies, &numberOfEnemies[8]);

                int leastNumberIndex = 0;
                do {
                    newLocation = currentGameData.player.location;
                    
                    for (int i = 0; i < 9; i++) {
                        if (numberOfEnemies[i] < numberOfEnemies[leastNumberIndex]) {
                            leastNumberIndex = i;
                        }
                    }

                    switch (leastNumberIndex) {
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

                    numberOfEnemies[leastNumberIndex] = 8; /* If this option is out of bounds or an enemy, make sure we don't pick it again */
                } while ((newLocation.y < 0 || newLocation.y >= MAP_HEIGHT) || (newLocation.x < 0 || newLocation.x >= MAP_WIDTH) || findDistanceToClosestEnemy(newLocation, false) == 0);
            }
            
            break;
        }
        
        case MOVEMENT_RETURN: { /* Attempt to move to terrain matching katana type */
            /* This could be more efficient, I know */
            struct Vec2 closestTerrain = (struct Vec2) {0, 0};
            int closestTerrainDistance = MAP_HEIGHT + MAP_WIDTH;
            for (int y = 0; y < MAP_HEIGHT; y++) {
                for (int x = 0; x < MAP_WIDTH; x++) {
                    if (currentGameData.map[y][x].type == katanaToTerrain[katana.type]) {
                        int currentTerrainDistance = findDistance(currentGameData.player.location, (struct Vec2) {y, x}, false);
                        if (currentTerrainDistance < closestTerrainDistance) {
                            closestTerrain = (struct Vec2) {y, x};
                            closestTerrainDistance = currentTerrainDistance;
                        }
                    }
                }
            }
            newLocation = pathfinding(currentGameData.player.location, closestTerrain, false);
            break;
        }
        
        case MOVEMENT_DEFEND: { /* Don't move */
            break;
        }

        case MOVEMENT_FALLEN: { /* Attempt to move to closest fallen katana */
            /* This too could be more efficient */
            if (currentGameData.currentNumberOfFallenKatanas == 0) {
                break;
            }

            struct Vec2 closestFallenKatana = (struct Vec2) {0, 0};
            int closestFallenKatanaDistance = MAP_HEIGHT + MAP_WIDTH;
            for (int y = 0; y < MAP_HEIGHT; y++) {
                for (int x = 0; x < MAP_WIDTH; x++) {
                    if (currentGameData.map[y][x].hasFallenKatana) {
                        int currentFallenKatanaDistance = findDistance(currentGameData.player.location, (struct Vec2) {y, x}, false);
                        if (currentFallenKatanaDistance < closestFallenKatanaDistance) {
                            closestFallenKatana = (struct Vec2) {y, x};
                            closestFallenKatanaDistance = currentFallenKatanaDistance;
                        }
                    }
                }
            }
            newLocation = pathfinding(currentGameData.player.location, closestFallenKatana, false);
            break;
        }
        
        case MOVEMENT_RAND: { /* Move in random direction */
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

    currentGameData.player.moves++;

    currentGameData.player.location = newLocation;
}

void enemyMovement(int enemyIndex) {
    struct Enemy* currentEnemy = &currentGameData.enemies[enemyIndex];
    struct Vec2 newLocation;

    if (CHECK_BIT(currentGameData.comboFlags, COMBO_FLAG_SCARE_ENEMIES)) {
        newLocation = pathfinding(currentEnemy->location, currentGameData.player.location, true);
    } else {
        newLocation = pathfinding(currentEnemy->location, currentGameData.player.location, false);
    }

    if (checkForEnemy(newLocation) == false && ((newLocation.y >= 0 && newLocation.y <= MAP_HEIGHT) && (newLocation.x >= 0 && newLocation.x <= MAP_WIDTH))) {
        currentEnemy->location = newLocation;
        currentEnemy->lastMovementTurn = currentGameData.turn;
    }
}

/* Attacking */
void attackEnemy(int enemyIndex, struct Katana* katana) {

    double resistancePercent = 1.0;
    if (katanaToEnemyResistance[katana->type] == currentGameData.enemies[enemyIndex].type) {
        resistancePercent = ENEMY_RESISTANCE_PERCENT;
    } 
    if (CHECK_BIT(currentGameData.comboFlags, COMBO_FLAG_FORCE_ATTACK)) {
        resistancePercent = 1.0;
        currentGameData.comboFlags ^= COMBO_FLAG_FORCE_ATTACK;
    } 


    double synergyPercent = 1.0;
    if (katanaToTerrain[katana->type] == currentGameData.map[currentGameData.player.location.y][currentGameData.player.location.x].type) {
        synergyPercent = TERRAIN_SYNERGY_PERCENT;
    } 

    double damage = ((double) (katana->damage + myRand(katana->damageMod)) * resistancePercent * synergyPercent) * ((double) katana->sharpness / 100.0);
    
    if (CHECK_BIT(currentGameData.comboFlags, COMBO_FLAG_DOUBLE_ATTACK)) {
        damage *= 2;
        currentGameData.comboFlags ^= COMBO_FLAG_DOUBLE_ATTACK;
    } 


    currentGameData.enemies[enemyIndex].health -= damage;
    if (currentGameData.enemies[enemyIndex].health <= 0) {
        removeEnemy(enemyIndex);
        katana->numberOfKills++;
    }

    int chanceForSharpnessDecrease = myRand(CHANCE_FOR_SHARPNESS_DECREASE);
    if (chanceForSharpnessDecrease > CHANCE_FOR_SHARPNESS_DECREASE/2) {
        katana->sharpness -= (chanceForSharpnessDecrease - (CHANCE_FOR_SHARPNESS_DECREASE/2));
    }
    if (katana->sharpness < MIN_KATANA_SHARPNESS) {
        katana->sharpness = MIN_KATANA_SHARPNESS;
    }

    currentGameData.player.attacks++;
}

void attackPlayer(int enemyIndex) {
    currentGameData.player.health -= currentGameData.enemies[enemyIndex].damage;
}


/* Generators & Destructors */
void genEnemy(int type, int sideLocation) {
    if (currentGameData.currentNumberOfEnemies < MAX_NUMBER_OF_ENEMIES) {
        struct Enemy *currentEnemy = &currentGameData.enemies[currentGameData.currentNumberOfEnemies];

        currentEnemy->type = type;

        currentEnemy->speed = ENEMY_SPEED_GEN;

        currentEnemy->level = ENEMY_LEVEL_GEN;

        if (currentEnemy->level > ENEMY_LEVEL_CAP) {
            currentEnemy->level = (ENEMY_LEVEL_CAP + 5) - myRand(10);
        }

        currentEnemy->maxHeath = ENEMY_MAX_HEALTH_GEN;
        currentEnemy->health = currentEnemy->maxHeath;

        currentEnemy->damage = ENEMY_DAMAGE_GEN;

        struct Vec2 location = (struct Vec2) {0, 0};

        /* Simple way to avoid infinite loop */
        int tests = 50;
        do {
            tests--;
            switch (sideLocation) {
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
                    ERROR("Invalid location input");
                }
            }
        } while ((checkForEnemy(location) != 0 || findDistance(location, currentGameData.player.location, false) < 8) && tests > 0);

        currentEnemy->location = location;
        currentGameData.currentNumberOfEnemies++;
    }
}

void removeEnemy(int enemyIndex) {
    currentGameData.score += (int) ((float) currentGameData.enemies[enemyIndex].level * (( (float) currentGameData.turn / (float) TURNS_UNTIL_DIFFICULTY_INCREASE) + 1.0)); 

    for (int i = enemyIndex; i < currentGameData.currentNumberOfEnemies - 1; i++) {
        currentGameData.enemies[i] = currentGameData.enemies[i + 1];
    }
    currentGameData.currentNumberOfEnemies--;
    currentGameData.player.enemiesKilled++;
}

void genKatana(struct Katana* katana, int setMovementType) {
    katana->type = myRand(NUMBER_OF_KATANA_TYPES);

    katana->damage = KATANA_DAMAGE_GEN;
    if (katana->damage == 0) {
        katana->damage++;
    }

    katana->damageMod = KATANA_DAMAGE_MOD_GEN;

    if (katana->damageMod > MAX_KATANA_DAMAGE_MOD) {
        katana->damageMod = MAX_KATANA_DAMAGE_MOD;
    }

    if (setMovementType == -1) {
        katana->movementType = myRand(NUMBER_OF_MOVEMENT_TYPES);
    } else {
        katana->movementType = setMovementType;
    }

    katana->sharpness = 100;

    katana->numberOfMoves = 0;
    katana->numberOfStrikes = 0;
    katana->numberOfKills = 0;

    sprintf(katana->name, "%s%s", katanaNameType[katana->type], katanaNameDamage[katana->damage - 1]);


    /* Image */
    int guardType = myRand(NUMBER_OF_KATANA_GUARD_TYPES);
    sprintf(katana->katanaImage[0], "    %s", katanaGuardTypes[guardType][0]);
    sprintf(katana->katanaImage[1], "%s%s%s%s", katanaHiltTypes[myRand(NUMBER_OF_KATANA_HILT_TYPES)], katanaGuardTypes[guardType][1], katanaBladeBodyTypes[myRand(NUMBER_OF_KATANA_BLADE_BODY_TYPES)], katanaBladeTipTypes[myRand(NUMBER_OF_KATANA_BLADE_TIP_TYPES)]);
    sprintf(katana->katanaImage[2], "    %s", katanaGuardTypes[guardType][2]);
}

void genFallenKatana() {
    if (currentGameData.currentNumberOfFallenKatanas >= MAX_NUMBER_OF_FALLEN_KATANAS) {
        return;
    }

    struct Katana fallenKatana;
    genKatana(&fallenKatana, -1);

    struct Vec2 location;
    do {
        location.x = myRand(MAP_WIDTH);
        location.y = myRand(MAP_HEIGHT);
    } while(findDistance(location, currentGameData.player.location, false) < 5 || checkForEnemy(location) || currentGameData.map[location.y][location.x].hasFallenKatana);

    if (myRand(3) == 0) {
        terrainAreaMap(katanaToTerrain[fallenKatana.type], location, myRand(3) + 1);
    } else {
        terrainAreaMap(TERRAIN_GRASS, location, myRand(3) + 1);
    }

    currentGameData.map[location.y][location.x].fallenKatana = fallenKatana;
    currentGameData.map[location.y][location.x].hasFallenKatana = true;
    currentGameData.currentNumberOfFallenKatanas++;
    update("A katana strikes the earth.", true);
}

void genMap() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            currentGameData.map[y][x].type = TERRAIN_GRASS;
            currentGameData.map[y][x].hasFallenKatana = false;
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

void genCombo(int protoCombo) {
    struct Combo combo;
    combo.length = protoCombos[protoCombo].length;
    combo.action = protoCombos[protoCombo].action;
    combo.infoIndex = protoCombo;

    do {
        for (int i = 0; i < combo.length; i++) {
            combo.combo[i] = myRand(4);
        }
    } while (!isViableCombo(combo));

    currentGameData.combos[currentGameData.numberOfCombos] = combo;
    currentGameData.numberOfCombos++;
}

void genWave(struct Wave* wave) {
    wave->difficulty = ((int) (currentGameData.turn / TURNS_UNTIL_DIFFICULTY_INCREASE)) + 1;

    int flag1 = pow(2, myRand(4));
    int flag2 = pow(2, myRand(4));
    int flag3 = pow(2, myRand(4));
    int flag4 = pow(2, myRand(4));

    wave->flags = flag1 + (flag2 << 4) + (flag3 << 8) + (flag4 << 12);

    if (CHECK_BIT(wave->flags, WAVE_FLAG_ONLY_ONE_TYPE)) {
        wave->enemiesToSpawn[myRand(4)] = (wave->difficulty * ENEMY_GEN_PER_DIFFICULTY_INCREASE) + ENEMY_SPAWN_NUMBER_BASE;
    } else if (CHECK_BIT(wave->flags, WAVE_FLAG_ONLY_TWO_TYPES)) {
        int typeOne = myRand(4);
        int typeTwo;
        do {
            typeTwo = myRand(4);
        } while (typeOne == typeTwo);

        wave->enemiesToSpawn[typeOne] = ((wave->difficulty * ENEMY_GEN_PER_DIFFICULTY_INCREASE) + ENEMY_SPAWN_NUMBER_BASE) / 2;
        wave->enemiesToSpawn[typeTwo] = ((wave->difficulty * ENEMY_GEN_PER_DIFFICULTY_INCREASE) + ENEMY_SPAWN_NUMBER_BASE) / 2;

    } else if (CHECK_BIT(wave->flags, WAVE_FLAG_ONLY_THREE_TYPES)) {
        int typeOne = myRand(4);
        int typeTwo;
        int typeThree;
        do {
            typeTwo = myRand(4);
        } while (typeOne == typeTwo);

        do {
            typeThree = myRand(4);
        } while (typeThree == typeOne || typeThree == typeTwo);

        wave->enemiesToSpawn[typeOne] = ((wave->difficulty * ENEMY_GEN_PER_DIFFICULTY_INCREASE) + ENEMY_SPAWN_NUMBER_BASE) / 3;
        wave->enemiesToSpawn[typeTwo] = ((wave->difficulty * ENEMY_GEN_PER_DIFFICULTY_INCREASE) + ENEMY_SPAWN_NUMBER_BASE) / 3;
        wave->enemiesToSpawn[typeThree] = ((wave->difficulty * ENEMY_GEN_PER_DIFFICULTY_INCREASE) + ENEMY_SPAWN_NUMBER_BASE) / 3;
    } else if (CHECK_BIT(wave->flags, WAVE_FLAG_ALL_TYPES)) {
        wave->enemiesToSpawn[0] = ((wave->difficulty * ENEMY_GEN_PER_DIFFICULTY_INCREASE) + ENEMY_SPAWN_NUMBER_BASE) / 4;
        wave->enemiesToSpawn[1] = ((wave->difficulty * ENEMY_GEN_PER_DIFFICULTY_INCREASE) + ENEMY_SPAWN_NUMBER_BASE) / 4;
        wave->enemiesToSpawn[2] = ((wave->difficulty * ENEMY_GEN_PER_DIFFICULTY_INCREASE) + ENEMY_SPAWN_NUMBER_BASE) / 4;
        wave->enemiesToSpawn[3] = ((wave->difficulty * ENEMY_GEN_PER_DIFFICULTY_INCREASE) + ENEMY_SPAWN_NUMBER_BASE) / 4;
    } else {
        ERROR("Wave flag bit error");
    }
    currentGameData.currentNumberOfWaves++;
}

void genPlayerDNA() {
    char nucleotides[62] = "!#$^*()+=-[]{}|\\/:<>XO'~.ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

    for (int i = 0; i < MAP_HEIGHT; i++) {
        for (int j = 0; j < MAP_WIDTH; j++) {
            currentGameData.player.DNA[i][j] = nucleotides[myRand(61)];
        }
        currentGameData.player.DNA[i][MAP_WIDTH] = '\0';
    }
}


/* Combo Functions */
int checkForCombo(){
    for (int i = 0; i < currentGameData.numberOfCombos; i++) {
        bool comboMach = true;
        if (currentGameData.turn - currentGameData.player.turnOfLastCombo >= currentGameData.combos[i].length){
            for (int j = 0; j < currentGameData.combos[i].length; j++) {
                if (currentGameData.combos[i].combo[j] != currentGameData.previousMoves[currentGameData.combos[i].length - j - 1][1]) {
                    comboMach = false;
                }
            }
            if (comboMach){
                currentGameData.player.turnOfLastCombo = currentGameData.turn;
                return i;
            }
        }
    }
    return -1;
}

bool isViableCombo(struct Combo combo) {
    /* New combos can't have existing ones exist in side of them, nor can they exist in existing ones either. */
    /* Otherwise the game wouldn't be able to tell which combo the player was trying to use. */
    /* This function takes care of checking for that */

    for (int i = 0; i < currentGameData.numberOfCombos; i++) {
        int comboLengthDifference = currentGameData.combos[i].length - combo.length;
        if (comboLengthDifference > 0) { /* New combo is smaller */
            for (int j = 0; j < comboLengthDifference + 1; j++) {
                bool comboMach = true;
                for (int k = 0; k < combo.length; k++) {
                    if (combo.combo[k] != currentGameData.combos[i].combo[j + k]) {
                        comboMach = false;
                    }
                }
                if (comboMach) {
                    return false;
                }
            }
        } else if (comboLengthDifference < 0) { /* New combo is bigger */
            for (int j = 0; j < (-comboLengthDifference) + 1; j++) {
                bool comboMach = true;
                for (int k = 0; k < currentGameData.combos[i].length; k++) {
                    if (currentGameData.combos[i].combo[k] != combo.combo[j + k]) {
                        comboMach = false;
                    }
                }
                if (comboMach) {
                    return false;
                }
            }
        } else { /* New combo is the same length */
            bool comboMach = true;
            for (int k = 0; k < combo.length; k++) {
                if (combo.combo[k] != currentGameData.combos[i].combo[k]) {
                    comboMach = false;
                }
            }
            if (comboMach) {
                return false;
            }
        }
    }
    return true;
}

void activateCombo(int comboIndex) {
    if (comboIndex == -1) {
        return;
    }
    switch (currentGameData.combos[comboIndex].action) {
        case 0: { /* Discover */
            if (currentGameData.numberOfCombos > currentGameData.currentNumberOfDiscoveredCombos) { 
                int discoveredCombo;
                bool beenDiscovered;
                do {
                    discoveredCombo = myRand(currentGameData.numberOfCombos);
                    beenDiscovered = false;
                    for (int i = 0; i < currentGameData.currentNumberOfDiscoveredCombos; i++) {
                        if (discoveredCombo == currentGameData.discoveredCombos[i]){
                            beenDiscovered = true;
                        }
                    }
                } while (beenDiscovered);

                currentGameData.discoveredCombos[currentGameData.currentNumberOfDiscoveredCombos] = discoveredCombo;
                currentGameData.currentNumberOfDiscoveredCombos++;
            } else {
                update("You already know all the combos", true);
                return;
            }
            break;
        }

        case 1: { /* Heal */
            currentGameData.player.health += myRand(PLAYER_START_HEALTH / 5);
            if (currentGameData.player.health > PLAYER_START_HEALTH) {
                currentGameData.player.health = PLAYER_START_HEALTH;
            }
            break;
        }

        case 2: { /* Terraform */
            int newTerrain;
            do {
                newTerrain = myRand(NUMBER_OF_TERRAIN_TYPES);
            } while (currentGameData.map[currentGameData.player.location.y][currentGameData.player.location.x].type == newTerrain);
            currentGameData.map[currentGameData.player.location.y][currentGameData.player.location.x].type = newTerrain;
            break;
        }
        
        case 3: { /* Freeze */
            currentGameData.turnsToFreeze = 6;
            break;
        }

        case 4: { /* Swap */
            int firstKatana = myRand(4);
            int secondKatana;
            do {
                secondKatana = myRand(4);
            } while (firstKatana == secondKatana);

            struct Katana temp = currentGameData.player.katanas[firstKatana];
            currentGameData.player.katanas[firstKatana] = currentGameData.player.katanas[secondKatana];
            currentGameData.player.katanas[secondKatana] = temp;
            break;
        }

        case 5: { /* Boost */
            currentGameData.comboFlags = (currentGameData.comboFlags | COMBO_FLAG_DOUBLE_ATTACK);
            break;
        }

        case 6: { /* Teleport */
            struct Vec2 newLocation;
            do {
                newLocation.y = myRand(MAP_HEIGHT);
                newLocation.x = myRand(MAP_WIDTH);
            } while(findDistance(currentGameData.player.location, newLocation, false) < 5 || findDistanceToClosestEnemy(newLocation, false) == 0 || currentGameData.map[newLocation.y][newLocation.x].hasFallenKatana);

            currentGameData.player.location = newLocation;
            break;
        }

        case 7: { /* Force */
            currentGameData.comboFlags = (currentGameData.comboFlags | COMBO_FLAG_FORCE_ATTACK);
            break;
        }

        case 8: { /* Scare */
            currentGameData.comboFlags = (currentGameData.comboFlags | COMBO_FLAG_SCARE_ENEMIES);
            break;
        }

        default: {
            ERROR("Invalid combo index");
        }
    }

    bool beenDiscovered = false;
    for (int i = 0; i < currentGameData.currentNumberOfDiscoveredCombos; i++) {
        if (comboIndex == currentGameData.discoveredCombos[i]) {
            beenDiscovered = true;
        }
    }

    char buffer[50];
    if (beenDiscovered) {
        sprintf(buffer, "%s combo activated", protoCombos[currentGameData.combos[comboIndex].infoIndex].title);
    } else {
        sprintf(buffer, "%s combo discovered", protoCombos[currentGameData.combos[comboIndex].infoIndex].title);
        currentGameData.discoveredCombos[currentGameData.currentNumberOfDiscoveredCombos] = comboIndex;
        currentGameData.currentNumberOfDiscoveredCombos++;
    }

    update(buffer, true);
}


/* Misc Functions */
int getStringInput(int y, int x, bool center, int maxBufferSize, char* buffer) {
    int input;

    buffer[0] = ' ';
    buffer[1] = '\0';

    int currentCharIndex = 0;
    int currentBufferSize = 1;

    do {
        printHorizontalLine((MAP_HEIGHT/2) + 1, 0, MAP_WIDTH, " ");
        if (center) {
            mvprintw(y, (MAP_WIDTH/2) - (strlen(buffer)/2), "%s", buffer);
            attrset(A_STANDOUT);
            mvprintw(y, (MAP_WIDTH/2) - (strlen(buffer)/2) + currentCharIndex, "%c", buffer[currentCharIndex]);
            attrset(A_NORMAL);
        } else {
            mvprintw(y, x, "%s", buffer);
            attrset(A_STANDOUT);
            mvprintw(y, x + currentCharIndex, "%c", buffer[currentCharIndex]);
            attrset(A_NORMAL);
        }

        input = myGetch();
        switch(input) {
            case KEY_ESC: {
                return -1;
            }
            case ACCEPT_KEY: {
                break;
            }
            case LEFT_ARROW_KEY: {
                currentCharIndex--;
                break;
            }
            case RIGHT_ARROW_KEY: {
                currentCharIndex++;
                break;
            }
            case KEY_BACKSPACE: {
                if (currentCharIndex == 0) {
                    break;
                }

                for (int i = currentCharIndex - 1; i < currentBufferSize - 1; i++){
                    buffer[i] = buffer[i + 1];
                }

                currentBufferSize--;
                currentCharIndex--;
                break;
            }
            default: {
                if (currentBufferSize < (maxBufferSize - 1)) {
                    for (int i = currentBufferSize; i > currentCharIndex; i--){
                        buffer[i] = buffer[i - 1];
                    }

                    buffer[currentCharIndex] = input;

                    currentBufferSize++;
                    currentCharIndex++;
                }
                break;
            }
        }

        if (currentCharIndex < 0) {
            currentCharIndex = 0;
        } else if (currentCharIndex >= currentBufferSize) {
            currentCharIndex = currentBufferSize - 1;
        }
        buffer[currentBufferSize] = '\0';

    } while(input != ACCEPT_KEY);

    buffer[currentBufferSize - 1] = '\0';

    return 1;
}

bool doesEnemyMoveThisTurn(int enemy) {
    bool enemyToMove;
    if (currentGameData.enemies[enemy].speed < 3) {
        enemyToMove = (currentGameData.turn % (5 - currentGameData.enemies[enemy].speed) != 0);
    } else {
        enemyToMove = (currentGameData.turn - currentGameData.enemies[enemy].lastMovementTurn >= (currentGameData.enemies[enemy].speed - 1));
    }
    return enemyToMove;
}

bool checkForEnemy(struct Vec2 location) {
    for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
        if (currentGameData.enemies[i].location.y == location.y && currentGameData.enemies[i].location.x == location.x) {
            return 1;
        }
    }
    return 0;
}

void replaceKatana(int slot, struct Katana katana) {
    currentGameData.player.katanas[slot] = katana;
}

void pickUpFallenKatana() {
    struct Katana fallenKatana = currentGameData.map[currentGameData.player.location.y][currentGameData.player.location.x].fallenKatana;
    printKatanaDescription(fallenKatana, true);

    char buffer[60];

    sprintf(buffer, "Select a katana to switch, \"%c\" to break it for health.", BREAK_KATANA_KEY);

    update(buffer, false);

    int command;
    bool validAction;
    do {
        validAction = true;
        command = myGetch();
        switch (command) {
            case TOP_LEFT_KATANA:
                replaceKatana(0, fallenKatana);
                currentGameData.player.katanasPickedUp++;
                break;
            case TOP_RIGHT_KATANA:
                replaceKatana(1, fallenKatana);
                currentGameData.player.katanasPickedUp++;
                break;
            case BOTTOM_LEFT_KATANA:
                replaceKatana(2, fallenKatana);
                currentGameData.player.katanasPickedUp++;
                break;
            case BOTTOM_RIGHT_KATANA:
                replaceKatana(3, fallenKatana);
                currentGameData.player.katanasPickedUp++;
                break;
            case BREAK_KATANA_KEY: /* Health boost */
                currentGameData.player.health += fallenKatana.damage * 2 + (myRand(currentGameData.turn / TURNS_UNTIL_DIFFICULTY_INCREASE) * 5);
                if (currentGameData.player.health > PLAYER_START_HEALTH) {
                    currentGameData.player.health = PLAYER_START_HEALTH;
                }
                currentGameData.player.katanasBroken++;
                break;
            default:
                validAction = false;
                break;
        }
    } while(!validAction);

    currentGameData.map[currentGameData.player.location.y][currentGameData.player.location.x].hasFallenKatana = false;
    currentGameData.currentNumberOfFallenKatanas--;
}

void enemyWave() {
    int totalEnemies = 0 ;
    for (int i = 0; i < NUMBER_OF_ENEMY_TYPES; i++) {
        totalEnemies += currentGameData.currentWave.enemiesToSpawn[i];
    }
    
    if (totalEnemies == 0 && currentGameData.currentNumberOfEnemies == 0) {
        genWave(&currentGameData.currentWave);
        char buffer[20];
        sprintf(buffer, "Wave %i approaches.", currentGameData.currentNumberOfWaves);
        update(&buffer[0], true);
    }

    int turnDelay;
    if (CHECK_BIT(currentGameData.currentWave.flags, WAVE_FLAG_2_TURN_DELAY)) {
        turnDelay = 2;
    } else if (CHECK_BIT(currentGameData.currentWave.flags, WAVE_FLAG_5_TURN_DELAY)) {
        turnDelay = 5;
    } else if (CHECK_BIT(currentGameData.currentWave.flags, WAVE_FLAG_7_TURN_DELAY)) {
        turnDelay = 7;
    } else if (CHECK_BIT(currentGameData.currentWave.flags, WAVE_FLAG_10_TURN_DELAY)) {
        turnDelay = 10;
    }

    if (currentGameData.turnOfLastEnemySpawn + turnDelay < currentGameData.turn) {
        currentGameData.turnOfLastEnemySpawn = currentGameData.turn;

        int numberOfEnemiesToSpawn;

        if (CHECK_BIT(currentGameData.currentWave.flags, WAVE_FLAG_ALL_AT_ONCE)) {
            numberOfEnemiesToSpawn = totalEnemies;
        } else if (CHECK_BIT(currentGameData.currentWave.flags, WAVE_FLAG_ONE_AT_A_TIME)) {
            numberOfEnemiesToSpawn = 1;
        } else if (CHECK_BIT(currentGameData.currentWave.flags, WAVE_FLAG_TWO_AT_A_TIME)) {
            numberOfEnemiesToSpawn = 2;
        } else if (CHECK_BIT(currentGameData.currentWave.flags, WAVE_FLAG_RAND_AT_A_TIME)) {
            numberOfEnemiesToSpawn = 1 + myRand(4);
        } else {
            ERROR("Wave flag bit error");
        }

        while (numberOfEnemiesToSpawn > totalEnemies) {
            numberOfEnemiesToSpawn--;
        } 

        int locations[4] = {-1, -1, -1, -1};

        locations[0] = myRand(4);
        if (!CHECK_BIT(currentGameData.currentWave.flags, WAVE_FLAG_ONE_SIDE_SPAWN)) {
            do {
                locations[1] = myRand(4);
            } while (locations[0] == locations[1]);
        }
        if (!CHECK_BIT(currentGameData.currentWave.flags, WAVE_FLAG_ONE_SIDE_SPAWN) && !CHECK_BIT(currentGameData.currentWave.flags, WAVE_FLAG_TWO_SIDES_SPAWN)) {
            do {
                locations[2] = myRand(4);
            } while (locations[0] == locations[2] || locations[1] == locations[2]);
        }
        if (CHECK_BIT(currentGameData.currentWave.flags, WAVE_FLAG_FOUR_SIDES_SPAWN)) {
            do {
                locations[3] = myRand(4);
            } while (locations[0] == locations[3] || locations[1] == locations[3] || locations[2] == locations[3]);
        }

        if (locations[0] + locations[1] + locations[2] + locations[3] == -4) {
            ERROR("Wave flag bit error");
        }

        for (int i = 0; i < numberOfEnemiesToSpawn; i++) {
            int type;
            do {
                type = myRand(NUMBER_OF_ENEMY_TYPES);
            } while (currentGameData.currentWave.enemiesToSpawn[type] == 0);

            int location;
            do {
                location = myRand(4);
            } while (locations[location] == -1); 

            genEnemy(type, locations[location]);

            currentGameData.currentWave.enemiesToSpawn[type]--;
        }
    }
}

int menu(char options[][50], int numberOfOptions, int yOffset, int xOffset) {
    int currentSelection = 0;

    attron(COLOR_PAIR(COLOR_WHITE));
    attrset(A_NORMAL);

    while (true) {
        for (int i = 0; i < numberOfOptions; i++) {
            if (currentSelection == i) {
                attron(A_STANDOUT);
            }
            mvprintw(i + yOffset, xOffset, "%s", options[i]);
            attrset(A_NORMAL);
        }

        int input = myGetch();
        switch (input) {
            case UP_ARROW_KEY:
                currentSelection--;
                break;
            
            case DOWN_ARROW_KEY:
                currentSelection++;
                break;

            case ACCEPT_KEY:
                return currentSelection;

            case QUIT_KEY:
                return -1;

            default:
                break;
        }
        
        if (currentSelection < 0) {
            currentSelection = 0;
        } else if (currentSelection >= numberOfOptions) {
            currentSelection = numberOfOptions - 1;
        }
    }
}

void saveGame(FILE* fileToSaveTo) {
    fwrite(&currentGameData, sizeof(struct GameData), 1, fileToSaveTo);
    fclose(fileToSaveTo);
}

int loadGame(FILE* fileToLoadFrom) {
    currentGameData.saveCheck = 0;
    fread(&currentGameData, sizeof(struct GameData), 1, fileToLoadFrom);
    fclose(fileToLoadFrom);

    if (currentGameData.saveCheck != 572) {
        return -1;
    }

    return 1; 
}

void saveScore() {
    FILE* scoresFile;

    scoresFile = fopen("Scores","a");
    fprintf(scoresFile,"%s - %i\n", currentGameData.player.name, currentGameData.score);
    fclose(scoresFile);
}

void genRandomName(char name[20], bool isSurname, bool gender){
    /* Simple name generator */
    /* Based off of: http://arns.freeservers.com/workshop38.html */
    char vowels[] = "aaaeeeiiou";
    int vowelsLen = 10;
    char consonants[] = "bbbcdddffgghjkllmmmnnnppqrrssstttvwxyz";
    int consonantsLen = 38;

    name[0] = '\0';

    if (myRand(2) == 0){
        strncat(name, &consonants[myRand(consonantsLen)], 1);
    }

    int nameLen = myRand(3) + 1;
    for (int i = 0; i < nameLen; i++){
        strncat(name, &vowels[myRand(vowelsLen)], 1);
        strncat(name, &consonants[myRand(consonantsLen)], 1);
    }

    char maleGenderEnding[] = "us";
    char femaleGenderEnding[] = "a";

    if (!isSurname){
        strcat(name, gender ? femaleGenderEnding : maleGenderEnding);
    }

    name[0] = toupper(name[0]);
}


/* Utility Functions */
int myRand(int number) {
    if (number > 0) {
        return rand() % number;
    }
    return 0;
}

int dice(int number, int sides) { 
    /* Roll "number" dice with sides 0-"sides" and add all the numbers up */
    sides++;
    int total = 0;
    for (int i = 0; i < number; i++) {
        total += myRand(sides);
    }
    return total;
}

void printError(char *message, char *file, int line) {
    stopCurses();
    printf("In file: %s, line: %i\n", file, line);
    printf("   Error: %s\n", message);
    exit(-1);
} 

void pushPreviousMove(int type, int location, int movement) {
    for (int i = HISTORY_LENGTH - 1; i > 0; i--) {
        currentGameData.previousMoves[i][0] = currentGameData.previousMoves[i - 1][0];
        currentGameData.previousMoves[i][1] = currentGameData.previousMoves[i - 1][1];
        currentGameData.previousMoves[i][2] = currentGameData.previousMoves[i - 1][2];
    }
    currentGameData.previousMoves[0][0] = type;
    currentGameData.previousMoves[0][1] = location;
    currentGameData.previousMoves[0][2] = movement;
}

void sliceInsertString(char* expression, char* insert, int location, int replacementLen){
    char* oldExpression = (char*) malloc(strlen(expression) * sizeof(char) * 2);
    strcpy(oldExpression, expression);
    expression[location] = '\0';
    strcat(expression, insert);
    strcat(expression, &oldExpression[location + replacementLen]);
    free(oldExpression);
}

void formatBlock(char* oldString, char* newString, int lineLength) {
    int stringLength = strlen(oldString);
    int temp = 0;
    int offset = 0;

    for (int i = 0; i < stringLength; i++){
        newString[i] = oldString[i];
    }

    for (int i = 0; i < stringLength + offset; i++){
        if ((i + 1) % lineLength == 0){
            if (newString[i] != ' ' && newString[i + 1] != ' '){
                temp = i;
                while (newString[temp] != ' '){
                    temp--;
                }
                for (int j = temp; j < i; j++){
                    sliceInsertString(newString, " ", j, 0);
                }
                offset += (i - temp);
            }
        }
    }
    newString[stringLength + offset] = '\0';
}

FILE* writeFile(char* filePath) {
    FILE* file = fopen(filePath, "wb");
    return file;
}

FILE* readFile(char* filePath) {
    FILE* file = fopen(filePath, "rb");
    return file;
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

    #if USE_COLOR
        initColor();
    #endif
}

void stopCurses(){
    /* Gracefully stop curses */
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
    for (int i = y; i <= stopY; i++){
        printHorizontalLine(i, x, stopX, toPrint);
    }
}

void update(char* message, bool pause){
    char* formattedMessage = (char *) malloc(strlen(message) * sizeof(char) * 2);

    int messageLineLength = SCREEN_WIDTH - 18;

    formatBlock(message, formattedMessage, messageLineLength);

    char mainBuffer[SCREEN_WIDTH - 2];
    char secondaryBuffer[messageLineLength + 1];

    int linesNeeded = (int) (strlen(formattedMessage) / (messageLineLength)) + 1;

    for (int i = 0; i < linesNeeded; i++) {
        strncpy(secondaryBuffer, &formattedMessage[i * messageLineLength], messageLineLength);
        secondaryBuffer[messageLineLength] = '\0';
        if (i != linesNeeded - 1) {
            sprintf(mainBuffer, "%s -cont- (exit)", secondaryBuffer);
        } else if (pause) {
            sprintf(mainBuffer, "%s (exit)", secondaryBuffer);
        } else {
            sprintf(mainBuffer, "%s", secondaryBuffer);
        }
        if (pause) {
            printHorizontalLine(SCREEN_HEIGHT / 3, 1, SCREEN_WIDTH - 2, " ");
            mvprintw(SCREEN_HEIGHT / 3, (SCREEN_WIDTH / 2) - (strlen(mainBuffer) / 2), mainBuffer);
            myGetch();
        } else {
            printHorizontalLine(SCREEN_HEIGHT / 3, 1, SCREEN_WIDTH - 2, " ");
            mvprintw(SCREEN_HEIGHT / 3, (SCREEN_WIDTH / 2) - (strlen(mainBuffer) / 2), mainBuffer);
        }
    }

    free(formattedMessage);

    if (pause) {
        printBoarder(true);
    }
}

void printBoarder(bool printMiddle){
    printVerticalLine(0, 0, SCREEN_HEIGHT - 1, "|");
    printVerticalLine(SCREEN_WIDTH - 1, 0, SCREEN_HEIGHT - 1, "|");

    printHorizontalLine(0, 0, SCREEN_WIDTH - 1, "-");

    if (printMiddle) {
        double healthAsIcons;
        /* Player health */
        attron(COLOR_PAIR(COLOR_RED));
        healthAsIcons = (( ((double) currentGameData.player.health) / ((double) PLAYER_START_HEALTH)) * (((double) SCREEN_WIDTH) / 2));
        if (healthAsIcons > 0 && healthAsIcons < 1) {
            healthAsIcons = 1;
        }

        printHorizontalLine(SCREEN_HEIGHT / 3, 1, (SCREEN_WIDTH / 2) - 1, "-");
        printHorizontalLine(SCREEN_HEIGHT / 3, (SCREEN_WIDTH / 2) - (int) healthAsIcons - 1, (SCREEN_WIDTH / 2) - 1, "=");

        /* Enemy health */
        attron(COLOR_PAIR(COLOR_MAGENTA));
        if (currentGameData.currentEnemyToAttack != -1) {
            healthAsIcons = (( ((double) currentGameData.enemies[currentGameData.currentEnemyToAttack].health) / ((double) currentGameData.enemies[currentGameData.currentEnemyToAttack].maxHeath)) * (((double) SCREEN_WIDTH) / 2));
            if (healthAsIcons > 0 && healthAsIcons < 1) {
                healthAsIcons = 1;
            }
        } else {
            healthAsIcons = (SCREEN_WIDTH / 2);
        }
        printHorizontalLine(SCREEN_HEIGHT / 3, (SCREEN_WIDTH / 2), SCREEN_WIDTH - 1, "-");
        printHorizontalLine(SCREEN_HEIGHT / 3, (SCREEN_WIDTH / 2), (SCREEN_WIDTH / 2) + (int) healthAsIcons - 1, "=");

        attrset(A_NORMAL);
    }

    printHorizontalLine(SCREEN_HEIGHT - 1, 0, SCREEN_WIDTH - 1, "-");

    mvprintw(0, 0, "+");
    mvprintw(0, SCREEN_WIDTH - 1, "+");

    if (printMiddle) {
        mvprintw(SCREEN_HEIGHT / 3, 0, "+");
        mvprintw(SCREEN_HEIGHT / 3, SCREEN_WIDTH - 1, "+");
    }

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
    
    sprintf(buffer,"Damage: %i", katana.damage);
    mvprintw(topLeftCorner.y + 3, (topLeftCorner.x + (katanaInfoBoxWidth/2)) - (strlen(buffer)/2), buffer);

    if (katana.damageMod != 0) {
        sprintf(buffer, "Damage Mod: %i", katana.damageMod);
        mvprintw(topLeftCorner.y + 4, (topLeftCorner.x + (katanaInfoBoxWidth/2)) - (strlen(buffer)/2), buffer);
    }

    sprintf(buffer, "Movement Type: %s", katanaMovementTypeNames[katana.movementType]);
    mvprintw(topLeftCorner.y + 5, (topLeftCorner.x + (katanaInfoBoxWidth/2)) - (strlen(buffer)/2), buffer);
}

void printKatanaDescription(struct Katana katana, bool fallenKatana) {
    char buffer[MAP_WIDTH];
    char damageModBuffer[20];

    printBox(1, 1, MAP_HEIGHT, MAP_WIDTH, " ");
    
    attron(COLOR_PAIR(katanaColor[katana.type]));

    mvprintw(1, 1, katanaCornerIcon[katana.type]);
    mvprintw(1, MAP_WIDTH, katanaCornerIcon[katana.type]);
    mvprintw(MAP_HEIGHT, 1, katanaCornerIcon[katana.type]);
    mvprintw(MAP_HEIGHT, MAP_WIDTH, katanaCornerIcon[katana.type]);

    sprintf(buffer, "%s (%s)", katana.name, katanaType[katana.type]);
    mvprintw(1, (MAP_WIDTH/2) - (strlen(buffer)/2), buffer);

    attrset(A_NORMAL);

    if (katana.damageMod != 0) {
        sprintf(damageModBuffer,"  Damage Mod: %i  ", katana.damageMod);
    } else {
        sprintf(damageModBuffer," ");
    }
    
    sprintf(buffer,"Damage: %i  %s  Movement Type: %s", katana.damage, damageModBuffer, katanaMovementTypeNames[katana.movementType]);
    mvprintw(2, (MAP_WIDTH/2) - (strlen(buffer)/2), buffer);

    if (!fallenKatana) {
        sprintf(buffer, "Sharpness: %i", katana.sharpness);
        mvprintw(4, 2, buffer);

        sprintf(buffer, "Movement uses: %i", katana.numberOfMoves);
        mvprintw(4, MAP_WIDTH - 20, buffer);

        sprintf(buffer, "Strikes: %i", katana.numberOfStrikes);
        mvprintw(5, MAP_WIDTH - 20, buffer);

        sprintf(buffer, "Kills: %i", katana.numberOfKills);
        mvprintw(6, MAP_WIDTH - 20, buffer);
    }

    attron(COLOR_PAIR(katanaColor[katana.type]));

    int imageHeightOffset = (MAP_HEIGHT / 3) * 2;
    int imageWidthOffset = (MAP_WIDTH/2) - (strlen(katana.katanaImage[1])/2);

    mvprintw(imageHeightOffset, imageWidthOffset, katana.katanaImage[0]);
    mvprintw(imageHeightOffset + 1, imageWidthOffset, katana.katanaImage[1]);
    mvprintw(imageHeightOffset + 2, imageWidthOffset, katana.katanaImage[2]);

    attrset(A_NORMAL);
}

void printMap() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (!currentGameData.map[y][x].hasFallenKatana) {
                attron(COLOR_PAIR(terrainColor[currentGameData.map[y][x].type]));
                mvprintw(y + 1, x + 1, terrainIcon[currentGameData.map[y][x].type]);
            } else {
                attron(COLOR_PAIR(katanaColor[currentGameData.map[y][x].fallenKatana.type]));
                mvprintw(y + 1, x + 1, "!");
            }
        }
    }
    attrset(A_NORMAL);
}

void printPlayerData() {
    char buffer[MAP_WIDTH];
    printBox(1, 1, MAP_HEIGHT, MAP_WIDTH, " ");
    for (int i = 0; i < MAP_HEIGHT; i++) {
        mvprintw(1 + i, 1, "%s", currentGameData.player.DNA[i]);
    }
    
    attron(COLOR_PAIR(COLOR_CYAN));
    mvprintw(2, (MAP_WIDTH/2) - (strlen(currentGameData.player.name)/2), "%s", currentGameData.player.name);

    attron(COLOR_PAIR(COLOR_MAGENTA));
    sprintf(buffer, "%i", currentGameData.score);

    mvprintw(3, (int) ((MAP_WIDTH * 0.5) - (strlen(buffer)/2)), "%s", buffer);

    attron(COLOR_PAIR(COLOR_RED));
    mvprintw(4, (int) (MAP_WIDTH * 0.25), "Hp:%i", currentGameData.player.health);
    mvprintw(4, (int) (MAP_WIDTH * 0.75), "Turn:%i", currentGameData.turn);

    attron(COLOR_PAIR(terrainColor[currentGameData.map[currentGameData.player.location.y][currentGameData.player.location.x].type]));
    mvprintw(5, (int) (MAP_WIDTH/2) - 1, "(%s)", terrainIcon[currentGameData.map[currentGameData.player.location.y][currentGameData.player.location.x].type]);
    
    attrset(A_NORMAL);
}

void printHelp() {
    clearScreen();
    for (int i = 0; i < NUMBER_OF_QUICK_HELP_LINES; i++) {
        mvprintw(i + 2, 1, quickHelpText[i]);
    }
    printBoarder(false);
}

void printComboReference() {
    clearScreen();

    mvprintw(2, (MAP_WIDTH / 2) - 6, "Known combos:");
    for (int i = 0; i < currentGameData.currentNumberOfDiscoveredCombos; i++) {
        mvprintw((i * 2) + 4, 4, "%s", protoCombos[currentGameData.combos[currentGameData.discoveredCombos[i]].infoIndex].title);

        char buffer[MAP_WIDTH] = "";

        for (int j = 0; j < currentGameData.combos[currentGameData.discoveredCombos[i]].length; j++) {
            if (j < currentGameData.combos[currentGameData.discoveredCombos[i]].length - 1) {
                buffer[j * 2] = comboLocationToText[currentGameData.combos[currentGameData.discoveredCombos[i]].combo[j]];
                buffer[j * 2 + 1] = '-';
            } else {
                buffer[j * 2] = comboLocationToText[currentGameData.combos[currentGameData.discoveredCombos[i]].combo[j]];
                buffer[j * 2 + 1] = '\0';
            }
        }

        mvprintw((i * 2) + 4, MAP_WIDTH - 4 - strlen(buffer), "%s", buffer);
        mvprintw((i * 2) + 5, 8, protoCombos[currentGameData.combos[currentGameData.discoveredCombos[i]].infoIndex].description);
    }

    printBoarder(false);
}

void printEnemyInfo() {
    int closestEnemy = findClosestEnemy();

    char buffer[50];

    sprintf(buffer, "Hp:%i Lv:%i", currentGameData.enemies[closestEnemy].health, currentGameData.enemies[closestEnemy].level);
    if (currentGameData.currentNumberOfEnemies == 0) {
        strcpy(buffer, "You see no enemies");
    } else {
        attron(A_STANDOUT);
        attron(COLOR_PAIR(enemyColor[currentGameData.enemies[closestEnemy].type]));
        if (currentGameData.enemies[closestEnemy].level > ((currentGameData.turn / TURNS_UNTIL_DIFFICULTY_INCREASE) * 4)) {
            mvprintw(currentGameData.enemies[closestEnemy].location.y + 1, currentGameData.enemies[closestEnemy].location.x + 1, enemyIcon[currentGameData.enemies[closestEnemy].type]);
        } else {
            mvprintw(currentGameData.enemies[closestEnemy].location.y + 1, currentGameData.enemies[closestEnemy].location.x + 1, enemyIcon[currentGameData.enemies[closestEnemy].type + NUMBER_OF_ENEMY_TYPES]);
        }
        attrset(A_NORMAL);
    }
    printHorizontalLine(SCREEN_HEIGHT / 3, 1, SCREEN_WIDTH - 2, " ");
    mvprintw(SCREEN_HEIGHT / 3, (SCREEN_WIDTH / 2) - (strlen(buffer) / 2), buffer);
}

void printEntities() {
    for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
        attron(COLOR_PAIR(enemyColor[currentGameData.enemies[i].type]));
        if (currentGameData.enemies[i].level > ((currentGameData.turn / TURNS_UNTIL_DIFFICULTY_INCREASE) * 4) ) {
            /* Uppercase */
            mvprintw(currentGameData.enemies[i].location.y + 1, currentGameData.enemies[i].location.x + 1, enemyIcon[currentGameData.enemies[i].type]);
        } else {
            mvprintw(currentGameData.enemies[i].location.y + 1, currentGameData.enemies[i].location.x + 1, enemyIcon[currentGameData.enemies[i].type + NUMBER_OF_ENEMY_TYPES]);
        }
    }

    attron(COLOR_PAIR(COLOR_WHITE));
    mvprintw(currentGameData.player.location.y + 1, currentGameData.player.location.x + 1, "@");
    attrset(A_NORMAL);
}

void printTileCard(){
    int start = (SCREEN_WIDTH/2) - (strlen(titleCard[0])/2);

    for (int i = 0; i < NUMBER_OF_TITLE_CARD_LINES; i++) {
        attrset(COLOR_PAIR(myRand(7)));
        mvprintw(1 + i, start, "%s", titleCard[i]);
    }
}

void printScoresFromScoresFile() {
    int scoresValue[100];

    char scores[100][50];

    FILE* scoresFile;
    char* line = NULL;
    int lineNumber = 0;
    size_t length = 0;
    ssize_t read;

    scoresFile = fopen("Scores","r");

    while ((read = getline(&line, &length, scoresFile)) != -1) {
        strcpy(&scores[lineNumber][0], line);
        strtok(line, " - ");
        scoresValue[lineNumber] = atoi(strtok(NULL, " - "));
        lineNumber++;

        if (lineNumber > 100) {
            ERROR("Play something else");
        }
    }

    int highestScore = 0;
    int highestScoreIndex = 0;

    clearScreen();

    for (int i = 0; i < lineNumber; i++) {
        for (int j = 0; j < lineNumber; j++) {
            if (scoresValue[j] > highestScore) {
                highestScore = scoresValue[j];
                highestScoreIndex = j;
            }
        }
        mvprintw(i + 1, (MAP_WIDTH/2 - strlen(scores[highestScoreIndex])/2), "%s", scores[highestScoreIndex]);
        
        scoresValue[highestScoreIndex] = 0;
        highestScore = 0;

        if (i > (SCREEN_HEIGHT - 4)) {
            break;
        }
    }

    printBoarder(false);
    getch();
}

void printDeathScreen() {
    clearScreen();
    printBoarder(false);

    mvprintw(2, (SCREEN_WIDTH/2) - 3, "Death");

    char buffer[500];
    sprintf(buffer, "%s died on turn %i on wave %i after scoring %i points. They were holding a %s, %s, %s, and %s. They attacked a total of %i times and killed a total of %i enemies. They moved %i times. They picked up %i katana%sand broke %i of them. They discoverd %i out of %i combos. May they rest in piece.",
                    currentGameData.player.name, 
                    currentGameData.turn, 
                    currentGameData.currentNumberOfWaves, 
                    currentGameData.score, 
                    currentGameData.player.katanas[0].name, 
                    currentGameData.player.katanas[1].name, 
                    currentGameData.player.katanas[2].name, 
                    currentGameData.player.katanas[3].name,
                    currentGameData.player.attacks,
                    currentGameData.player.enemiesKilled,
                    currentGameData.player.moves,
                    currentGameData.player.katanasPickedUp,
                    (currentGameData.player.katanasPickedUp==1 ?" ":"s "),
                    currentGameData.player.katanasBroken,
                    currentGameData.currentNumberOfDiscoveredCombos - 1,
                    currentGameData.numberOfCombos - 1);

    char formattedBuffer[500];

    formatBlock(&buffer[0], &formattedBuffer[0], SCREEN_WIDTH - 2);

    for (int i = 0; i < SCREEN_HEIGHT - 4; i++) {
        for (int j = 0; j < (SCREEN_WIDTH - 2); j++) {
            if (formattedBuffer[(i * (SCREEN_WIDTH - 2)) + j] == 0) {
                getch();
                return;
            }
            mvprintw(i + 4, j + 1 , "%c", formattedBuffer[(i * (SCREEN_WIDTH - 2)) + j]);
        }
    }
}