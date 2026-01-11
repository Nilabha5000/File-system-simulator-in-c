#ifndef PATH_H
#define PATH_H

struct path_node{
    char *name;
    struct path_node *next;
    struct path_node *prev;
};

struct path{
    struct path_node *begin;
    struct path_node *end;
};

struct path *create_path();
int is_path_empty(struct path *);
void path_push(struct path *, const char *name);
void path_pop(struct path *);
struct path *get_path(const char *);
void path_destroy(struct path *);
#endif