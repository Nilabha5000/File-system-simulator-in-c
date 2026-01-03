#include "hst_mng.h"
#include <stdlib.h>
#include <string.h>
// This function initializes the history manager
// It creates a new mng_history structure
// and sets the head and tail to NULL
// It returns a pointer to the new mng_history structure
mng_history *init_history(){
    mng_history *h = (mng_history*)malloc(sizeof(mng_history));
    h->head = NULL;
    h->tail = NULL;
    return h;
}
// This function adds a command to the history
// It creates a new hst_node structure  
void add_history(mng_history *h , char *command){
    hst_node *new_node = (hst_node*)malloc(sizeof(hst_node));
    new_node->command = (char*)malloc(strlen(command) + 1);
    strcpy(new_node->command, command);
    new_node->next = NULL;
    if(h->head == NULL){
        h->head = new_node;
        h->tail = new_node;
    }
    else{
        h->tail->next = new_node;
        h->tail = new_node;
    }
}
void print_history(mng_history *h){
    hst_node *current = h->head;
    int index = 1;
    while(current != NULL){
        printf("%d. %s\n", index, current->command);
        current = current->next;
        index++;
    }
}
void destroy_history(mng_history *h){
    hst_node *current = h->head;
    while(current != NULL){
        hst_node *temp = current;
        current = current->next;
        free(temp->command);
        free(temp);
    }
    free(h);
}