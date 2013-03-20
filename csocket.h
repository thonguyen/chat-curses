//
#ifndef CSOCKET_H 
#define CSOCKET_H 
#include "buddy.h"
void *send_message_hello(char **nick);              //envoyer les messages udp 
void *receive_message_hello();                      //recevoir et traiter les messages bonjour
void *handle_input(char **nick);                    //manipuler les entrées
void *receive_tcp();                                //recevoir et traiter les messages tcp 
void send_message_udp(char *message);               
void send_message_tcp(buddy b, char *message);      //envoyer les messages tcp
void send_message_multicast(char *message);
void send_message_quit(char *);
void handle_send_string(char* message, char *nick);
void handle_cmd(char* cmd, char *nick);             //manipuler les commandes
void create_group();
void restore_message(queue *history);
void close_session();
#endif
