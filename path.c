#include "path.h"
#include <stdlib.h>
#include <string.h>

struct path_node *create_path_node(const char *name){
    struct path_node *pn = (struct path_node*)malloc(sizeof(struct path_node));
    if(pn == NULL) return NULL;

    pn->name = strdup(name);
    if(pn->name == NULL){
        free(pn);
        return NULL;
    }

    pn->next = pn->prev = NULL;

    return pn;
}

struct path *create_path(){
    struct path *p = (struct path*)malloc(sizeof(struct path));

    if(p == NULL) return NULL;
   
    p->begin = p->end = NULL;
    
    return p;
}

int is_path_empty(struct path *p){
    return p->begin == NULL;
}
void path_destroy(struct path *p){ 
    if(p == NULL) return;

    if(!is_path_empty(p)){
        struct path_node * curr = p->begin;
        while(curr != NULL){
            struct path_node *next = curr->next;
            free(curr->name);
            free(curr);
            curr = next;
        }
    }

    free(p);
}

void path_push(struct path *p , const char *name){
    if(!p) return;
    
    struct path_node *pn = create_path_node(name);

    if(pn == NULL) return;
    if(is_path_empty(p)){
        p->begin = p->end = pn;
        return;
    }

    pn->prev = p->end;
    p->end->next = pn;
    p->end = p->end->next;
}

void path_pop(struct path *p){
    if(!p || is_path_empty(p)) return;

    if(p->begin == p->end){
        free(p->begin);
        p->begin = p->end = NULL;
        return;
    }

    p->end = p->end->prev;
    free(p->end->next->name);
    free(p->end->next);
    p->end->next = NULL;

}