#include "FS.h"
#include "map.h"
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
// This is a command structure 
// that holds the command and its arguments
typedef struct cmd{
  int argc;
  char *argv[50];  
}cmd;
// This is a history node structure
// It holds a command and a pointer to the next node
// It is used to store the history of commands entered by the user
// The history is managed as a linked list
// The head of the list is the most recent command
// and the tail is the oldest command
// The history can be used to retrieve the last commands entered by the user
// It can be used to implement the "history" command in the shell
// The history can be printed to the screen
typedef struct hst_node{
    char *command;
    struct hst_node *next;
}hst_node;
typedef struct mng_history{
    hst_node *head , *tail;
}mng_history;
// This is a list of commands that the shell supports
// The commands are indexed from 0 to 7
// and correspond to the cases in the switch statement
// in the main function
// This is a path structure that holds the current path
// It is used to keep track of the current directory
// and the directories that have been visited
// It is used to print the current path in the shell
// when the user enters a command
typedef struct path{
    int count;
    char *vector[100];
}path;

struct essentail_items{
    struct FS *fs;
    path *p;
    cmd *c;
    mng_history *history;
};
// This function extracts the tokens from the command
// and stores them in the cmd structure
// It uses dynamic memory allocation to create a new token
// for each argument
// It also handles spaces in the command
// and removes them before storing the tokens
// The tokens are stored in the argv array of the cmd structure
// and the argc variable is incremented for each token found
// The last token is always NULL to indicate the end of the arguments
void extract_tokens(cmd *c , char *command){
     int token_size = 20;
      char *token = (char*)malloc(token_size);
      if(token == NULL) return;
      char *temp = NULL;
      int l = 0, r = strlen(command)-1;
      int j = 0; // for token index
      bool is_quote_open = false;
       //for left trimming.
       while(command[l] == ' ') l++;
       //for right trimming.
       while(command[r] == ' ')  r--;
      while(l < r+1){
          if(command[l] == '\"'){
              if(is_quote_open) is_quote_open = false;
              else is_quote_open = true;
          }
          else if(is_quote_open || command[l] != ' '){
             //check the size if it the cherecters exceeds the size limit
            if(j >= token_size){
                 token_size *= 2;
                 temp = realloc(token , token_size);
                 //checking for NULL
                 if(temp == NULL){
                    free(token);
                    return;
                 }
                 else token = temp;
                 temp = NULL;
            }
            token[j] = command[l];
            j++;
          }
          else{
            
            token[j] = '\0';
            //check if the token is empty then free the token.
            if(strlen(token) == 0){
                free(token);
            }
            else if(c->argc >= 50){
                free(token);
                return;
            }
            else{
                c->argv[c->argc] = token;
                c->argc++;
            }
            token_size = 20;
            token = (char*)malloc(token_size);
            if(token == NULL) return;
            j = 0;
          }
         l++;
      }
       token[j] = '\0';
       if(strlen(token) == 0)
       {
            free(token);
            return;
       }
     c->argv[c->argc] = token;
     c->argc++;
}

// This function initializes the history manager
// It creates a new mng_history structure
// and sets the head and tail to NULL
// It returns a pointer to the new mng_history structure
mng_history *init_history(){
    mng_history *h = (mng_history*)malloc(sizeof(mng_history));
    h->head = NULL;
    h->tail = NULL;
    return h;
}
// This function adds a command to the history
// It creates a new hst_node structure  
void add_history(mng_history *h , char *command){
    hst_node *new_node = (hst_node*)malloc(sizeof(hst_node));
    new_node->command = (char*)malloc(strlen(command) + 1);
    strcpy(new_node->command, command);
    new_node->next = NULL;
    if(h->head == NULL){
        h->head = new_node;
        h->tail = new_node;
    }
    else{
        h->tail->next = new_node;
        h->tail = new_node;
    }
}
void print_history(mng_history *h){
    hst_node *current = h->head;
    int index = 1;
    while(current != NULL){
        printf("%d. %s\n", index, current->command);
        current = current->next;
        index++;
    }
}
void destroy_history(mng_history *h){
    hst_node *current = h->head;
    while(current != NULL){
        hst_node *temp = current;
        current = current->next;
        free(temp->command);
        free(temp);
    }
    free(h);
}

void wrap_history(void * arg){
    struct essentail_items *ei = (struct essentail_items *)arg;
    print_history(ei->history);
}

void wrap_view_contents(void *arg){
    struct essentail_items *ei = (struct essentail_items *)arg;
    view_contents(ei->fs);

}


void wrap_delete_dir(void *arg){
    struct essentail_items *ei = (struct essentail_items *)arg;
    delete_dir(ei->fs->current_dir,ei->c->argv[1]);
}

void wrap_create_file(void *arg){
    struct essentail_items *ei = (struct essentail_items *)arg;

    create_file(ei->fs,ei->c->argv[1]);
}

void wrap_rm(void *arg){
    struct essentail_items *ei = (struct essentail_items *)arg;

    remove_file(ei->fs, ei->c->argv[1]);
}
void wrap_mkdir(void * arg){
    struct essentail_items *ei = (struct essentail_items *)arg;

    if(ei->c->argc > 1)
        make_directory_in_a_current_directory(ei->fs , ei->c->argv[1]);
    else
        printf("mkdir needs more the one argument\n");
}

void wrap_go_back(void *arg){
    struct essentail_items *ei = (struct essentail_items *)arg;
    if(ei->fs->current_dir != ei->fs->root)
        ei->fs->current_dir = go_back_to_prev(ei->fs);
    if(ei->p->count > 1)
        ei->p->count--;
}

void wrap_exit(void *arg){
    int *is_running = (int*)arg;
    *is_running = 1;
}

void wrap_clear(void *){
    system("clear");
}
void wrap_python(void *){
    system("python3");
}
void wrap_pwd(void *arg){
    struct essentail_items *ei = (struct essentail_items *)arg;
     for(int i = 0; i < ei->p->count; ++i){
        printf("/%s",ei->p->vector[i]);
    }
    printf("\n");
}

void wrap_cd(void *arg){
    struct essentail_items *ei = (struct essentail_items *)arg;
    ei->fs->current_dir = change_directory(ei->fs , ei->c->argv[1]);
    if(ei->fs->current_dir != NULL){
        ei->p->vector[ei->p->count] = ei->fs->current_dir->dir_name;
        ei->p->count++;
    }
    else{
        ei->fs->current_dir = peek(&ei->fs->s);
    }
}
int main(void) {
    int running = 0;
    // This is the command buffer that holds the command entered by the user
    // It is a fixed size array of characters
    // It is used to read the command from the standard input
    // and to parse it into tokens
    char command[200] = {0};
    struct FS *fs = initFS("root");    
    path p;
    p.count = 1;
    p.vector[0] = fs->root->dir_name;
    cmd c;
    mng_history *history = init_history();
    nlist **map = init();

    set(map,"ls",wrap_view_contents);
    set(map,"mkdir", wrap_mkdir);
    set(map,"clear",wrap_clear);
    set(map,"exit",wrap_exit);
    set(map,"touch",wrap_create_file);
    set(map,"rm",wrap_rm);
    set(map,"rmdir",wrap_delete_dir);
    set(map,"back",wrap_go_back);
    set(map,"pwd",wrap_pwd);
    set(map,"cd",wrap_cd);
    set(map,"history",wrap_history);
    set(map,"python",wrap_python);
    struct essentail_items ei;
    ei.c = &c;
    ei.fs = fs;
    ei.history = history;
    ei.p = &p;
    while (!running) {
      // This is the prompt that shows the current path
       printf("\033[1;32m"); // Set text color to green
       printf("~");
       for(int i = 0; i < p.count; ++i){
        printf("/%s",p.vector[i]);
       }
       printf("$ ");
       fgets(command,200,stdin);
       c.argc = 0;
       
       command[strcspn(command, "\n")] = 0; // Remove newline
        add_history(history, command);
       extract_tokens(&c , command);
        func_t f = get_func(map, c.argv[0]);
        if(f == NULL){
             if(strlen(command) == 0 || strcmp(command , " ") == 0);
              else
                printf("%s: command not found\n", command);
        }
        else if(strcmp(c.argv[0],"exit") == 0){
            f(&running);
        }
        else if(strcmp(c.argv[0], "clear") == 0 || strcmp(c.argv[0], "python") == 0){
            f(NULL);
        }
        else if(f != NULL){
            f(&ei);
        }
            
                

         for(int i = 0; i < c.argc; ++i){
              free(c.argv[i]);
         }
        memset(command , 0 , 67);
    }
     
    // Free the file system resources
    destroy_history(history);
    destroy_FS(fs);
    destroylist(map);
    return 0;
}
