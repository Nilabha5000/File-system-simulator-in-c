#include "FS.h"
#include <unistd.h>
#include <string.h>
void options(char *dir_name){
    printf("Current directory %s \n\n",dir_name);
    printf("1. View contents \n");
    printf("2. Create directory \n");
    printf("3. Create file \n");
    printf("4. Change directory \n");
    printf("5. Go back\n");
    printf("6. Close\n");
}
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int main(void) {
    int running = 0;
    int opt;
    char filename[20], dir_name[20];
    struct FS *fs = initFS("root");    

    while (!running) {
        system("clear");
        options(fs->current_dir->dir_name);

        if (scanf("%d", &opt) != 1) {
            printf("Invalid input! Please enter a number.\n");
            clear_input_buffer();
            sleep(1);
            continue;
        }
        clear_input_buffer();

        switch (opt) {
            case 1:
                view_contents(fs);
                break;

            case 2:
                view_contents(fs);
                printf("\nEnter the directory name: ");
                fgets(dir_name, sizeof(dir_name), stdin);
                dir_name[strcspn(dir_name, "\n")] = 0; // Remove newline
                make_directory_in_a_current_directory(fs, dir_name);
                break;

            case 3:
                view_contents(fs);
                printf("\nEnter the file name: ");
                fgets(filename, sizeof(filename), stdin);
                filename[strcspn(filename, "\n")] = 0; // Remove newline
                create_file(fs, filename);
                break;
            case 4:
                view_contents(fs);
                printf("\nEnter the directory name to change to: ");
                fgets(dir_name, sizeof(dir_name), stdin);
                dir_name[strcspn(dir_name, "\n")] = 0; // Remove newline
                fs->current_dir = change_directory(fs, dir_name);
                if (fs->current_dir == NULL) {
                    printf("Failed to change directory.\n");
                }
                break;  
            case 5:
                if (fs->current_dir != fs->root) {
                    fs->current_dir = go_back_to_prev(fs);
                } else {
                    printf("Cannot go back from root.\n");
                }
                break;

            case 6:
                running = 1;
                break;

            default:
                printf("Invalid option. Try again.\n");
        }

        if (opt != 5) {
            printf("\nPress Enter to continue...");
            getchar();
        }
    }

    destroy_FS(fs);
    return 0;
}
