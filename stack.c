#include "stack.h"
#include "FS.h"
#include <errno.h>
struct stack *init(){
    struct stack *s = (struct stack*)malloc(sizeof(struct stack));
    s->top = NULL;
   return s;
}
void push(struct stack **s , struct dir *d){
    //allocating new node 
    struct s_node *newnode = (struct s_node*)malloc(sizeof(struct s_node));
    newnode->d = d;
    newnode->next = NULL;
    //if the stack is empty 
    if((*s)->top == NULL)
        (*s)->top = newnode;
    else{
       newnode->next = (*s)->top;
       (*s)->top = newnode;
    }
}

void pop(struct stack **s){
    if(*s != NULL || (*s)->top != NULL){
         struct s_node *temp = (*s)->top;
         (*s)->top = (*s)->top->next;
         free(temp);
    }
    else
      perror("stack underflow");
}

struct dir *peek(struct stack **s){
    if(*s == NULL || (*s)->top == NULL)
       return NULL;
    return (*s)->top->d;
}
int is_empty(struct stack **s){
     
     return ((*s)->top == NULL);
}
void destroy_stack(struct stack *s){
    
    if(s == NULL) return;
    if(s->top == NULL){
        free(s);
        return;
    }
    
    struct s_node *curr = s->top;
    struct s_node *prev = NULL;
    while(curr != NULL){
        prev = curr;
        curr = curr->next;
        free(prev);
    }
    
    free(s);
    printf("success \n");
}


