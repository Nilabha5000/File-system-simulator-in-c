#ifndef HST_MNG_H


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

mng_history *init_history();
void add_history(mng_history * , char *);
void print_history(mng_history *);

void destroy_history(mng_history *);
#endif