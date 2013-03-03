/*
 * =====================================================================================
 *
 *       Filename:  chat.c
 *
 *    Description:  Projet chat
 *                  Module conception des réseaux
 *        Version:  1.0
 *        Created:  03/02/2013 10:35:34 AM
 *       Revision:  1
 *       Compiler:  gcc
 *
 *         Author:  NGUYEN Van Tho, 
 *   Organization:  IFI
 *
 * =====================================================================================
 */
#include "chat.h"
#include "window.h"

int main(int argc, char ** argv){
    WINDOW *main_win;
    WINDOW *buddy_win;
    WINDOW *chat_win;
    int startx, starty, width, height;
    int ch;

    initscr();      
    cbreak();
    refresh();
    buddy_win = create_newwin(LINES, 20, 0, COLS - 20); 
    mvprintw(0, COLS - 20, "Buddy list");
    move(LINES - 4, 1);

    chat_win = create_newwin(5, COLS , LINES - 5, 0); 
    mvprintw(LINES - 5, 0, "Send Window");
    move(LINES - 4, 1);
    getch();
    destroy_win(chat_win);
    endwin();
    return 0;
}
