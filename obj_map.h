#ifndef OBJ_MAP_H
#define OBJ_MAP_H

struct bucket{
   char *key;
   void *value;
   struct bucket *next;
};

struct obj_table{
    struct bucket **table;
    int count;
    int size;
    int min_cap;
};

struct obj_table *create_table();

void *get_obj(struct obj_table * , const char *);
void *del_obj(struct obj_table *, const char *);
int insert_obj(struct obj_table * , const char *, void *);
void destroy_obj_table(struct obj_table *);
#endif