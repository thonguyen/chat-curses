/*
 * =====================================================================================
 *
 *       Filename:  util.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  03/15/2013 05:54:19 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <sys/time.h>

char *getLocalIp(){
    FILE * fp = popen("ifconfig", "r");
    char *ip = (char *)malloc(16*sizeof(char));
    if (fp) {
        char *p=NULL, *e; size_t n;
        while ((getline(&p, &n, fp) > 0) && p) {
            if ((p = strstr(p, "inet"))){
                p+=5;
                if ((e = strchr(p, ' '))) {
                    *e='\0';
                    if( strstr(p, "192.") != NULL || strstr(p, "172.") != NULL){
                        ip = strncpy(ip, p, strlen(p));
                    }
                }
            }
        }
    }

    pclose(fp);
    return ip;                              
}
char *malloc_char(int n){
    char *ret = (char *)malloc(n*sizeof(char));
    memset(ret, '\0', n); 
    return ret;
}
long getCurrentTime(){
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec;
}
