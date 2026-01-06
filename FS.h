#ifndef FS_H
#define FS_H

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "obj_map.h"

// Define maximum number of files and directories in a directory
#define MAX_CONTENT_LEN 4007
struct stack;

struct file{
    char *file_name;
    char content_buffer[MAX_CONTENT_LEN];
};
struct dir{
     struct obj_table *files;
     char *dir_name;
     struct obj_table *child;
};

struct FS{
    struct dir *root;
    struct dir *current_dir;
    struct stack *s;

};

struct FS *initFS(char *);
struct dir *make_directory(char *);
void make_directory_in_a_current_directory(struct FS* , char *);
void create_file(struct FS * , const char *);
void write_file(struct FS * , const char *, const char *);
void edit_file(struct FS * , const char *);
void show_file_content(struct FS *, const char *);
void remove_file(struct FS * , const char *);
struct dir *change_directory(struct FS*,char *);

struct dir *go_back_to_prev(struct FS*);
void delete_dir_tree(struct dir *);
void delete_dir(struct dir *, char *);
void view_contents(struct FS *);
void destroy_FS(struct FS *);
#endif
