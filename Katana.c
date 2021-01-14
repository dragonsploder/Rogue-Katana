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
#include <time.h>            /* Functions for  date and time                                               */
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



    myGetch();

    stopCurses();
}


/* Generators */

void genKatana(struct Katana *katana) {
    katana->damageType = (rand() % 4) + 1;
    katana->damageAmount = dice(4, 3);

    katana->movementType = (rand() % 4) + 1;

    katana->hitChance = dice(5, 20);
};




/* Utility Functions */
int dice(int number, int sides) {
    int total = 0;
    for (int i = 0; i < number; i++) {
        total += (rand() % sides);
    }
    
    return total;
}


/* Curses Functions */


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