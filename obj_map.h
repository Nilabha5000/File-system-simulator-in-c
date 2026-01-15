#ifndef OBJ_MAP_H
#define OBJ_MAP_H

struct bucket{
   char *key;
   void *value;
   struct bucket *next;
};
struct bucket_list{
    struct bucket *start;
    struct bucket_list *next;
    struct bucket_list *prev;
};
struct obj_table{
    struct bucket_list **table;
    struct bucket_list *begin;
    struct bucket_list *end;
    int count;
    int size;
    int min_cap;
};

struct obj_table *create_table();

void *get_obj(struct obj_table * , const char *);
int empty(struct obj_table *);
void *del_obj(struct obj_table *, const char *);
int insert_obj(struct obj_table * , const char *, void *);
void destroy_obj_table(struct obj_table *);
#endif