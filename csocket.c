/*
 * =====================================================================================
 *
 *       Filename:  socket.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/02/2013 07:31:03 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <netdb.h>

#include "csocket.h"
#include "util.h"
#include "interface.h"
#include "buddy.h"
#include "queue.h"

#define HELLO_PORT 3333
#define TCP_PORT 3334
//#define HELLO_GROUP "172.19.0.255"
#define HELLO_GROUP "225.0.0.133"
#define SLEEP_TIME 5 
#define MSG_BUF_SIZE 512
#define MSG_HELLO "/hello"
#define PENDING_CONNECTION_QUEUE_LENGTH 5
#define ENTER_KEY 10
#define GROUP_NAME "group"

#define MSG_QUIT "/quit "
#define CHAT_CMD "/chat "
#define QUIT_CMD "/quit"
#define HELP_CMD "/help"

buddy buddies[252];
buddy group;
int selection = 0;
int udp_fd = -1;
int listen_port;
int buddy_count;

int create_socket_tcp(){
    int sockfd;

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("client: socket");
        return -1; 
    }   

    return sockfd;

}    
int create_socket_udp(){
    if(udp_fd == -1){
        /*  create what looks like an ordinary UDP socket */
        if ((udp_fd = socket(AF_INET,SOCK_DGRAM,0)) == -1) {
            perror("socket");
            return -1; 
        }
        int so_broadcast = 1;

        if (setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof (so_broadcast)) < 0){
            perror("setsockopt(2), create_sock_udp");
            return -2;
        }
    }
    struct sockaddr_in sin;

    socklen_t len = sizeof(sin);
    if (getsockname(udp_fd, (struct sockaddr *)&sin, &len) == -1){
        perror("getsockname");
        exit(1);
    }  
    listen_port = sin.sin_port;
    return udp_fd;
}

int create_socket_udp_reveiver(){
    int fd;
    //créer un socket UDP 
    if ((fd = socket(AF_INET,SOCK_DGRAM,0)) == -1) {
            perror("socket");
            return -1; 
    }
    int so_broadcast = 1;
    //set option broadcast
    if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof (so_broadcast)) < 0){
            perror("setsockopt(2), create_sock_udp");
        return -2;
    }
    return fd;
}

void create_group(){
    char *groupName = "group";
    group.history = init_queue();    
    strncpy(group.ip, HELLO_GROUP, strlen(HELLO_GROUP));
    //sprintf(group.nick, "%s", GROUP_NAME);
    strncpy(group.nick, groupName, strlen(groupName));
}

void *send_message_hello(char **nick){
    struct sockaddr_in addr;
    int sockfd = create_socket_udp(); 
    if(!sockfd){
        exit(1); 
    }
    create_group();//create group for store history
//    char *ip = getLocalIp();
    /*  set up destination address */
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(HELLO_GROUP);
    addr.sin_port=htons(HELLO_PORT);

    char *msg = (char *)malloc(strlen(*nick) + 2);
    memset(msg, '\0', strlen(*nick) + strlen(MSG_HELLO) + 1);
    //msg[0] = '#';
    strncpy(msg, MSG_HELLO, strlen(MSG_HELLO));
    strncat(msg, *nick, strlen(*nick));
    /*  now just sendto() our destination! */
    while (1) {
        if (sendto(sockfd, msg, strlen(msg),0,(struct sockaddr *) &addr, sizeof(addr)) < 0) {
            perror("sendto");
            exit(1);
        }
//        printf("sending message ...%s", MSG);
        sleep(SLEEP_TIME);
    }
//    free(ip);
    free(msg);
}

void send_message_multicast(char *message){

    struct sockaddr_in addr;
    int sockfd = create_socket_udp(); 
    if(!sockfd){
        exit(1); 
    }

    /*  set up destination address */
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(HELLO_GROUP);
    addr.sin_port=htons(HELLO_PORT);

//printf("Message: %s", message);
    /*  now just sendto() our destination! */
    if (sendto(sockfd, message, strlen(message),0,(struct sockaddr *) &addr,
                sizeof(addr)) < 0) {
        perror("sendto");
        exit(1);
    }
}

void send_message_quit(char *name){
    char *message = (char *)malloc(MSG_BUF_SIZE*sizeof(char));
    memset(message, '\0', MSG_BUF_SIZE);
    strncpy(message, MSG_QUIT, strlen(MSG_QUIT));
    strncat(message, name, strlen(name));
    send_message_multicast(message);
    free(message);
}

void send_message_tcp(buddy b, char *message){
    struct sockaddr_in addr; 

    int sockfd = create_socket_tcp();
    memset(&addr, 0, sizeof(addr));     
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = inet_addr(b.ip); 
//    addr.sin_port        = htons(b.port); /*  Server port */
    addr.sin_port        = htons(TCP_PORT); /*  Server port */

//printf("String %s", message);

    /*  Establish the connection to the echo server */
    if (connect(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0){
        perror("connect tcp");
        exit(1);
    }

    /*  Send the string to the server */
    if (send(sockfd, message, strlen(message), 0) != strlen(message)){
        //"send() sent a different number of bytes than expected"
        perror("send ");
        exit(1);
    }
//    buddy b = getBuddy(selection);
    if(b.nick[0] != '\0'){
        enqueue(buddies[selection].history, message);
    }
    close(sockfd);
}

void *receive_message_hello(){
    //printf("entering client....");
    struct sockaddr_in addr, client_addr;
    int nbytes;
    struct ip_mreq mreq;

    int yes=1;            /* ** MODIFICATION TO ORIGINAL */
    int maxFD;

    fd_set readSet, testSet;

    int activeFds;
    int sockfd = create_socket_udp_reveiver(); 
    int fd;
    if(!sockfd){
        exit(1); 
    }
    /*  allow multiple sockets to use the same PORT number */
    if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(yes)) < 0) {
        perror("Reusing ADDR failed");
        exit(1);
    }

    /*  set up destination address */
    memset(&addr,0,sizeof(addr));
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=htonl(INADDR_ANY); /*  N.B.: differs from sender */
    addr.sin_port=htons(HELLO_PORT);

    /*  bind to receive address */
    if (bind(sockfd,(struct sockaddr *) &addr,sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    /*  use setsockopt() to request that the kernel join a multicast group */
    mreq.imr_multiaddr.s_addr = inet_addr(HELLO_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(sockfd,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq)) < 0) {
        perror("setsockopt");
        exit(1);
    }
    maxFD = sockfd;
    listen(sockfd, PENDING_CONNECTION_QUEUE_LENGTH);
    /*  Initialize the file descriptor sets. */
    FD_ZERO(&readSet);
    FD_ZERO(&testSet);        
    FD_SET(sockfd, &readSet);
    FD_SET(0, &readSet);  /*  ajouter stdin */
    //le boucle principal
    while (1) {
        testSet = readSet; 

        activeFds = select((maxFD + 1), &testSet, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        if (activeFds == -1) {
            delete_interface();
            perror("chat: select");
            exit(1);
        };

        for (fd = 0; fd <= maxFD; fd++) {
            if (FD_ISSET(fd, &testSet)) {
                if (fd == sockfd) {
                    socklen_t addr_len=sizeof(client_addr);

                    char *buf = (char *)malloc(MSG_BUF_SIZE*sizeof(char *));
                    memset(buf, '\0', MSG_BUF_SIZE);
                    if ((nbytes = recvfrom(fd, buf, MSG_BUF_SIZE, MSG_TRUNC,(struct sockaddr *) &client_addr,&addr_len)) < 0) {
                        perror("recvfrom");
                        exit(1);
                    }
                    if(client_addr.sin_addr.s_addr == addr.sin_addr.s_addr && client_addr.sin_port == addr.sin_port){
                        //if( client_addr.sin_port == addr.sin_port){
                        printf("The same client");
                    }
                    struct sockaddr_in sin;

                    socklen_t len = sizeof(sin);
                    if (getsockname(udp_fd, (struct sockaddr *)&sin, &len) == -1){
                        perror("getsockname");
                        exit(1);
                    }
                    char *ip =  (char *)malloc(16*sizeof(char));
                    memset(ip, '\0', 16);
                    ip = inet_ntoa(client_addr.sin_addr);
                    if( client_addr.sin_port == sin.sin_port){
                    }else{
                        //control message
                        if(buf[0] == '/'){
                            //manipuler le message au revoir 
                            if(strstr(buf, MSG_QUIT)){
                                char *nic = (char *)malloc(64*sizeof(char));
                                memset(nic, '\0', 64);
                                strncpy(nic, buf + strlen(MSG_QUIT), strlen(buf) - strlen(MSG_QUIT));
                                inactivateBuddy(nic); 
                                free(nic);
                            //manipuler le message bonjour
                            }else if(strstr(buf, MSG_HELLO)){
                                buddy b;
                                strncpy(b.nick, buf + strlen(MSG_HELLO), strlen(buf) - strlen(MSG_HELLO)); 
                                b.nick[ strlen(buf) - strlen(MSG_HELLO)] = '\0';
                                b.port = client_addr.sin_port;
                                strncpy(b.ip, ip, strlen(ip));
                                b.ip[strlen(ip)] = '\0';
                                b.status = 1;
                                //ajouter cet ami à la liste
                                addBuddy(b);
                            }
                        }else if (selection == 0){//in the session of group
                            buf[nbytes] = '\0';
                            enqueue(group.history, buf);
                            writeMessage(buf);
                        }else{
                            buf[nbytes] = '\0';
                            enqueue(group.history, buf);
                            group.notification = 1;
                        }
//                        free(buf);
                    }
                    free(buf);
                }
            }
        }
        usleep(100000);
    }
}

void *handle_input(char **nick){
    int max_fd = 1;
    fd_set test_set;
    fd_set read_set;

    FD_ZERO(&test_set);
    FD_ZERO(&read_set);

    FD_SET(0,&read_set);
    
    while(true){
        test_set = read_set;
        int activeFds = select(max_fd, &test_set, (fd_set *)0, (fd_set *)0, (struct timeval *)0);
        if (activeFds == -1){
            perror("select:");
            exit(1);
        }
        // if there are any data ready to read from the socket
        if (FD_ISSET(0, &test_set)){
            char *input_buf = (char *)malloc(MSG_BUF_SIZE*sizeof(char));
            memset(input_buf, '\0', MSG_BUF_SIZE);
            getstr(input_buf);
            if(input_buf[0] == '/'){
                handle_cmd(input_buf, *nick);
            }else{
                handle_send_string(input_buf, *nick);
            }
            free(input_buf);
            input_buf = NULL;
            refreshInputWin();                    
        }
        usleep(100000);
    }
}

void *receive_tcp(){
    int sockfd, clientfd;  
    struct sockaddr_in addr, client_addr; 

    /*  Create socket for incoming connections */
    if ((sockfd= socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
        perror("socket:");
        exit(1);
    }

    /*  Construct local address structure */
    memset(&addr, 0, sizeof(addr));   
    addr.sin_family = AF_INET;                
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
//    addr.sin_port = htons(listen_port);   
    addr.sin_port = htons(TCP_PORT);      

    /*  Bind to the local address */
    if (bind(sockfd, (struct sockaddr *) &addr, sizeof(addr)) < 0){
        perror("bind tcp:");
        exit(1);
    }

    /*  Mark the socket so it will listen for incoming connections */
    if (listen(sockfd, PENDING_CONNECTION_QUEUE_LENGTH) < 0){
        perror("listen:");
        exit(1);
    }

    while (1) 
    {
        socklen_t addr_len=sizeof(client_addr);

        /*  Wait for a client to connect */
        if ((clientfd = accept(sockfd, (struct sockaddr *) &client_addr, &addr_len)) < 0){
            perror("accept:");
            exit(1);
        }

//        char buf[MSG_BUF_SIZE]; 
        char *buf = (char *)malloc(MSG_BUF_SIZE*sizeof(char));
        memset(buf, '\0', MSG_BUF_SIZE);
        int recvMsgSize;  

        //  Recevoir message 
        if ((recvMsgSize = recv(clientfd, buf, MSG_BUF_SIZE, 0)) < 0){
            perror("recv:");
            exit(1);
        }

        if (recvMsgSize > 0)      // Zero signifie il n'y a plus de donnée
        {
            buf[recvMsgSize] = '\0';
            buddy b = getBuddy(selection);
            char *nickName = malloc_char(64);
            char *seperator = strchr(buf, ':');
            nickName = strncpy(nickName, buf, seperator - buf);
            if(strcmp(b.nick, nickName) == 0){
                enqueue(b.history, buf);
                writeMessage(buf);
            }else{
                
                int id = getBuddyId(nickName);
                if(id >0 && buddies[id].nick[0] != '\0'){
                        enqueue(buddies[id].history, buf);
                        buddies[id].notification = 1;
                    }
                }
        }
        free(buf);
        close(clientfd);    /*  Close client socket */ 
        usleep(100000);
    }
}

void handle_cmd(char* cmd, char *nick){
    if(strstr(cmd, CHAT_CMD) != NULL){
        char *id = (char *)malloc(4*sizeof(char));
        memset(id, '\0', 4);
        strncpy(id, cmd + strlen(CHAT_CMD), strlen(cmd) - strlen(CHAT_CMD)); 
        selection = atoi(id);
        if(selection == 0){//select group
            //restore message
            print_queue(group.history);
            group.notification = 0;
        }else if(selection <= buddy_count && buddies[selection].nick[0] != '\0'){
                print_queue(buddies[selection].history);
                buddies[selection].notification = 0;
        }else{
            writeMessage("***Invalid user!***");
        }

    }else if(strcmp(cmd, QUIT_CMD) == 0){
        send_message_quit(nick);    
        close_session();
    }else if(strcmp(cmd, HELP_CMD) == 0){
        printHelp();
    }else {
        writeMessage("***Command not found!***");
    }
}
void handle_send_string(char* message, char *nick){
    char *my_name = "Moi:";
    char *my_message = (char *)malloc((strlen(message) + strlen(my_name) + 1)*sizeof(char));
    memset(my_message, '\0', strlen(message) + strlen(my_name) + 1);
    my_message = strncpy(my_message, my_name, strlen(my_name));
    my_message = strncat(my_message, message, strlen(message));
    enqueue(group.history, my_message);
    writeMessage(my_message);

    char *send_buf = (char *)malloc((strlen(message) + strlen(nick) + 2)*sizeof(char));

    memset(send_buf, '\0', strlen(message) + strlen(nick) + 2);

    send_buf = strncpy(send_buf, nick, strlen(nick));
    send_buf = strncat(send_buf, ":", strlen(send_buf));
    send_buf = strncat(send_buf, message, strlen(message));

    if(selection == 0){
        send_message_multicast(send_buf);
    }else{
        buddy b = getBuddy(selection);                    
        send_message_tcp(b, send_buf);
    }
    free(my_message);
    free(send_buf);
    refreshInputWin();                    
}

void restore_message(queue *history){
    //erase main window
    refreshMainWin(); 
    while(!empty(history)){
        writeMessage(dequeue(history));
    }
}

void close_session(){
    delete_interface();
    exit(0);
}
