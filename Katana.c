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
#include <unistd.h>          /* Misc                                                                       */
#include <pdcurses/curses.h> /* Libraray for terminal manipulation                                         */
#include "Katana.h"          /* Katana variables, arrays and structures                                    */
/*---- Main Function --------------------------------------------------------------------------------------*/


void main(){
    long seed = time(NULL);
    srand(seed);

    initCurses();


    currentGameData.turn = 0;
    currentGameData.turnsToFreeze = 0;
    currentGameData.numberOfCombos = 0;
    currentGameData.comboFlags = 0;

    currentGameData.saveCheck = 572;

    for (int i = 0; i < HISTORY_LENGTH; i++) {
        currentGameData.previousMoves[i][0] = -1;
        currentGameData.previousMoves[i][1] = -1;
        currentGameData.previousMoves[i][2] = -1;
    }

    strcpy(currentGameData.player.name, "Tanjiro"); // Gotta have that Demon Slayer reference
    currentGameData.player.location = (struct Vec2) {MAP_HEIGHT/2, MAP_WIDTH/2};
    currentGameData.player.health = PLAYER_START_HEALTH;
    currentGameData.player.turnOfLastCombo = 0;
    genKatana(&currentGameData.player.katanas[0]);
    genKatana(&currentGameData.player.katanas[1]);
    genKatana(&currentGameData.player.katanas[2]);
    genKatana(&currentGameData.player.katanas[3]);



    genCombo(0);
    currentGameData.currentNumberOfDiscoveredCombos = 1;
    currentGameData.discoveredCombos[0] = 0;

    int numberOfCombosInGame = 5 + myRand(2);
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

    for (int i = 0; i < 10; i++) {
        //genEnemy();
        genFallenKatana();
    }

    mainMenu();
    stopCurses();
}

void mainMenu(){
    printBoarder(false);
    printTilecard();

    int choice = menu(menuOptions, NUMBER_OF_MENU_OPTIONS, NUMBER_OF_TITLE_CARD_LINES + 2, (SCREEN_WIDTH/2) - (strlen(menuOptions[0])/2));

    switch (choice) {
        case 0: {
            gameLoop();
            break;
        }

        case 1: {
            char buffer[100];
            clearScreen();
            do {
                mvprintw(MAP_HEIGHT/2, (MAP_WIDTH/2) - 10, "Enter valid filepath");
                if (getStringInput((MAP_HEIGHT/2) + 1, 0, true, &buffer[0]) == -1) {
                    mainMenu();
                    return;
                }

                mvprintw(MAP_HEIGHT/2 + 4, (MAP_WIDTH/2) - 14, "File is not valid save file");
            } while (loadGame(readFile(buffer)) != 1);

            clearScreen();

            gameLoop();
            break;
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
            case TOP_LEFT_KATANA: {
                selectedKatana = 0;
                break;
            }
            case TOP_LEFT_KATANA_SECONDARY: {
                selectedKatana = 0;
                infoKey = true;
                break;
            }
            case TOP_RIGHT_KATANA: {
                selectedKatana = 1;
                break;
            }
            case TOP_RIGHT_KATANA_SECONDARY: {
                selectedKatana = 1;
                infoKey = true;
                break;
            }
            case BOTTOM_LEFT_KATANA: {
                selectedKatana = 2;
                break;
            }
            case BOTTOM_LEFT_KATANA_SECONDARY: {
                selectedKatana = 2;
                infoKey = true;
                break;
            }
            case BOTTOM_RIGHT_KATANA: {
                selectedKatana = 3;
                break;
            }
            case BOTTOM_RIGHT_KATANA_SECONDARY: {
                selectedKatana = 3;
                infoKey = true;
                break;
            }

            case HELP_KEY: {
                infoKey = true;
                break;
            } 

            case COMBO_REFERNCE_KEY: {
                infoKey = true;
                break;
            }

            case ENEMY_CHECK_KEY: {
                infoKey = true;
                break;
            }

            default: {
                validMove = false;
            }
        }

        if (validMove && !noActionCommand && !infoKey) {
            enemyWave();
            pushPreviousMove(currentGameData.player.katanas[selectedKatana].type, selectedKatana, currentGameData.player.katanas[selectedKatana].movementType);


            if (CHECK_BIT(currentGameData.comboFlags, COMBO_FLAG_SCARE_ENEMIES)) {
                currentGameData.comboFlags ^= COMBO_FLAG_SCARE_ENEMIES;
            }

            activateCombo(checkForCombo());

            currentGameData.currentEnemyToAttack = -1;
            if (selectedKatana != -1) {
                int nearByEnemies[8];
                int number;

                /* If enemy is nearby and a retreat move has not been used twice in a row */
                if (findNearbyEnemies(&nearByEnemies, &number) && !(currentGameData.previousMoves[2][0] == MOVEMENT_RETREAT && currentGameData.player.katanas[selectedKatana].movementType == MOVEMENT_RETREAT)) {
                
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
                }
            }

            if (currentGameData.turnsToFreeze == 0) {
                for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
                    if (doesEnemyMoveThisTurn(i)) {
                        if (findDistance(currentGameData.enemies[i].location, currentGameData.player.location) == 1) {
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

        if (infoKey) {
            if (selectedKatana != -1) {
                printKatanaDescription(currentGameData.player.katanas[selectedKatana], false);
            } else if (command == HELP_KEY) {
                printHelp();
            } else if (command == COMBO_REFERNCE_KEY) {
                printComboReference();
            } else if (command == ENEMY_CHECK_KEY) {
                printEnemyInfo();
            } else {
                ERROR("Invalid info key");
            }
        }
        
        //mvprintw(30, 0, "i:%i", currentGameData.currentWave.flags);
        //mvprintw(26, 0, "Max:%i", currentGameData.numberOfCombos);
        //mvprintw(27, 0, "Current:%i", currentGameData.currentNumberOfDiscoveredCombos);

        command = myGetch();
    } while (command != QUIT_KEY && currentGameData.player.health > 0);
    
    clearScreen();
    if (currentGameData.player.health > 0) {
        char buffer[100];

        mvprintw(MAP_HEIGHT/2, (MAP_WIDTH/2) - 10, "Enter save filename");
        
        if (getStringInput((MAP_HEIGHT/2) + 1, 0, true, &buffer[0]) == -1) {
            return;
        }

        saveGame(writeFile(buffer));
    } else {
        mvprintw(MAP_HEIGHT/2, (MAP_WIDTH/2) - 3, "Death");
        myGetch();
    }
}

/* Find functions */


double findDistance(struct Vec2 start, struct Vec2 end) {
    return fmax(abs(end.y - start.y), abs(end.x - start.x));
}

int findClosestEnemy(){
    int closestEnemy = 0;
    int closestEnemyDistance = MAP_HEIGHT + MAP_WIDTH;
    for (int i = 0; i < currentGameData.currentNumberOfEnemies; i++) {
        int currentEnemyDistance = findDistance(currentGameData.player.location, currentGameData.enemies[i].location);
        if (currentEnemyDistance < closestEnemyDistance) {
            closestEnemy = i;
            closestEnemyDistance = currentEnemyDistance;
        }
    }
    return closestEnemy;
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

struct Vec2 pathFinding(struct Vec2 start, struct Vec2 end, bool inverse) {
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
            newLocation = pathFinding(currentGameData.player.location, currentGameData.enemies[strongestEnemy].location, false);
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
            newLocation = pathFinding(currentGameData.player.location, currentGameData.enemies[closestEnemy].location, false);
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
            newLocation = pathFinding(currentGameData.player.location, closestTerrain, false);
            break;
        }
        
        case MOVEMENT_DEFEND: {
            newLocation = currentGameData.player.location;
            break;
        }

        case MOVEMENT_FALLEN: { /* This too could be more efficient */
            struct Vec2 closestFallenKatana = (struct Vec2) {0, 0};
            int closestFallenKatanaDistance = MAP_HEIGHT + MAP_WIDTH;
            for (int y = 0; y < MAP_HEIGHT; y++) {
                for (int x = 0; x < MAP_WIDTH; x++) {
                    if (currentGameData.map[y][x].hasFallenKatana) {
                        int currentFallenKatanaDistance = findDistance(currentGameData.player.location, (struct Vec2) {y, x});
                        if (currentFallenKatanaDistance < closestFallenKatanaDistance) {
                            closestFallenKatana = (struct Vec2) {y, x};
                            closestFallenKatanaDistance = currentFallenKatanaDistance;
                        }
                    }
                }
            }
            newLocation = pathFinding(currentGameData.player.location, closestFallenKatana, false);
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

void enemyMovement(int enemyIndex) {
    struct Enemy* currentEnemy = &currentGameData.enemies[enemyIndex];

    struct Vec2 newLocation;

    if (CHECK_BIT(currentGameData.comboFlags, COMBO_FLAG_SCARE_ENEMIES)) {
        newLocation = pathFinding(currentEnemy->location, currentGameData.player.location, true);
    } else {
        newLocation = pathFinding(currentEnemy->location, currentGameData.player.location, false);
    }



    if (checkForEnemy(newLocation) == false) {

        currentEnemy->location = newLocation;

        currentEnemy->lastMovementTurn = currentGameData.turn;
    }
}

/* Attacking */

void attackEnemy(int enemyIndex, struct Katana* katana) {
    double resistancePercent = 1.0;
    if (katanaToTerrain[katana->type] == currentGameData.enemies[enemyIndex].type) {
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

    int temp = myRand(10);

    if (temp > 5) {
        katana->sharpness -= (temp - 5);
    }
    if (katana->sharpness < MIN_KATANA_SHARPNESS) {
        katana->sharpness = MIN_KATANA_SHARPNESS;
    }
}

void attackPlayer(int enemyIndex) {
    currentGameData.player.health -= currentGameData.enemies[enemyIndex].damage;
}


/* Generators & Destructors */

void genEnemy(int type, int sideLocation) {
    if (currentGameData.currentNumberOfEnemies < MAX_NUMBER_OF_ENEMIES) {
        struct Enemy *currentEnemy = &currentGameData.enemies[currentGameData.currentNumberOfEnemies];

        //currentEnemy->type = myRand(NUMBER_OF_ENEMY_TYPES);
        currentEnemy->type = type;

        //currentEnemy->speed = abs(dice(3, 4) - 6);
        currentEnemy->speed = dice(2, 2);
        //currentEnemy->speed = 4;

        currentEnemy->maxHeath = dice(4, 5);
        currentEnemy->health = currentEnemy->maxHeath;
        currentEnemy->damage = dice(2, 5);
        currentEnemy->level = currentGameData.enemies[currentGameData.currentNumberOfEnemies].health * currentGameData.enemies[currentGameData.currentNumberOfEnemies].damage;


        struct Vec2 location = (struct Vec2) {0, 0};

        // Simple way to avoid infinate loop
        int tests = 50;
        do {
            tests--;
            //switch (myRand(4)) {
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
        } while ((checkForEnemy(location) != 0 || findDistance(location, currentGameData.player.location) < 8) && tests > 0);

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

void genKatana(struct Katana* katana) {
    katana->type = myRand(NUMBER_OF_KATANA_TYPES);

    /* 1 - 12 */
    katana->damage = dice(3, 4) + 1;

    /* 0 - 6 */
    katana->damageMod = fmax(dice(4,3) - 6, 0);

    katana->movementType = myRand(NUMBER_OF_MOVEMENT_TYPES);

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
    struct Katana fallenKatana;
    genKatana(&fallenKatana);

    struct Vec2 location;
    do {
        location.x = myRand(MAP_WIDTH);
        location.y = myRand(MAP_HEIGHT);
    } while(findDistance(location, currentGameData.player.location) < 5 || checkForEnemy(location) || currentGameData.map[location.y][location.x].hasFallenKatana);

    if (myRand(3) == 0) {
        terrainAreaMap(katanaToTerrain[fallenKatana.type], location, myRand(3) + 1);
    } else {
        terrainAreaMap(TERRAIN_GRASS, location, myRand(3) + 1);
    }

    currentGameData.map[location.y][location.x].fallenKatana = fallenKatana;
    currentGameData.map[location.y][location.x].hasFallenKatana = true;
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
    int baseEnemise = 5;
    int additionEnemise = 5;
    wave->difficulty = (int) (currentGameData.turn / 100) + 1;

    int flag1 = pow(2, myRand(4));
    int flag2 = pow(2, myRand(4));
    int flag3 = pow(2, myRand(4));
    int flag4 = pow(2, myRand(4));

    wave->flags = flag1 + (flag2 << 4) + (flag3 << 8) + (flag4 << 12);

    if (CHECK_BIT(wave->flags, WAVE_FLAG_ONLY_ONE_TYPE)) {
        wave->enemiesToSpawn[myRand(4)] = wave->difficulty * (myRand(5) + 5);
    } else if (CHECK_BIT(wave->flags, WAVE_FLAG_ONLY_TWO_TYPES)) {
        int typeOne = myRand(4);
        int typeTwo;
        do {
            typeTwo = myRand(4);
        } while (typeOne == typeTwo);

        wave->enemiesToSpawn[typeOne] = wave->difficulty * (myRand(5) + 5);
        wave->enemiesToSpawn[typeTwo] = wave->difficulty * (myRand(5) + 5);

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

        wave->enemiesToSpawn[typeOne] = wave->difficulty * (myRand(5) + 5);
        wave->enemiesToSpawn[typeTwo] = wave->difficulty * (myRand(5) + 5);
        wave->enemiesToSpawn[typeThree] = wave->difficulty * (myRand(5) + 5);
    } else if (CHECK_BIT(wave->flags, WAVE_FLAG_ALL_TYPES)) {
        wave->enemiesToSpawn[0] = wave->difficulty * (myRand(5) + 5);
        wave->enemiesToSpawn[1] = wave->difficulty * (myRand(5) + 5);
        wave->enemiesToSpawn[2] = wave->difficulty * (myRand(5) + 5);
        wave->enemiesToSpawn[3] = wave->difficulty * (myRand(5) + 5);
    } else {
        ERROR("Wave flag bit error");
    }

    //wave->enemiesToSpawn = (difficulty) * 2 + myRand(difficulty * 3);
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
    for (int i = 0; i < currentGameData.numberOfCombos; i++) {
        int comboLengthDifference = currentGameData.combos[i].length - combo.length;
        if (comboLengthDifference > 0) { // New combo smaller
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
        } else if (comboLengthDifference < 0) { // New combo bigger
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
        } else { // Same len
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
            currentGameData.player.health += myRand(PLAYER_START_HEALTH / 20);
            if (currentGameData.player.health > PLAYER_START_HEALTH) {
                currentGameData.player.health = PLAYER_START_HEALTH;
            }
            break;
        }

        case 2: { /* Terriform */
            int newTerrain;
            do {
                newTerrain = myRand(NUMBER_OF_TERRAIN_TYPES);
            } while (currentGameData.map[currentGameData.player.location.y][currentGameData.player.location.x].type == newTerrain);
            currentGameData.map[currentGameData.player.location.y][currentGameData.player.location.x].type = newTerrain;
            break;
        }
        
        case 3: { /* Freeze */
            currentGameData.turnsToFreeze = 2;
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
            } while(findDistance(currentGameData.player.location, newLocation) < 5 || findDistanceToClosestEnemy(newLocation) == 0 || currentGameData.map[newLocation.y][newLocation.x].hasFallenKatana);

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
int getStringInput(int y, int x, bool center, char* buffer) {
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
                for (int i = currentBufferSize; i > currentCharIndex; i--){
                    buffer[i] = buffer[i - 1];
                }

                buffer[currentCharIndex] = input;

                currentBufferSize++;
                currentCharIndex++;
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
    //printf("Turn: %i, Speed: %i, Moved: %i\n", currentGameData.turn, currentGameData.enemies[enemy].speed, enemyToMove);
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
            break;
        case TOP_RIGHT_KATANA:
            replaceKatana(1, fallenKatana);
            break;
        case BOTTOM_LEFT_KATANA:
            replaceKatana(2, fallenKatana);
            break;
        case BOTTOM_RIGHT_KATANA:
            replaceKatana(3, fallenKatana);
            break;
        case BREAK_KATANA_KEY: /* Small health boost */
            currentGameData.player.health += myRand(fallenKatana.damage);
            if (currentGameData.player.health > PLAYER_START_HEALTH) {
                currentGameData.player.health = PLAYER_START_HEALTH;
            }
            break;
        default:
            validAction = false;
            break;
        }
    } while(!validAction);

    currentGameData.map[currentGameData.player.location.y][currentGameData.player.location.x].hasFallenKatana = false;
}


void enemyWave() {
    if (currentGameData.currentNumberOfEnemies == 0) {
        genWave(&currentGameData.currentWave);
    }


    int totalEnemies = 0 ;
    for (int i = 0; i < NUMBER_OF_ENEMY_TYPES; i++) {
        totalEnemies += currentGameData.currentWave.enemiesToSpawn[i];
    }
    
    if (totalEnemies == 0) {
        return;
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
/*
void saveGame(FILE* fileToSaveTo) {
    if (fileToSaveTo != NULL) {
        fwrite(&currentGameData, sizeof(struct GameData), 1, fileToSaveTo);
        fclose(fileToSaveTo);
    } else {
        ERROR("fileToSaveTo is NULL");
    }
}

int loadGame(FILE* fileToLoadFrom) {
    if (fileToLoadFrom != NULL) {
        fread(&currentGameData, sizeof(struct GameData), 1, fileToLoadFrom);
        fclose(fileToLoadFrom);
    } else {
        ERROR("fileToLoadFrom is NULL");
    }

    if (currentGameData.saveCheck != 572) {
        return -1;
    }

    return 1; 
    
}
*/

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

void sliceIncertString(char* expression, char* incert, int location, int replacmentLen){
    char* oldExpression = (char*) malloc(strlen(expression) * sizeof(char) * 2);
    strcpy(oldExpression, expression);
    expression[location] = '\0';
    strcat(expression, incert);
    strcat(expression, &oldExpression[location + replacmentLen]);
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
                    sliceIncertString(newString, " ", j, 0);
                    
                    
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
    for (int i = y; i <= stopY; i++){
        printHorizontalLine(i, x, stopX, toPrint);
    }
}

void update(char* message, bool pause){
    char* formatedMessage = (char *) malloc(strlen(message) * sizeof(char) * 2);

    int messageLineLength = SCREEN_WIDTH - 18;

    formatBlock(message, formatedMessage, messageLineLength);


    char mainBuffer[SCREEN_WIDTH - 2];
    char secondaryBuffer[messageLineLength + 1];

    int linesNeeded = (int) (strlen(formatedMessage) / (messageLineLength)) + 1;

    for (int i = 0; i < linesNeeded; i++) {
        strncpy(secondaryBuffer, &formatedMessage[i * messageLineLength], messageLineLength);
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

    free(formatedMessage);

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
    
    sprintf(buffer,"Dammage: %i", katana.damage);
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
        sprintf(damageModBuffer,"");
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

void printHelp() {
    clear();
    for (int i = 0; i < NUMBER_OF_QUICK_HELP_LINES; i++) {
        mvprintw(i + 2, 1, quickHelpText[i]);
    }
    printBoarder(false);
}

void printComboReference() {
    clear();

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
        if (currentGameData.enemies[closestEnemy].level > 100) {
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
        if (currentGameData.enemies[i].level > 100) {
            mvprintw(currentGameData.enemies[i].location.y + 1, currentGameData.enemies[i].location.x + 1, enemyIcon[currentGameData.enemies[i].type]);
        } else {
            mvprintw(currentGameData.enemies[i].location.y + 1, currentGameData.enemies[i].location.x + 1, enemyIcon[currentGameData.enemies[i].type + NUMBER_OF_ENEMY_TYPES]);
        }
    }

    attron(COLOR_PAIR(COLOR_WHITE));
    mvprintw(currentGameData.player.location.y + 1, currentGameData.player.location.x + 1, "@");
    attrset(A_NORMAL);
}

void printTilecard(){
    int start = (SCREEN_WIDTH/2) - (strlen(titleCard[0])/2);

    attrset(COLOR_PAIR(COLOR_WHITE));
    for (int i = 0; i < NUMBER_OF_TITLE_CARD_LINES; i++) {
        mvprintw(1 + i, start, "%s", titleCard[i]);
    }
}