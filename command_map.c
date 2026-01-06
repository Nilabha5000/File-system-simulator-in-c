#include "command_map.h"
#include <string.h>

struct nlist **init(){
    struct nlist **res = (struct nlist**)malloc(sizeof(struct nlist*)*MAXLEN);
    
    for(int i = 0; i < MAXLEN; ++i)
       res[i] = NULL;
    return res;
}

unsigned hash(char *s){
       unsigned hashval;
       for(hashval = 0 ; *s != '\0'; s++)
        {
             hashval = *s + 31 *hashval;
        }
     return hashval % MAXLEN;
}

void set(struct nlist **list , char *command , void(*value)(void *)){
      unsigned index = hash(command);
      //allocating a new node 
     struct nlist *np = NULL;
      np = (struct nlist*)malloc(sizeof(nlist));
      np->next = NULL;
      np->command = (char*)malloc(sizeof(char)*strlen(command)+1);
      np->func = value;
      strcpy(np->command, command);
      
     // if the list index is empty then we assign it with np
      if(list[index] == NULL){
           list[index] = np;
           return;
      }

      //adding the node at last of list index
     struct nlist *i = NULL;
      for(i = list[index]; i->next != NULL ; i = i->next);
      i->next = np;
      
}

func_t get_func(struct nlist**list , char *command){
     unsigned index = hash(command);
         for(nlist *i = list[index]; i != NULL; i = i->next){
              if(strcmp(i->command, command) == 0){
                     return i->func;
               }
         }
      return NULL;
}


void destroylist(struct nlist ** list){
     //code for deallocating list
     for(int i = 0; i < MAXLEN; ++i){
           //if list[i] is not NULL then deallocate all the nodes of list[i]
           if(list[i] != NULL){
                 nlist *curr = list[i];
                 nlist *prev = NULL;
                 while(curr != NULL){
                    prev = curr;
                    free(curr->command);
                    curr = curr->next;
                    free(prev);
                 }
           }
      }
     free(list);

}
