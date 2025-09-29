#ifndef MAP_H
#define MAP_H
#include <stdlib.h>
#define MAXLEN 100

struct nlist{
     char *command;
     void (*func) (void *);
     struct nlist * next;
};
typedef void (*func_t)(void *);

typedef struct nlist nlist;

struct nlist **init(); // for initailizing the hashmap
unsigned hash(char*); // hash function
func_t get_func(struct nlist ** , char *); // this returns a pagename with respect to specific path . If the path does not exist it returns NULL.
void set(struct nlist ** , char * , void (*)(void*)); // it maps section with the pagename
void destroylist(struct nlist**);  // for deallocating the nlist

#endif
