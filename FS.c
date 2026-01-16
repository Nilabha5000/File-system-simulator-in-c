#include "FS.h"
#include "stack.h"
#include "queue.h"
#include <errno.h>
#include <string.h>

void save_dir(struct dir *d, FILE *fp);

struct FS *initFS(){
     struct FS *fs = (struct FS*)malloc(sizeof(struct FS));
     if(fs == NULL) return NULL;
     fs->root = make_directory("root");
     if(fs->root == NULL) return NULL;
     fs->current_dir = fs->root;
     fs->s = init_stack();
     fs->current_path = create_path();
     path_push(fs->current_path,fs->root->dir_name);
     push(&fs->s , fs->current_dir);
     return fs;
}

struct dir *get_dest_dir(struct FS *fs, struct path *p){
     struct dir *dest_dir = fs->current_dir;
      for(struct path_node *i = p->begin; i != p->end; i = i->next){
          dest_dir = (struct dir*)get_obj(dest_dir->child,i->name);
          if(dest_dir == NULL){
               perror("directory not found");
               return NULL;
          }
      }
     return dest_dir;
}
char *get_slashed_path(const char *path){
      char *slash_path = (char*)malloc(strlen(path)+2);
      if(path[0] != '/'){
          slash_path[0] = '/';
          strcpy(slash_path+1,path);
      }
      else{
          strcpy(slash_path,path);
      }
      slash_path[strlen(slash_path)] = '\0';
     return slash_path;
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

void create_file(struct FS *fs , const char *file_path){
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
       char *slash_file_path = get_slashed_path(file_path);
      struct path *p = get_path(slash_file_path);
      free(slash_file_path);
      if(p == NULL){
          perror("file cannot be crated because of path failure ");
          return;
      }
      //this is reash destination dir where to create file
      struct dir *dest_dir = get_dest_dir(fs,p);
      if(dest_dir == NULL){
          path_destroy(p);
          return;
      }
      char *filename = strdup(p->end->name);
      int start_from = 0;
      if(filename[0] == '/'){start_from = 1;}
      if(get_obj(dest_dir->files,filename+start_from)){
          perror("file already exists");
          path_destroy(p);
          free(filename);
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
      strcpy(newfile->file_name, filename+start_from);
      //setting the content buffer with 0.
      memset(newfile->content_buffer , 0 , MAX_CONTENT_LEN);
     //check if the file insertion failed then free the memory of file and filename
     if(!insert_obj(dest_dir->files,filename + start_from, newfile)){
          free(newfile->file_name);
          free(newfile);
          free(filename);
          path_destroy(p);
          perror("file creation failed");
          return;
     }

     free(filename);
     path_destroy(p);
}

void remove_file(struct FS *fs , const char *file_path){
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
      char *slash_file_path = get_slashed_path(file_path);
      struct path *p = get_path(slash_file_path);
      free(slash_file_path);
      if(p == NULL){
          perror("file cannot be crated because of path failure ");
          return;
      }
      //this is reash destination dir where to create file
      struct dir *dest_dir = get_dest_dir(fs,p);
      if(dest_dir == NULL)
      {
          path_destroy(p);
          return;
      }
     char *filename = strdup(p->end->name);
     int start_from = 0;
     if(filename[0] == '/')
       start_from = 1;
     //search for a filename 
     struct file *getfile = (struct file*)del_obj(dest_dir->files,filename + start_from);
     free(filename);
     if(getfile == NULL){
          perror("file not found");
     }
     else{
          free(getfile->file_name);
          free(getfile);
     }
     path_destroy(p);
}

void write_file(struct FS *fs , const char *content , const char *file_path){
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
     char *slash_file_path = get_slashed_path(file_path);
      struct path *p = get_path(slash_file_path);
      free(slash_file_path);
      if(p == NULL){
          perror("file cannot be crated because of path failure ");
          return;
      }
      //this is reash destination dir where to create file
      struct dir *dest_dir = get_dest_dir(fs,p);

     char *filename = strdup(p->end->name);
     int start_from = 0;
     if(filename[0] == '/')
       start_from = 1;
     //search for file with a given filename in the current directory.
     struct file *getfile = (struct file*)get_obj(dest_dir->files,filename + start_from);
     free(filename);

     if(getfile == NULL){
          perror("file of this name does not exist in current directory");
          path_destroy(p);
          return;
     }
     path_destroy(p);
     strncpy(getfile->content_buffer,content,MAX_CONTENT_LEN-1);

}
void edit_file(struct FS *fs , const char *file_path){
     if(fs == NULL){
          perror("file system not created");
          return;
     }
      char *slash_file_path = get_slashed_path(file_path);
      struct path *p = get_path(slash_file_path);
      free(slash_file_path);
      if(p == NULL){
          perror("file cannot be crated because of path failure ");
          return;
      }
      //this is reash destination dir where to create file
      struct dir *dest_dir = get_dest_dir(fs,p);
      if(dest_dir == NULL){
          path_destroy(p);
          return;
      }
      char *filename = strdup(p->end->name);
      int start_from = 0;
     if(filename[0] == '/')
       start_from = 1;
     struct file *getfile = (struct file*)get_obj(dest_dir->files,filename + start_from);
     free(filename);
     if(getfile == NULL){
          path_destroy(p);
          perror("file of this name does not exist in current directory");
          return;
     }
     path_destroy(p);
     //open a temp file

     FILE *temp_filew = fopen("temp.txt", "w");
     if(temp_filew == NULL){
          perror("error to edit this file.\n");
          return;
     }
     fprintf(temp_filew,"%s", getfile->content_buffer);
     fclose(temp_filew);
     
     //opening the vim editor.
     system("vim temp.txt");

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
void show_file_content(struct FS *fs , const char *file_path){
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
      char *slash_file_path = get_slashed_path(file_path);
      struct path *p = get_path(slash_file_path);
      free(slash_file_path);
      if(p == NULL){
          perror("file cannot be crated because of path failure ");
          return;
      }
      //this is reash destination dir where to create file
      struct dir *dest_dir = get_dest_dir(fs,p);
      if(dest_dir == NULL){
          path_destroy(p);
          return;
      }
      char *filename = strdup(p->end->name);
      int start_from = 0;
     if(filename[0] == '/')
       start_from = 1;
     path_destroy(p);
     struct file *getfile = (struct file*)get_obj(dest_dir->files,filename + start_from);
     free(filename);
     if(getfile == NULL){
          perror("file of this name does not exist in current directory");
          return;
     }
     if(strlen(getfile->content_buffer) == 0) return;
     printf("%s\n",getfile->content_buffer);

}
struct dir *change_directory(struct FS *fs ,char * dir_path){
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
     if(dir_path == NULL || strlen(dir_path) == 0) {
          perror("Invalid directory name");
          return NULL;
     }
     char *slashed_dir_path = get_slashed_path(dir_path);

       struct path *p = get_path(slashed_dir_path);
       free(slashed_dir_path);
       if(p == NULL)
         return NULL;
        // Search for the directory in the current directory's children
        struct path_node *pn = p->begin;
        struct path_node *prev_end = p->end;
        struct dir *prev_top = peek(&fs->s);
        struct dir *changed_dir = (struct dir*)get_obj(fs->current_dir->child,pn->name);
        if(changed_dir == NULL){
               perror("Directory not found");
               path_destroy(p);
               return NULL;
         }
         pn = pn->next;
         push(&fs->s,changed_dir);
         path_push(fs->current_path,changed_dir->dir_name);
        while(pn != NULL){
          changed_dir = (struct dir*)get_obj(changed_dir->child,pn->name);
          if(changed_dir == NULL){
               perror("Directory not found");
               while(peek(&fs->s) != prev_top && fs->current_path->end != prev_end){
                    pop(&fs->s);
                    path_pop(fs->current_path);
               }
               path_destroy(p);
               return NULL;
          }
          path_push(fs->current_path,pn->name);
          push(&fs->s, changed_dir);
          
          pn = pn->next;
        }
         fs->current_dir = changed_dir;
         path_destroy(p);
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
void make_directory_in_a_directory(struct FS *fs , const char *dir_path){
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
     if(dir_path == NULL || strlen(dir_path) == 0){
          perror("Invalid directory name");
          return;
     }
        
     char *slashed_dir_path = get_slashed_path(dir_path);
     
     struct path *p = get_path(slashed_dir_path);
     free(slashed_dir_path);
     if(p == NULL){
          return;
     }

     char *dir_name = p->end->name;

     struct dir *dest_dir = get_dest_dir(fs,p);
     if(dest_dir == NULL){
          path_destroy(p);
          return;
     }
     //check for if current directory has a directory of dir_name.
     if(get_obj(dest_dir->child,dir_name)){
          perror("directory already exists");
          path_destroy(p);
          return;
     }

     struct dir *new_dir = make_directory(dir_name);
     if(dest_dir->child == NULL || new_dir == NULL){
          perror("directory creation failed");
          path_destroy(p);
          return;
     }
     insert_obj(dest_dir->child,dir_name,new_dir);
     path_destroy(p);
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
               for(struct bucket_list *i = temp->child->begin; i != NULL; i = i->next){
                  struct bucket *curr = i->start;
                  while(curr){
                      q_push(q,(struct dir*)curr->value);
                      curr = curr->next;
                  }
               }
          }
          
            free(temp->dir_name);
            temp->dir_name = NULL;
            for(struct bucket_list* i = temp->files->begin; i != NULL; i = i->next){
                  struct bucket *curr1 = i->start;
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
}

void delete_dir(struct FS *fs , char *dir_path){
     if(fs == NULL){
         perror("can not perform delete in null directory !");
         return;
     }
     // check if the directory name is valid
     if(dir_path == NULL || strlen(dir_path) == 0){
        perror("Invalid directory name !");
        return;
     }
     //check for if the directory has no directories
     if(fs->current_dir->child == NULL || fs->current_dir->child->count == 0){
         perror("This directory has no directories !");
         return;
     }
     char *slashed_dir_path = get_slashed_path(dir_path);

     struct path *p = get_path(slashed_dir_path);
     free(slashed_dir_path);
     if(p == NULL){
          perror("path failed");
          return;
     }
     // search for the directory name
     struct dir *dest_dir = get_dest_dir(fs,p);
     if(dest_dir == NULL){
          path_destroy(p);
          return;
     }
     struct dir *recieved_dir = (struct dir*)del_obj(dest_dir->child,p->end->name);

     if(recieved_dir == NULL){
           perror("this directory does not exist !");
           path_destroy(p);
           return;
     }
     delete_dir_tree(recieved_dir);
     path_destroy(p);
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
     for(struct bucket_list* i = fs->current_dir->child->begin; i != NULL; i = i->next){
          struct bucket *node1 = i->start;
          while(node1){
               struct dir *d = (struct dir*)node1->value;
               printf("%s                   DIR\n\n",d->dir_name);
               node1 = node1->next;
          }
         
     }
     //first showing all the files in current directory;
     for(struct bucket_list* i = fs->current_dir->files->begin; i != NULL; i = i->next){
          struct bucket *node2 = i->start;
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

int save_fs(struct FS *fs , const char *filename){
     FILE *fp = fopen(filename, "wb");
     if(!fp) return 0;
     
     save_dir(fs->root,fp);
     fclose(fp);
     return 1;
}
void save_dir(struct dir *d , FILE *fp){
       
     int n = strlen(d->dir_name);
     fwrite(&n,sizeof(n), 1 , fp);
     fwrite(d->dir_name,1,n,fp);
     
     int file_count = d->files->count;
     fwrite(&file_count,sizeof(file_count), 1 , fp);
     for(struct bucket_list *i = d->files->begin; i != NULL; i = i->next){
          struct bucket *b = i->start;
          while(b != NULL){
               struct file *f = (struct file *)b->value;
               int file_name_len = strlen(f->file_name);
               fwrite(&file_name_len, sizeof(file_name_len), 1 , fp);
               fwrite(f->file_name,1,file_name_len,fp);

               int content_len = strlen(f->content_buffer);
               fwrite(&content_len,sizeof(content_len), 1 , fp);
               fwrite(f->content_buffer, 1 , content_len, fp);
               b = b->next;
          }
          
     }
     int dir_count = d->child->count;
     fwrite(&dir_count,sizeof(dir_count), 1 , fp);
      for(struct bucket_list *i = d->child->begin; i != NULL; i = i->next){
          struct bucket *b = i->start;
          while(b != NULL){
               struct dir *directory = (struct dir *)b->value;
               save_dir(directory,fp);
               b = b->next;
          }
          
     }

}
struct dir *create_fs_tree(FILE *fp){
     int len;
    fread(&len, sizeof(len), 1, fp);

    char *name = malloc(len + 1);
    fread(name, 1, len, fp);
    name[len] = '\0';

    struct dir *d = make_directory(name);
    free(name);

    int file_count;
    fread(&file_count, sizeof(file_count), 1, fp);
    //retriving all the files and its content 
    for(int i = 0; i < file_count; i++){
        //recreating a file of file name and file content
        struct file *f = (struct file *)malloc(sizeof(struct file));
        int file_name_len;
        fread(&file_name_len,sizeof(file_name_len), 1 , fp);
        f->file_name = (char*)malloc(file_name_len + 1);
        memset(f->file_name , 0,file_name_len);
        fread(f->file_name,1,file_name_len,fp);
        f->file_name[file_name_len] = '\0';
       // retriving the file content 
       int content_len;
       fread(&content_len,sizeof(content_len), 1, fp);
       memset(f->content_buffer, 0 , MAX_CONTENT_LEN);
       fread(f->content_buffer,1,content_len,fp);
       f->content_buffer[content_len] = '\0';
       //inserting the file in a hash_map.
       insert_obj(d->files,f->file_name,f);

    }

    int dir_count;
    fread(&dir_count, sizeof(dir_count), 1, fp);

    for(int i = 0; i < dir_count; i++){
        struct dir *child = create_fs_tree(fp);
        insert_obj(d->child, child->dir_name, child);
    }

    return d;
}
struct FS *load_FS(const char *filename){
    struct FS *fs = initFS();
    FILE *fp = fopen(filename , "rb");
    if(!fp) return fs;
    
    delete_dir_tree(fs->root);

    fs->root = create_fs_tree(fp);
    fs->current_dir = fs->root;
    fclose(fp);

    return fs;
}