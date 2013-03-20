#include <stdlib.h>
#include <string.h>
#include "interface.h" 
#include "util.h" 
#include "queue.h"

//#include "bool.h"

queue *init_queue()
{
    queue* que = (queue*)malloc(sizeof(queue));
    que->q = (char **)malloc(QUEUESIZE*sizeof(char*));
    int i;
    for(i = 0 ; i < QUEUESIZE; ++i){
        que->q[i] = (char *)malloc(sizeof(char) * MSG_BUF_SIZE); 
        memset(que->q[i], '\0', MSG_BUF_SIZE);
    }
    que->first = 0;
    que->last = QUEUESIZE-1;
    que->count = 0;
    return que;
}

void enqueue(queue *q, char *msg)
{
//    char *s = malloc_char(strlen(msg) + 1);

    char *s = (char *)malloc(strlen(msg) + 1);
    memset(s, '\0', strlen(msg) + 1);
    strncpy(s, msg, strlen(msg));
    
//    sprintf("%s", msg);
    while(q->count >= QUEUESIZE){
        dequeue(q);
    }
    q->last = (q->last+1) % QUEUESIZE;
    q->q[ q->last ] = s;    
    q->count = q->count + 1;
}

char *dequeue(queue *q)
{
    char *x;

    if (q->count <= 0){
        return "";
    }
    else {
        x = q->q[ q->first ];
        q->first = (q->first+1) % QUEUESIZE;
        q->count = q->count - 1;
    }

    return x;
}

int empty(queue *q)
{
    if (q->count <= 0) return 1;
    else return 0;
}


void print_queue(queue *q)
{
    int i;

    i = q->first; 

    refreshMainWin(); 
    while (i != q->last) {
        writeMessage(q->q[i]);
        i = (i+1) % QUEUESIZE;
    }

    writeMessage(q->q[i]);
//    printf("%2d ",q->q[i]);
//    printf("\n");
}
