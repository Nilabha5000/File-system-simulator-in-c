#include "FS.h"
#include "stack.h"
#include "queue.h"
#include <errno.h>
#include <string.h>

struct FS *initFS(char *root_name){
     struct FS *fs = (struct FS*)malloc(sizeof(struct FS));
     fs->root = make_directory(root_name);
     fs->current_dir = fs->root;
     fs->s = init();
     push(&fs->s , fs->current_dir);
     return fs;
}
struct dir *make_directory(char *dir_name){
    struct dir *d = (struct dir*)malloc(sizeof(struct dir));
    d->dir_name = (char*)malloc(strlen(dir_name)+1);
    strcpy(d->dir_name , dir_name);
    // initailizing all child as NULL
    for(int i = 0; i < MAX_CONTAIN; ++i){
        d->child[i] = NULL;
    }
    // initailizing all file as NULL means no file
    for(int i = 0; i < MAX_CONTAIN; ++i)
         d->files[i] = NULL;
     //intailizing the file and index with -1;
     d->file_index = -1;
     d->child_index = -1;
    return d;
}

void create_file(struct FS *fs , char *filename){
     if(fs == NULL){
           perror("file system not created");
           return;
     }
    // Check if the current directory is NULL
      if(fs->current_dir == NULL)
      {
           perror("directory does not exist");
           return;
      }

      int index = ++fs->current_dir->file_index;
      if(index >= MAX_CONTAIN) {
           perror("Maximum file limit reached");
           return;
      }
      // Allocate memory for the new file name and copy it
      fs->current_dir->files[index] = (char*)malloc(strlen(filename)+1);
      // Check if memory allocation was successful
      if(fs->current_dir->files[index] == NULL) {
           perror("Memory allocation failed");
           return;
      }
      // Copy the file name into the directory's files array
      strcpy(fs->current_dir->files[index], filename);
}

struct dir *change_directory(struct FS *fs ,char * dir_name){
      if(fs == NULL){
           perror("file system not created");
           return NULL;
      }
     // Check if the current directory is NULL
     if(fs->current_dir == NULL) {
          perror("Current directory does not exist");
          return NULL;
     }
     
     // Check if the directory name is valid
     if(dir_name == NULL || strlen(dir_name) == 0) {
          perror("Invalid directory name");
          return peek(&fs->s);
     }
        // Search for the directory in the current directory's children
         for (int i = 0; i <= fs->current_dir->child_index; i++)
         {
            if (strcmp(fs->current_dir->child[i]->dir_name, dir_name) == 0)
            {
                // Directory found, push current directory onto stack
                push(&fs->s, fs->current_dir->child[i]);
                return fs->current_dir->child[i];
            }
         }
         perror("Directory not found");
         return peek(&fs->s);
}
// Function that returns the previous directory
struct dir *go_back_to_prev(struct FS *fs){
        if(!is_empty(&fs->s)){
           pop(&fs->s);
          return peek(&fs->s);
        }
      return NULL;
}
void make_directory_in_a_current_directory(struct FS *fs , char *dir_name){
        if(fs == NULL){
           perror("file system not created");
           return;
        }
        //check if the current directory is NULL
        if(fs->current_dir == NULL){
             perror("Current directory does not exist");
             return;
        }
        // Check if the directory name is valid
        if(dir_name == NULL || strlen(dir_name) == 0){
                perror("Invalid directory name");
                return;
        }
      int index = ++fs->current_dir->child_index;
      //if(index >= MAX_CONTAIN)
      fs->current_dir->child[index] = make_directory(dir_name);
}
void delete_dir_tree(struct dir *root){
       if(root == NULL){
           perror("its NULL");
           return;
       }
       
       struct queue *q = q_init();
       q_push(q,root);
       while(!q_empty(q)){
            struct dir *temp = q_front(q);
            q_pop(q);
            for(int i = 0; i <= temp->child_index; ++i){
                q_push(q,temp->child[i]);
            }
            
            free(temp->dir_name);
            for(int i = 0; i <= temp->file_index; ++i){
                  free(temp->files[i]);
            }
            free(temp);
       }
       
       q_destroy(q);
      printf("\n done \n");
}
void view_contents(struct FS *fs){
      if(fs == NULL){
           perror("file system not created");
           return;
     }
      if(fs->current_dir == NULL)
      {
           perror("current dir is NULL");
           return;
      }
      if(fs->current_dir->file_index == -1 && fs->current_dir->child_index == -1){
            printf("\nThe directory is empty \n \n");
            return;
      }
      printf("\nInside /%s directory\n\n", fs->current_dir->dir_name);
     //first showing all the directories in current directory;
     for(int i = 0; i <= fs->current_dir->child_index; ++i){
          printf("/%s                   DIR\n\n",fs->current_dir->child[i]->dir_name);
     }
     //first showing all the files in current directory;
     for(int i = 0; i <= fs->current_dir->file_index; ++i){
          printf("%s                   FILE\n\n",fs->current_dir->files[i]);
     }
}

void destroy_FS(struct FS *fs){
     if(fs == NULL){
         return;
     }
    
    delete_dir_tree(fs->root);
    
    destroy_stack(fs->s);
    free(fs);
    printf("\ndone\n");

}




