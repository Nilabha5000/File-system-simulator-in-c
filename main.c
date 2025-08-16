#include "FS.h"
#include <unistd.h>
#include <string.h>
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
// The commands are:
// 0: ls - list contents of the current directory
// 1: rmdir - remove a directory
// 2: touch - create a file
// 3: cd - change directory
// 4: mkdir - make a directory
// 5: back - go back to the previous directory
// 6: exit - exit the shell
// 7: clear - clear the screen
const char*command_list[] = {"ls", "rmdir", "touch", "cd" , "mkdir" , "back" , "exit" , "clear", "history"};
// This is a path structure that holds the current path
// It is used to keep track of the current directory
// and the directories that have been visited
// It is used to print the current path in the shell
// when the user enters a command
typedef struct path{
    int count;
    char *vector[100];
}path;
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
      char *token = (char*)malloc(20);
      int i = 0;
       int j = 0;
       while(command[i] == ' ') i++;
      while(command[i] != '\0'){
          if(command[i] != ' '){
            token[j] = command[i];
            j++;
          }
          else{
             token[j] = '\0';
             c->argv[c->argc] = token;
             c->argc++;
             token = (char*)malloc(20);
             j = 0;
          }
         i++;
      }
       token[j] = '\0';
     c->argv[c->argc] = token;
     c->argc++;
}
// This function returns the index of the command in the command_list array
// It compares the command with each command in the list
// and returns the index if a match is found 
int get_command_index(char *command){
     for(int i = 0; i < 9; ++i){    
         if(strcmp(command, command_list[i]) == 0)
                return i;
     }
    return -1;
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
int main(void) {
    int running = 0;
    int opt;
    // This is the command buffer that holds the command entered by the user
    // It is a fixed size array of characters
    // It is used to read the command from the standard input
    // and to parse it into tokens
    char command[67] = {0};
    struct FS *fs = initFS("root");    
    path p;
    p.count = 1;
    p.vector[0] = fs->root->dir_name;
    cmd c;
    mng_history *history = init_history();
    while (!running) {
      // This is the prompt that shows the current path
       printf("\033[1;32m"); // Set text color to green
       printf("~");
       for(int i = 0; i < p.count; ++i){
        printf("/%s",p.vector[i]);
       }
       printf("$ ");
       fgets(command,67,stdin);
       c.argc = 0;
       
       command[strcspn(command, "\n")] = 0; // Remove newline
        add_history(history, command);
       extract_tokens(&c , command);
       opt = get_command_index(c.argv[0]);
        switch (opt) {
            case 0:
                view_contents(fs);
                break;
            case 1:
               delete_dir(fs->current_dir,c.argv[1]);
               break;
            case 2:
               create_file(fs,c.argv[1]);
              break;
            case 3:
              fs->current_dir = change_directory(fs , c.argv[1]);
              if(fs->current_dir != NULL){
                 p.vector[p.count] = fs->current_dir->dir_name;
                 p.count++;
              }
              else
                fs->current_dir = peek(&fs->s);
              break;
            case 4:
             make_directory_in_a_current_directory(fs , c.argv[1]);
              break;
             case 5:
                if(fs->current_dir != fs->root)
                    fs->current_dir = go_back_to_prev(fs);
                if(p.count > 1)
                       p.count--;
              break;
            case 6:
               running = 1;
               break;
            case 7:
               system("clear");
               break;
            case 8:
                print_history(history);
               break;
            default:
                if(strlen(command) == 0 || strcmp(command , " ") == 0);
                else
                 printf("%s: command not found\n", command);
        }
         for(int i = 0; i < c.argc; ++i){
              free(c.argv[i]);
         }
        memset(command , 0 , 67);
    }
     
    // Free the file system resources
    destroy_history(history);
    destroy_FS(fs);
    return 0;
}