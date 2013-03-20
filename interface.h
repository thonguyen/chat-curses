#ifndef INTEFACE_H 
#define INTEFACE_H
#include<ncurses.h>

extern WINDOW *main_win, //pour afficher les messages
       *buddy_win,       //pour afficher la liste des amis
       *chat_win;        //pour l'entrée de l'utilisateur
extern int selection;

WINDOW *create_newwin(int height, int width, int starty, int startx);
void destroy_win(WINDOW *local_win);

void create_interface();
void delete_interface();
void setupColor();
void writeMessage();
void writeBuddy(char *string);
void writeBuddyNotification(char *string);
void writeBuddySelection(char *string);
void writeBuddyColor(char *string, int color);
void refreshInputWin();
void refreshBuddyWin();
void refreshMainWin();
void printHelp();
void drawLine();
#endif
