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



int compare_jpg_files(const char* file1_path, const char* file2_path) {
     FILE *fp1 = fopen(file1_path, "rb");
    FILE *fp2 = fopen(file2_path, "rb");

    if (fp1 == NULL || fp2 == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    char buffer1[1024];
    char buffer2[1024];

    int same = 1;

    while (1) {
        size_t bytes_read1 = fread(buffer1, 1, 1024, fp1);
        size_t bytes_read2 = fread(buffer2, 1, 1024, fp2);

        if (bytes_read1 != bytes_read2) {
            same = 0;
            break;
        }

        if (bytes_read1 == 0) {
            break;
        }

        for (int i = 0; i < bytes_read1; i++) {
            if (buffer1[i] != buffer2[i]) {
                same = 0;
                break;
            }
        }

        if (!same) {
            break;
        }
    }

    fclose(fp1);
    fclose(fp2);

    if (same) {
        return 1;
    } else {
        return 0;
    }


}


int arePdfFilesIdentical(const char* file1, const char* file2) {
    FILE* f1 = fopen(file1, "rb");
    FILE* f2 = fopen(file2, "rb");

    if (!f1 || !f2) {
        return -1; // file not found
    }

    fseek(f1, 0, SEEK_END);
    fseek(f2, 0, SEEK_END);

    long size1 = ftell(f1);
    long size2 = ftell(f2);

    if (size1 != size2) {
        return 0; // different file sizes
    }

    fseek(f1, 0, SEEK_SET);
    fseek(f2, 0, SEEK_SET);

    char buffer1[size1];
    char buffer2[size2];

    fread(buffer1, size1, 1, f1);
    fread(buffer2, size2, 1, f2);

    int areIdentical = 1;
    for (int i = 0; i < size1; i++) {
        if (buffer1[i] != buffer2[i]) {
            areIdentical = 0;
            break;
        }
    }

    fclose(f1);
    fclose(f2);

    return areIdentical;
}


// Function to check if two files are identical
int are_files_identical(const char *file1, const char *file2) {
    FILE *fp1 = fopen(file1, "rb");
    FILE *fp2 = fopen(file2, "rb");
    /*if (!fp1 ||!fp2) return 0;
    char buf1[1024], buf2[1024];
    while (fread(buf1, 1, 1024, fp1) == 1024 && fread(buf2, 1, 1024, fp2) == 1024) {

if (memcmp(buf1, buf2, 1024)!= 0) break;
    }
    fclose(fp1);
    fclose(fp2);
    return feof(fp1) && feof(fp2);*/
    int ch1 ,ch2;
    do{
    ch1=fgetc(fp1);
    ch2=fgetc(fp2);
    if(ch1 != ch2){
        fclose(fp1);
        fclose(fp2);
        return 0;
    }
    }while(ch1 != EOF && ch2 != EOF);
    fclose(fp1);
    fclose(fp2);
    if(ch1 == EOF && ch2== EOF){
        return 1;
    }else{
        return 0;
    }
}

// Thread function to check a file
void *check_file(void *arg) {
    file_info_t *file = (file_info_t *)arg;
    sem_wait(&sem);
    //printf("%s\n",file->type);
    //char *txt=file->path;
    number_of_files++;
    //printf("%s\n",file_list[0].path);
    // Check if file is duplicate
        if(file->type[0]=='t'){
            number_txt++;
            num_txt_array++;
            if(num_txt_array==1){
                file_list_txt[0]= *file;
            }
            for(int i=0;i<num_txt_array-1;i++){
                if (are_files_identical(file->path, file_list_txt[i].path)) {
                    num_txt_array--;
                    remove(file->path);
                    duplicate_file_t duplicate;
                    strcpy(duplicate.path, file->path);
                    strcpy(duplicate.type, file->type);
                    duplicate_list[num_deleted_files++] = duplicate;
                    break;
                }else{
                    file_list_txt[num_txt_array-1]= *file;
                }
            }
        }else if(file->type[0]=='j'){
            number_jpg++;
            num_jpg_array++;
            if(num_jpg_array==1){
                file_list_jpg[0]= *file;
            }
            for(int i=0;i<num_jpg_array-1;i++){
                if (compare_jpg_files(file->path, file_list_jpg[i].path)) {
                    num_jpg_array--;
                    remove(file->path);
                    duplicate_file_t duplicate;
                    strcpy(duplicate.path, file->path);
                    strcpy(duplicate.type, file->type);
                    duplicate_list[num_deleted_files++] = duplicate;
                    break;
                }else{
                    file_list_txt[num_jpg_array-1]= *file;
                }
            }

        }else if(file->type[0]=='p'){
            number_pdf++;
            num_pdf_array++;
            if(num_pdf_array==1){
                file_list_pdf[0]= *file;
            }
            for(int i=0;i<num_pdf_array-1;i++){
                if (arePdfFilesIdentical(file->path, file_list_pdf[i].path)) {
                    num_pdf_array--;
                    remove(file->path);
                    duplicate_file_t duplicate;
                    strcpy(duplicate.path, file->path);
                    strcpy(duplicate.type, file->type);
                    duplicate_list[num_deleted_files++] = duplicate;
                    break;
                }else{
                    file_list_pdf[num_pdf_array-1]= *file;
                }
            }
        }
    sem_post(&sem);
    return NULL;
}


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
    long long  total_size1 = total_size(root_path);
    // Initialize semaphore
    sem_init(&sem, 0, 1);
    create_process(root_path);
    traverse_directory(root_path);
    printf("Total number of files: %d\n", number_of_files);
    printf("Number of each files:\n");
    printf("- txt %d\n",number_txt);
    printf("- jpg %d\n",number_jpg);
    printf("- pdf %d\n",number_pdf);
    printf("duplicate file found and remove: %d\n", num_deleted_files);
    for(int j = 0 ;j<num_deleted_files;j++){
        printf("- %s\n",duplicate_list[j].path);
    }
    long long  total_size2 = total_size(root_path);
    printf("Path size before removing: %lld bytes\n",total_size1);
    printf("Path size after removing: %lld bytes\n",total_size2);
}
