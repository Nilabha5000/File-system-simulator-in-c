#include "queue.h"

struct queue *q_init(){
     struct queue *q = (struct queue*)malloc(sizeof(struct queue));
     q->front = NULL;
     q->rear = NULL;
     return q;
}

void q_push(struct queue *q , struct dir *d){
      if(q == NULL){
          perror("queue is NULL");
          return;
      }
      struct q_node *newnode = (struct q_node*)malloc(sizeof(struct q_node));
      newnode->d = d;
      newnode->next = NULL;
      
      if(q->front == NULL){
          q->front = newnode;
          q->rear = q->front;
          return;
      }
      
      q->rear->next = newnode;
      q->rear = q->rear->next;
}

void q_pop(struct queue *q){
     if(q == NULL){
          perror("queue is NULL");
          return;
     }
     
    if(q->front == NULL){
         perror("queue is empty");
         return;
    }
    if(q->front == q->rear){
         free(q->front);
         q->front = NULL;
         q->rear = NULL;
         return;
    }
    struct q_node *temp = q->front;
    
    q->front = q->front->next;
    free(temp);
}
struct dir *q_front(struct queue *q){
    if(q == NULL || q->front == NULL){
        perror("queue is empty");
        return NULL;
    }
    return q->front->d;
}

int q_empty(struct queue *q){
    if(q == NULL) return 1;
    return (q->front == NULL);
}

void q_destroy(struct queue *q){
    if(q == NULL) return;
    
    while(!q_empty(q)){
        q_pop(q);
    }
    
    free(q);
}
