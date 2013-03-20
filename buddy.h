#ifndef BUDDY_H 
#define BUDDY_H
#include "queue.h"

typedef struct {
    char nick[64];      //nick name d'un ami
    char ip[16];        //ip d'ami
    int port;           
    long last_seen;     //le timestamp de la dernière vue
    char status;        //active ou inactive
    char notification;  //est-ce que des nouveaux messages
    queue *history;     //histoire des messages
} buddy;

extern buddy buddies[252];
extern buddy group;
//extern int buddy_count;



buddy *getBuddyList();      

buddy getSelectionBuddy();
buddy getBuddy(int index);
void addBuddy(buddy b);
void updateInterface();
void initBuddyList();
void *updateBuddyList();
void inactivateBuddy(char *name);
char *buildDisplayName(buddy b);
char *buildBuddyName(int i);
char *buildGroupName();
void setNotification(buddy b);
void unsetNotifcation(buddy b); 
buddy getNick(char *ip, int port);
buddy getBuddyByNick(char *nick);
int getBuddyId(char *nick);
#endif
