#ifndef QUEUE_H
#define QUEUE_H
#include "FS.h"
struct q_node{
    struct dir *d;
    struct q_node *next;
};

struct queue{
    struct q_node *front;
    struct q_node *rear;
};


struct queue *q_init();
void q_push(struct queue*, struct dir *);
void q_pop(struct queue*);
struct dir *q_front(struct queue*);
int q_empty(struct queue*);
void q_destroy(struct queue*);
#endif
