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
