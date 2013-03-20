/*
 * =====================================================================================
 *
 *       Filename:  buddy.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/16/2013 03:10:56 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include <menu.h>
#include <errno.h>
#include <unistd.h>

#include "buddy.h"
#include "queue.h"
#include "interface.h"
#include "util.h"

#define HELLO_TIME 5
#define TIME_OUT_BUDDY 5*HELLO_TIME 

//buddy buddies[252];
buddy buddies[252];
int buddy_count = 0; 
int selection;

WINDOW *buddy_win;
char *displayNames[252];
//static int  buddy_count;
ITEM **items;
MENU *buddy_menu;
ITEM *cur_item;

void addBuddy(buddy b){
    int i;
//    struct timeval t;
    long current_time = getCurrentTime();
    if(buddy_count == 0){
            b.last_seen = current_time;
            b.history = init_queue();
            b.notification = 0;
            buddies[1] = b; 
            buddy_count++;
            char *s = buildBuddyName(1);
            writeBuddy(s);
//            writeBuddySelection(s);
//             initBuddyList();
            free(s);
    }else{
        bool found = 0;
        for (i = 1; i <= buddy_count; i++){
//            if(strcmp(buddies[i].nick, b.nick) == 0 && strcmp(buddies[i].ip, b.ip) == 0){
            if(strcmp(buddies[i].nick, b.nick) == 0){
                found = 1;
//printf("Update");
                buddies[i].last_seen = current_time; 
                buddies[i].status = 1;
            }
        }
        if(!found){
            b.last_seen = current_time;
            b.history = init_queue();
            b.notification = 0;
            buddies[++buddy_count] = b; 
            char *s = buildBuddyName(buddy_count);
//            updateBuddyList();
            writeBuddy(s);
            free(s);
        }
    }
    for (i = 1; i <= buddy_count; i++){
        if(current_time - buddies[i].last_seen > TIME_OUT_BUDDY){
//printf("%ld-%ld", current_time, buddies[i].last_seen);
            //remove this buddy
            buddies[i].status = 0;
        }
    }
//    updateBuddyList();
}
buddy getSelectionBuddy(){
    return buddies[selection];
}

buddy getBuddy(int index){
    return buddies[index];
}
int getBuddyId(char *nick){
    int i;
    for(i = 1; i <= buddy_count; i++){
        if(strcmp(buddies[i].nick, nick) == 0 ){
            return i;
        }
    }   
    return 0;
}
char *buildBuddyName(int i){
    char num[4];
    num[0] = '(';
    num[1] = (char)(i+48);
    num[2] = ')';
    num[3] = '\0';
    char *s = (char *)malloc(100*sizeof(char));
    memset(s, '\0', 100);
    s = strncpy(s, num, strlen(num));
    s = strncat(s, buddies[i].nick, strlen(buddies[i].nick));
    strcat(s, "@");
    strcat(s, buddies[i].ip);
    return s;
}
char *buildDisplayName(buddy b){
    char *s = (char *)malloc(100*sizeof(char));
    memset(s, '\0', 100);
    s = strncpy(s, b.nick, strlen(b.nick));
    strcat(s, "@");
    strcat(s, b.ip);
    return s;
}
char *buildGroupName(){
    char *s = (char *)malloc(100*sizeof(char));
    memset(s, '\0', 100);
    s = strncpy(s, "(0)", strlen("(0)"));
    s = strncat(s, group.nick, strlen(group.nick));
    strcat(s, "@");
    strcat(s, group.ip);
    return s;
}

buddy getNick(char *ip, int port){
    int i;
    buddy b;
    for(i = 0; i < buddy_count; i++){
        if(strcmp(buddies[i].ip, ip) == 0 && buddies[i].port == port){
            b = buddies[i];
        }
    }
    return b;
}

buddy getBuddyByNick(char *nick){
    int i;
    buddy b;
    for(i = 1; i <= buddy_count; i++){
        if(strcmp(buddies[i].nick, nick) == 0 ){
            return buddies[i];
        }
    }   
    return b;
}
void inactivateBuddy(char *nick){
    int i;
    for (i = 2; i <= buddy_count; i++){
//printf("%s++%s\n", buddies[i].nick, nick);
        if(strcmp(buddies[i].nick, nick) == 0){
//printf("%s++%s", buddies[i].nick, nick);
            buddies[i].status = 0;
            break;
        }   
    }   
//    updateBuddyList();
}
void *updateBuddyList(){
    while(1){
    int i;
//    long current_time = getCurrentTime();
    refreshBuddyWin();
    char * g = buildGroupName();
    if(selection == 0){
        writeBuddySelection(g);
    }else if(group.notification){
        writeBuddyNotification(g);
    }else{
        writeBuddy(g);
    }
    free(g);
    for (i = 1; i <= buddy_count; i++){
        if(buddies[i].status){
            char *s = buildBuddyName(i);
            if(selection == i){
                writeBuddySelection(s);
            }else if(buddies[i].notification){
                writeBuddyNotification(s);
            }else{
                writeBuddy(s);
            }
            free(s);
        }
    }
//    refresh();
//    wrefresh(buddy_win);
//    refreshInputWin();
    sleep(1);
    }
}

/* 
void updateBuddyList(){
    int i;
    for(i = 1; i <= buddy_count; i++){
        char *s = (char *)malloc(100*sizeof(char));
        memset(s, '\0', 100);
        s = strncpy(s, buddies[i].nick, strlen(buddies[i].nick));
        strcat(s, "@");
        strcat(s, buddies[i].ip);

        items[i] = new_item(s, "");
    }
    unpost_menu(buddy_menu);
    post_menu(buddy_menu);
        //s = strdup(buddies[i].nick);
    //set_menu_items(buddy_menu, items);
    wrefresh(buddy_win);
  char c;
    while((c = wgetch(buddy_win)) != KEY_F(1))
    {       switch(c)
        {   case KEY_DOWN:
            menu_driver(buddy_menu, REQ_DOWN_ITEM);
            break;
            case KEY_UP:
            menu_driver(buddy_menu, REQ_UP_ITEM);
            break;
        }
    }
}
*/
void initBuddyList(){
    //build list displayName
    int i;
    items = (ITEM **)calloc(252, sizeof(ITEM *));
    items[0] = new_item("Group","");
    for(i = 1; i <= buddy_count; i++){
        char *s = (char *)malloc(100*sizeof(char));
        memset(s, '\0', 100);
        s = strncpy(s, buddies[i].nick, strlen(buddies[i].nick));
        strcat(s, "@");
        strcat(s, buddies[i].ip);

        //s = strdup(buddies[i].nick);
        items[i] = new_item(s, "");
    }
    for(i = 0; i < buddy_count; i++){
        //printf(displayNames[i]);
        //        items[i] = new_item(displayNames[i], displayNames[i]);
//        char *name = (char *)malloc(strlen(displayNames[i]) + 1);
//        name = strncpy(name, displayNames[i], strlen(displayNames[i]));
//        char *xxx = "Bonsoir";
//        name[strlen(displayNames[i])] = '\0';
        //name = strncpy(name, displayNames[i], strlen(displayNames[i]));
//        char *yyy = (char *)malloc(strlen(xxx) + 1);
//        yyy = strdup(xxx);
     //   items[i] = new_item(yyy, "");

    }

//    keypad(buddy_win, TRUE);
    buddy_menu = new_menu((ITEM **)items);
    set_menu_win(buddy_menu, buddy_win);
    set_menu_sub(buddy_menu, derwin(buddy_win, 0, 0, 0, 0));
    //set_menu_sub(buddy_menu, buddy_win);
    post_menu(buddy_menu);
    wrefresh(buddy_win);
    refresh();
    /*  char c;
    while((c = wgetch(buddy_win)) != KEY_F(1))
    {       switch(c)
        {   case KEY_DOWN:
            menu_driver(buddy_menu, REQ_DOWN_ITEM);
            break;
            case KEY_UP:
            menu_driver(buddy_menu, REQ_UP_ITEM);
            break;
        }
        wrefresh(buddy_win);
    }*/   
}

void updateInterface(){
    /*    int i;

          refreshBuddyWin();
    for (i = 1; i <= buddy_count; i++){
    char *s = (char *)malloc(100*sizeof(char));
    s = strncpy(s, buddies[i].nick, strlen(buddies[i].nick));
    strcat(s, "@");
    strcat(s, buddies[i].ip);
    writeBuddy(s);
    }
    */
}
