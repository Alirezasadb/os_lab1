#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <dirent.h>

// Structure to hold file information
typedef struct {
    char path[256];
    char type[16];
    char name[16];
    int size;
} file_info_t;

// Structure to hold duplicate file information
typedef struct {
    char path[256];
    char type[16];
} duplicate_file_t;

// Global variables
sem_t sem; // semaphore for synchronization
int k=0;
int t=1;
int number_of_files=1;
int num_checked_files = 1;
int num_deleted_files = 0;
int number_of_txt_files = 0;
file_info_t file_list[10]; // list of all checked files
duplicate_file_t duplicate_list[10]; // list of duplicate files

void create_process(const char *path) {
    pid_t pid = vfork();
    if (pid == 0) { // child process
        // Create a thread for each file in the folder
        DIR *dir;
        struct dirent *ent;
        dir = opendir(path);
        while ((ent = readdir(dir))!= NULL) {
            if (ent->d_type == DT_REG) { // regular file
                file_info_t file;
                sprintf(file.path, "%s/%s", path, ent->d_name);
                sprintf(file.name, "%s", ent->d_name);
                char *result1=strstr(file.path,"txt");
                char *result2=strstr(file.path,"jpg");
                char *result3=strstr(file.path,"pdf");
               if(result1!=NULL){
                        sprintf(file.type, "txt");
                    }else if(result2!=NULL){
                        sprintf(file.type, "jpg");
                    }else{
                        sprintf(file.type, "pdf");
                }
                pthread_t thread;
                pthread_create(&thread, NULL, check_file, &file);
                pthread_join(thread, NULL);
            }
        }
        closedir(dir);
    } else { // parent process
        waitpid(pid, NULL, 0);
    }
}

void traverse_directory(const char *root_path){
    DIR *dir;
    struct dirent *ent;
    dir = opendir(root_path);
    while ((ent = readdir(dir))!= NULL) {
        if (ent->d_type == DT_DIR && strcmp(ent->d_name, ".")!= 0 && strcmp(ent->d_name, "..")!= 0) {
            char subfolder_path[256];
            sprintf(subfolder_path, "%s/%s", root_path, ent->d_name);
            create_process(subfolder_path);
            traverse_directory(subfolder_path);
        }
    }
    closedir(dir);
}

int main() {
    const char *root_path = "/home/alireza/Desktop/test_project";
    // Initialize semaphore
    sem_init(&sem, 0, 1);
    create_process(root_path);
    traverse_directory(root_path);
    printf("Total number of files: %d\n", number_of_files-1);
    printf("Number of txt files:%d\n", number_of_txt_files);
    printf("duplicate file found and remove: %d\n", num_deleted_files);
    for(int j = 0 ;j<num_deleted_files;j++){
        printf("- %s\n",duplicate_list[j].path);
    }
    
}
