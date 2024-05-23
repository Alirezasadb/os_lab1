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
    // TODO: implement
}
