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
#include <stdlib.h>
#include <pthread.h> 
#include <string.h>
#include "chat.h"
#include "window.h"
#include "csocket.h"
#include "interface.h"
#include "buddy.h"



WINDOW *main_win, *buddy_win, *chat_win;
buddy buddies[252];

//extern int buddy_count;
int main(int argc, char ** argv){
    if(argc < 2){
        perror("Enter nickname!");
        exit(1);
    }
    char *nick = (char *)malloc((strlen(argv[1]) + 1)*sizeof(char));
    memset(nick, '\0', strlen(argv[1]) + 1);
    strncpy(nick, argv[1], strlen(argv[1]));

    create_interface(); //initialise l'interface
    printHelp();
    pthread_t send_thread,          //envoyer des messages bonjour 
              receive_thread,       //recevoir et traiter les messages multicast 
              input_thread,         //manipuler les entrées de l'utilisateur
              receive_tcp_thread,   //recevoir et traiter les messages tcp
              ui_thread;            //gérer l'interface
    void *send_message_hello(), *receive_message_hello(), *handle_input(), *receive_tcp();
    void *updateBuddyList();
    pthread_create(&send_thread, NULL, (void *)send_message_hello, (void *)&nick);
    pthread_create(&receive_thread, NULL, (void *)receive_message_hello,NULL);
    pthread_create(&input_thread, NULL, (void *) handle_input, (void *) &nick);
    pthread_create(&receive_tcp_thread, NULL, (void *)receive_tcp , NULL);
    pthread_create(&ui_thread, NULL, (void *)updateBuddyList, NULL);
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);
    pthread_join(input_thread, NULL);
    pthread_join(receive_tcp_thread, NULL);
    pthread_join(ui_thread, NULL);
    
    //delete_interface();
    return 0;
}
