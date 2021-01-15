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
#include <time.h>            /* Functions for date and time                                                */
#include <string.h>          /* Functions for string manipulation                                          */
#include <pdcurses/curses.h> /* Libraray for terminal manipulation                                         */
#include "Katana.h"          /* Katana variables, arrays and structures                                    */
/*---- Main Function --------------------------------------------------------------------------------------*/

int main(){
    long seed = time(NULL);
    srand(seed);

    initCurses();
    printBoarder();

    struct Katana katanaTest;
    genKatana(&katanaTest);

    printKatana(katanaTest, 0);
    genKatana(&katanaTest);
    printKatana(katanaTest, 1);
    genKatana(&katanaTest);
    printKatana(katanaTest, 2);
    genKatana(&katanaTest);
    printKatana(katanaTest, 3);



    myGetch();

    stopCurses();
}


/* Generators */

void genKatana(struct Katana *katana) {
    katana->katanaType = (rand() % NUMBER_OF_KATANA_TYPES);
    /* 1 - 12 */
    katana->damageAmount = dice(3, 4) + 1;

    katana->movementType = (rand() % NUMBER_OF_MOVEMENT_TYPES);

    /* 20 - 100 */
    katana->hitChance = dice(4, 20) + 20;

    sprintf(katana->name, "%s%s", katanaNameType[katana->katanaType], katanaNameDamage[katana->damageAmount - 1]);
};




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


wchar_t myGetch(){
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
    printVerticalLine(0, 0, SCREEN_HEIGHT, "|");
    printVerticalLine(SCREEN_WIDTH, 0, SCREEN_HEIGHT, "|");

    printHorizontalLine(0, 0, SCREEN_WIDTH, "-");
    printHorizontalLine(SCREEN_HEIGHT, 0, SCREEN_WIDTH, "-");

    mvprintw(0, 0, "+");
    mvprintw(0, SCREEN_WIDTH, "+");
    mvprintw(SCREEN_HEIGHT, 0, "+");
    mvprintw(SCREEN_HEIGHT, SCREEN_WIDTH, "+");
}


void printKatana(struct Katana katana, int position) {
    int katanaInfoBoxHeight = (SCREEN_HEIGHT / 3) - 2;
    int katanaInfoBoxWidth = (SCREEN_WIDTH / 2) - 2;

    struct Vec2 topRightCorner = {0, 0};
    struct Vec2 topLeftCorner = {0, 0};
    struct Vec2 bottomRightCorner = {0, 0};
    struct Vec2 bottomLeftCorner = {0, 0};
    
    switch(position) { 
        case 0:
            topRightCorner.y = (SCREEN_HEIGHT / 3) + 2;
            topRightCorner.x = 1;
            break;
        case 1:
            topRightCorner.y = (SCREEN_HEIGHT / 3) + 2;
            topRightCorner.x = (SCREEN_WIDTH - 1) - katanaInfoBoxWidth;
            break;
        case 2:
            topRightCorner.y = (SCREEN_HEIGHT / 3) + 3 + katanaInfoBoxHeight;
            topRightCorner.x = 1;
            break;
        case 3:
            topRightCorner.y = (SCREEN_HEIGHT / 3) + 3 + katanaInfoBoxHeight;
            topRightCorner.x = (SCREEN_WIDTH - 1) - katanaInfoBoxWidth;
            break;
        default:
            ERROR("Invalid position argument");
    }

    topLeftCorner.y = topRightCorner.y;
    topLeftCorner.x = topRightCorner.x + katanaInfoBoxWidth;

    bottomRightCorner.y = topRightCorner.y + katanaInfoBoxHeight;
    bottomRightCorner.x = topRightCorner.x;

    bottomLeftCorner.y = topRightCorner.y + katanaInfoBoxHeight;
    bottomLeftCorner.x = topRightCorner.x + katanaInfoBoxWidth;

    
    printVerticalLine(topRightCorner.x, topRightCorner.y, bottomRightCorner.y, "|");
    printVerticalLine(topLeftCorner.x, topRightCorner.y, bottomRightCorner.y, "|");

    printHorizontalLine(topRightCorner.y, topRightCorner.x, topLeftCorner.x, "-");
    printHorizontalLine(bottomLeftCorner.y, topRightCorner.x, topLeftCorner.x, "-");

    
    mvprintw(topRightCorner.y, topRightCorner.x, &katanaCornerIcon[katana.katanaType][0]);
    mvprintw(topLeftCorner.y, topLeftCorner.x, &katanaCornerIcon[katana.katanaType][0]);
    mvprintw(bottomRightCorner.y, bottomRightCorner.x, &katanaCornerIcon[katana.katanaType][0]);
    mvprintw(bottomLeftCorner.y, bottomLeftCorner.x, &katanaCornerIcon[katana.katanaType][0]);

    char buffer[katanaInfoBoxWidth - 2];

    
    mvprintw(topRightCorner.y + 1, (topRightCorner.x + (katanaInfoBoxWidth/2)) - (strlen(katana.name)/2), katana.name);

    sprintf(buffer,"Dammage: %i", katana.damageAmount);
    mvprintw(topRightCorner.y + 3, (topRightCorner.x + (katanaInfoBoxWidth/2)) - (strlen(buffer)/2), buffer);

    sprintf(buffer, "Hit Chance: %i", katana.hitChance);
    mvprintw(topRightCorner.y + 4, (topRightCorner.x + (katanaInfoBoxWidth/2)) - (strlen(buffer)/2), buffer);

    sprintf(buffer, "Movement Type: %s", katanaMovementTypeNames[katana.movementType]);
    mvprintw(topRightCorner.y + 5, (topRightCorner.x + (katanaInfoBoxWidth/2)) - (strlen(buffer)/2), buffer);
    
    
}