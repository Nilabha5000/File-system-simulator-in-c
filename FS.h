#ifndef FS_H
#define FS_H

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

// Define maximum number of files and directories in a directory
#define MAX_CONTAIN 100
struct stack;
struct dir{
     char *files[MAX_CONTAIN];
     int file_index;
     int child_index;
     char *dir_name;
     struct dir *child[MAX_CONTAIN];
};

struct FS{
    struct dir *root;
    struct dir *current_dir;
    struct stack *s;

};

struct FS *initFS(char *);
struct dir *make_directory(char *);
void make_directory_in_a_current_directory(struct FS* , char *);
void create_file(struct FS * , char *);
struct dir *change_directory(struct FS*,char *);

struct dir *go_back_to_prev(struct FS*);
void delete_dir_tree(struct dir *);
void view_contents(struct FS *);
void destroy_FS(struct FS *);
#endif
