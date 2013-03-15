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

#define HELLO_PORT 3333
//#define HELLO_GROUP "172.19.0.255"
#define HELLO_GROUP "225.0.0.133"
#define SLEEP_TIME 30
#define MSG_BUF_SIZE 512
#define MSG "hello,tom@172.19.0.104"

int create_socket_tcp(){
    int sockfd;

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("client: socket");
        return -1; 
    }   

    return sockfd;

}    
int create_socket_udp(){
    int sockfd;
    /*  create what looks like an ordinary UDP socket */
    if ((sockfd=socket(AF_INET,SOCK_DGRAM,0)) == -1) {
        perror("socket");
        return -1; 
    }
    int so_broadcast = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &so_broadcast, sizeof (so_broadcast)) < 0){
        perror("setsockopt(2), create_sock_udp");
        return -2;
    }
    return sockfd;
}


void send_message_hello(){
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

    /*  now just sendto() our destination! */
    while (1) {
        if (sendto(sockfd,MSG,sizeof(MSG),0,(struct sockaddr *) &addr,
                    sizeof(addr)) < 0) {
            perror("sendto");
            exit(1);
        }
        printf("sending message ...%s", MSG);
        sleep(SLEEP_TIME);
    }
}

void receive_message_hello(){
    struct sockaddr_in addr;
    int nbytes,addrlen;
    struct ip_mreq mreq;
    char msgbuf[MSG_BUF_SIZE];

    int yes=1;            /* ** MODIFICATION TO ORIGINAL */

    int sockfd = create_socket_udp(); 
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

    /*  now just enter a read-print loop */
    while (1) {
        socklen_t addr_len=sizeof(addr);
        if ((nbytes=recvfrom(sockfd,msgbuf,MSG_BUF_SIZE,0,
                        (struct sockaddr *) &addr,&addr_len)) < 0) {
            perror("recvfrom");
            exit(1);
        }
        puts(msgbuf);
    }
}
