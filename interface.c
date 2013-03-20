/*
 * =====================================================================================
 *
 *       Filename:  interface.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/15/2013 11:16:38 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include<errno.h>
#include<stdlib.h>
#include<string.h>
#include "interface.h"

#define BUDDY_WINDOW_WIDTH 30
#define CHAT_WINDOW_HEIGHT 5

#define ME "Moi:" 

WINDOW *main_win, *buddy_win, *chat_win;

bool first_message = 1;
bool first_buddy = 1;

void create_interface(){
    initscr();
    cbreak();
    //nonl();
    keypad(stdscr, TRUE);
    setupColor();
    drawLine();

    buddy_win = create_newwin(LINES - CHAT_WINDOW_HEIGHT - 2, BUDDY_WINDOW_WIDTH, 0, COLS - BUDDY_WINDOW_WIDTH);

    chat_win = create_newwin(CHAT_WINDOW_HEIGHT, COLS , LINES - CHAT_WINDOW_HEIGHT, 0);
    main_win = create_newwin(LINES - CHAT_WINDOW_HEIGHT - 2, COLS - BUDDY_WINDOW_WIDTH - 2, 0, 0);

    touchwin(stdscr);
    refresh();

    refreshBuddyWin();

    scrollok(main_win, 1);
    werase(main_win);
    wrefresh(main_win);

    scrollok(chat_win, 1);
    refreshInputWin();
    move(LINES - CHAT_WINDOW_HEIGHT + 1, 0);
}   

void delete_interface(){
    delwin(chat_win);
    delwin(buddy_win);
    delwin(main_win);
    touchwin(stdscr);
    refresh();
    endwin();
}

int isMe(char *string){
    char *moi;    
    if((moi = strstr(string, ME)) && moi == string){
        return 1;
    }
    return 0;
}

void writeMessage(char *string) {
    if(isMe(string)){
        wattron(main_win, COLOR_PAIR(3));//Mes messages sont en cyan
    }else{
        wattron(main_win, COLOR_PAIR(4));//Messages de mes amis sont en verte
    }

    if(first_message == 1){
        if(isMe(string)){
            wattron(buddy_win, COLOR_PAIR(3));
        }
        mvwprintw(main_win, 1, 0, string);
        first_message = 0;
    }else{
        wprintw(main_win, string);
    }
    wprintw(main_win, "\n");
    wrefresh(main_win); 
    if(isMe(string)){
        wattroff(main_win, COLOR_PAIR(3));
    }else{
        wattroff(main_win, COLOR_PAIR(4));
    }
} 

void writeBuddy(char *string){
    writeBuddyColor(string, 0);
}

void writeBuddyNotification(char *string){
    char *sel = (char *)malloc(strlen(string) + 2);
    memset(sel, '\0', strlen(string) + 2);
    strncpy(sel, string, strlen(string));
    writeBuddyColor(sel, 1);
    free(sel);
}
void writeBuddySelection(char *string){
    char *sel = (char *)malloc(strlen(string) + 2);
    memset(sel, '\0', strlen(string) + 2);
    strncpy(sel, string, strlen(string));
    writeBuddyColor(sel, 2);
    free(sel);
}
void writeBuddyColor(char *string, int color){
    if(color > 0){
        wattron(buddy_win,COLOR_PAIR(color)); //si je chatte avec lui le couleur rouge sur fond blanch
    }                                         //si il y a des messages de lui, le couleur rouge sur noir
    if(color == 1){
        wattron(buddy_win, A_STANDOUT);//si je chatte avec lui 
    }
    if(first_buddy == 1){
        mvwprintw(buddy_win, 0, 0, string);
        first_buddy = 0;
    }else{
        wprintw(buddy_win, string);
    }
    wprintw(buddy_win, "\n");

    if(color > 0){
        wattroff(buddy_win,COLOR_PAIR(color));
    }
    if(color == 1){
        wattroff(buddy_win, A_STANDOUT);
    }
    wrefresh(buddy_win); 
}

void setupColor(){
    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_WHITE);
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

    local_win = subwin(stdscr, height, width, starty, startx);
    wrefresh(local_win);

    return local_win;
}

void drawLine(){
    int x_loop = 0;
    for (x_loop = 0; x_loop < COLS; x_loop++)
        mvwaddch(stdscr, LINES - CHAT_WINDOW_HEIGHT - 1, x_loop, '_');
    
    for (x_loop = 0; x_loop < LINES; x_loop++){
        mvwaddch(stdscr, x_loop, COLS - BUDDY_WINDOW_WIDTH - 1, '|');
    }
}

void refreshInputWin(){
    werase(chat_win);
    wmove(chat_win, 1, 0);                                                              
    wrefresh(chat_win);
}
void refreshBuddyWin(){
    werase(buddy_win);
    wmove(buddy_win, 0, 0);                                                              
    wrefresh(buddy_win);
}

void refreshMainWin(){
    werase(main_win);
    wmove(main_win, 0, 0);                                                              
    wrefresh(main_win);
}

void printHelp(){
    init_pair(1, COLOR_RED, COLOR_BLACK);
    wattron(main_win, COLOR_PAIR(1));
    writeMessage("********************************************");
    writeMessage("* Système de messagerie                    *");
    writeMessage("* Groupe 3 - IFI Promotion 17              *");
    writeMessage("*                                          *");
    writeMessage("* Liste de commandes                       *");
    writeMessage("* /chat nombre : chat avec un utilisateur  *");
    writeMessage("* /quit        : Quitter le programme      *");
    writeMessage("* /help        : Imprunter cette aide      *");
    writeMessage("********************************************");
    wattroff(main_win, COLOR_PAIR(1));
}

