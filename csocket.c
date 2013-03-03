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
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "csocket.h"

int c_socket_create(){
    int sockfd;

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("client: socket");
        return -1; 
    }   

    return sockfd;

}    

int mk_socket_connect(int socket_fd, char *host, int port)
{
    int ret;

//    ret = plg_netiomap->connect(socket_fd, host, port);


    return ret;
}

