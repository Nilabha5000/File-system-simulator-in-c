#ifndef STACK_H
#define STACK_H

#include "FS.h"

struct s_node{
   struct dir *d; 
   struct s_node *next;
};
struct stack{
     struct s_node *top; 
};
struct stack *init();
void push(struct stack ** , struct dir *);
void pop(struct stack **);
struct dir *peek(struct stack **);
int is_empty(struct stack **);
void destroy_stack(struct stack *);
#endif
