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
#include <stdio.h>
#include <pthread.h> 
#include "chat.h"
#include "window.h"
#include "csocket.h"

int main(int argc, char ** argv){
    WINDOW *main_win;
    WINDOW *buddy_win;
    WINDOW *chat_win;
    int startx, starty, width, height;
    int ch;
    pthread_t send_thread, receive_thread;
    void *send_message_hello(), *receive_message_hello();
    initscr();      
    cbreak();
    refresh();
    buddy_win = create_newwin(LINES, 20, 0, COLS - 20); 
    mvprintw(0, COLS - 20, "Buddy list");
    move(LINES - 4, 1);

    chat_win = create_newwin(5, COLS , LINES - 5, 0); 
    mvprintw(LINES - 5, 0, "Send Window");
    move(LINES - 4, 1);
    pthread_create(&send_thread,NULL,send_message_hello,NULL);
    pthread_create(&receive_thread,NULL,receive_message_hello,NULL);
    pthread_join(send_thread,NULL);
    pthread_join(receive_thread,NULL);
//    send_message_hello();
//    receive_message_hello();
    getch();
    destroy_win(chat_win);
    endwin();
    return 0;
}
