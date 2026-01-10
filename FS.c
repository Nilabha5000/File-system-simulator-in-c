#include "FS.h"
#include "stack.h"
#include "queue.h"
#include <errno.h>
#include <string.h>

struct FS *initFS(char *root_name){
     struct FS *fs = (struct FS*)malloc(sizeof(struct FS));
     if(fs == NULL) return NULL;
     fs->root = make_directory(root_name);
     if(fs->root == NULL) return NULL;
     fs->current_dir = fs->root;
     fs->s = init_stack();
     fs->current_path = create_path();
     path_push(fs->current_path,fs->root->dir_name);
     push(&fs->s , fs->current_dir);
     return fs;
}
struct dir *make_directory(char *dir_name){
    struct dir *d = (struct dir*)malloc(sizeof(struct dir));
    if(d == NULL) return NULL;
    d->dir_name = (char*)malloc(strlen(dir_name)+2);
    if(d->dir_name == NULL){
        free(d);
        return NULL;
    }
    int start_from = 0;
    if(dir_name[0] != '/'){
        d->dir_name[0] = '/';
        d->dir_name[1] = '\0';
        start_from = 1;
    }
    strcpy(d->dir_name+start_from , dir_name);
    d->child = create_table();
    if(d->child == NULL){
         free(d->dir_name);
         free(d);
         return NULL;
    }
    // initailizing all file as NULL means no file
      d->files = create_table();
     if(d->files == NULL){
         free(d->dir_name);
         d->dir_name = NULL;
         destroy_obj_table(d->child);
         free(d);
         return NULL;
     }
    return d;
}

void create_file(struct FS *fs , const char *filename){
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
      
      if(get_obj(fs->current_dir->files,filename)){
          perror("file already exists");
          return;
      }
      
      
      // Allocate memory for the new file.
      struct file *newfile = (struct file*)malloc(sizeof(struct file));
      // Check if memory allocation was successful
      if(newfile == NULL) {
           perror("Memory for file allocation failed");
           return;
      }
      
      newfile->file_name = (char*)malloc(strlen(filename)+1);
      // Copy the file name into the directory's files array
      strcpy(newfile->file_name, filename);
      //setting the content buffer with 0.
      memset(newfile->content_buffer , 0 , MAX_CONTENT_LEN);
     //check if the file insertion failed then free the memory of file and filename
     if(!insert_obj(fs->current_dir->files,filename, newfile)){
          free(newfile->file_name);
          free(newfile);
          perror("file creation failed");
     }
}

void remove_file(struct FS *fs , const char *filename){
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
     
     //search for a filename 
     struct file *getfile = (struct file*)del_obj(fs->current_dir->files,filename);
     
     if(getfile == NULL){
          perror("file not found");
     }
     else{
          free(getfile->file_name);
          free(getfile);
     }
}

void write_file(struct FS *fs , const char *content , const char *filename){
     if(fs == NULL){
          perror("file system not created");
          return;
     }

     // Check if the current directory is NULL.
     if(fs->current_dir == NULL)
     {
           perror("directory does not exist");
           return;
     }

     //search for file with a given filename in the current directory.
     struct file *getfile = (struct file*)get_obj(fs->current_dir->files,filename);
   

     if(getfile == NULL){
          perror("file of this name does not exist in current directory");
          return;
     }

     strncpy(getfile->content_buffer,content,MAX_CONTENT_LEN);
}
void edit_file(struct FS *fs , const char *filename){
     if(fs == NULL){
          perror("file system not created");
          return;
     }

     struct file *getfile = (struct file*)get_obj(fs->current_dir->files,filename);
    

     if(getfile == NULL){
          perror("file of this name does not exist in current directory");
          return;
     }

     //open a temp file

     FILE *temp_filew = fopen("temp.txt", "w");
     if(temp_filew == NULL){
          perror("error to edit this file.\n");
          return;
     }
     fprintf(temp_filew,"%s", getfile->content_buffer);
     fclose(temp_filew);
     char cmd[100] = {0};
     memset(cmd,0,100);
     strcpy(cmd,"vim ");
     strcat(cmd,"temp.txt");
     
     //opening the vim editor.
     system(cmd);

     //saving contents to a file.
     char ch = '\0';
     int index = 0;
     FILE *temp_filer = fopen("temp.txt", "r");
     while((ch = fgetc(temp_filer)) != EOF && index < MAX_CONTENT_LEN-1){
          getfile->content_buffer[index++] = ch;
     }
     getfile->content_buffer[index] = '\0';
     fclose(temp_filer);

     remove("temp.txt");
}
void show_file_content(struct FS *fs , const char *filename){
     if(fs == NULL){
          perror("file system not created");
          return;
     }

     // Check if the current directory is NULL.
     if(fs->current_dir == NULL)
     {
           perror("directory does not exist");
           return;
     }
     //search for file with a given filename in the current directory.
     struct file *getfile = (struct file*)get_obj(fs->current_dir->files,filename);

     if(getfile == NULL){
          perror("file of this name does not exist in current directory");
          return;
     }
     if(strlen(getfile->content_buffer) == 0) return;
     printf("%s\n",getfile->content_buffer);

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
          return NULL;
     }
     char slashed_dir_name[50] = {0};
     if(dir_name[0] != '/'){
         slashed_dir_name[0] = '/';
         strcpy(slashed_dir_name+1,dir_name);
     }
     else
       strcpy(slashed_dir_name,dir_name);
        // Search for the directory in the current directory's children
        struct dir *changed_dir = (struct dir*)get_obj(fs->current_dir->child,slashed_dir_name);
        if(changed_dir == NULL){
               perror("Directory not found");
               return NULL;
        }
         path_push(fs->current_path,slashed_dir_name);
         push(&fs->s, changed_dir);
         return changed_dir;
        
}
// Function that returns the previous directory
struct dir *go_back_to_prev(struct FS *fs){
        if(!is_empty(&fs->s)){
           pop(&fs->s);
           path_pop(fs->current_path);
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
        
     char slashed_dir_name[256] = {0};
     memset(slashed_dir_name,0,256);
     if(dir_name[0] != '/'){
         slashed_dir_name[0] = '/';
         strncpy(slashed_dir_name+1,dir_name,255);
     }
     else{
       strncpy(slashed_dir_name,dir_name,256);
     }
       
     //check for if current directory has a directory of dir_name.
     if(get_obj(fs->current_dir->child,slashed_dir_name)){
          perror("directory already exists");
          return;
     }

     struct dir *new_dir = make_directory(slashed_dir_name);
     if(fs->current_dir->child == NULL || new_dir == NULL){
          perror("directory creation failed");
          return;
     }
     insert_obj(fs->current_dir->child,slashed_dir_name,new_dir);
      
}
void delete_dir_tree(struct dir *root){
       if(root == NULL){
           perror("its NULL");
           return;
       }
       if(root->child == NULL) return;
       struct queue *q = q_init();
       q_push(q,root);
       while(!q_empty(q)){
          struct dir *temp = q_front(q);
          q_pop(q);
          if(temp->child){
               for(int i = 0; i < temp->child->size; ++i){
                  struct bucket *curr = temp->child->table[i];
                  while(curr){
                      q_push(q,(struct dir*)curr->value);
                      curr = curr->next;
                  }
               }
          }
          
            free(temp->dir_name);
            temp->dir_name = NULL;
            for(int i = 0; i < temp->files->size; ++i){
                  struct bucket *curr1 = temp->files->table[i];
                  while(curr1){
                     struct file *getfile = (struct file*)curr1->value;
                     free(getfile->file_name);
                     free(getfile);
                    curr1 = curr1->next;
                  }
                  
            }
            destroy_obj_table(temp->child);
            destroy_obj_table(temp->files);
            free(temp);
       }
       
       q_destroy(q);
      printf("\n done \n");
}

void delete_dir(struct dir *d , char *dir_name){
     if(d == NULL){
         perror("can not perform delete in null directory !");
         return;
     }
     // check if the directory name is valid
     if(dir_name == NULL || strlen(dir_name) == 0){
        perror("Invalid directory name !");
        return;
     }
     //check for if the directory has no directories
     if(d->child == NULL || d->child->count == 0){
         perror("This directory has no directories !");
         return;
     }
     char slashed_dir_name[50] = {0};
     if(dir_name[0] != '/'){
         slashed_dir_name[0] = '/';
         strcpy(slashed_dir_name+1,dir_name);
     }
     else
         strcpy(slashed_dir_name,dir_name);
     // search for the directory name

     struct dir *recieved_dir = (struct dir*)get_obj(d->child,slashed_dir_name);
     if(recieved_dir == NULL){
           perror("this directory does not exist !");
           return;
     }
     delete_dir_tree(recieved_dir);
     del_obj(d->child,slashed_dir_name);
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
      if((fs->current_dir->child->count == 0) && fs->current_dir->files->count == 0){
            printf("\nThe directory is empty \n \n");
            return;
      }
      printf("\nInside %s directory\n\n", fs->current_dir->dir_name);
     //first showing all the directories in current directory;
     for(int i = 0; i < fs->current_dir->child->size; ++i){
          struct bucket *node1 = fs->current_dir->child->table[i];
          while(node1){
               struct dir *d = (struct dir*)node1->value;
               printf("%s                   DIR\n\n",d->dir_name);
               node1 = node1->next;
          }
         
     }
     //first showing all the files in current directory;
     for(int i = 0; i < fs->current_dir->files->size; ++i){
          struct bucket *node2 = fs->current_dir->files->table[i];
          while(node2){
               struct file *f = (struct file*)node2->value;
               printf("%s                   FILE\n\n",f->file_name);
               node2 = node2->next;
          }
         
     }
}

void destroy_FS(struct FS *fs){
     if(fs == NULL){
         return;
     }
    
    delete_dir_tree(fs->root);
    
    destroy_stack(fs->s);
    path_destroy(fs->current_path);
    free(fs);
    printf("\ndone\n");

}
