#include <dirent.h> 
#include <sys/types.h> 
#include <sys/param.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <locale.h>

int compare(const void *a, const void *b) {
    return strcoll(*(const char **)a, *(const char **)b);
}

int findAndSort(char* path, char* options){
    DIR *d = opendir(path);
    struct dirent *dir;
    struct stat fileStat;
    char fullpath[MAXPATHLEN];
    int totalFiles = 0;
    char **files = NULL;
    while ((dir = readdir(d))){
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") ==0){
            continue;
        }
        snprintf(fullpath, MAXPATHLEN, "%s/%s", path, dir->d_name);
        if (stat(fullpath, &fileStat) == -1) {
            perror("stat");
            continue;
        } 
        int match = 0;
        if(strchr(options, 'l') != NULL && S_ISLNK(fileStat.st_mode)) {
            match = 1;
        } else if(strchr(options, 'd') != NULL && S_ISDIR(fileStat.st_mode)) {
            match = 1;
        } else if(strchr(options, 'f') != NULL && S_ISREG(fileStat.st_mode)) {
            match = 1;
        }
        if (S_ISDIR(fileStat.st_mode)) {
            totalFiles += findAndSort(fullpath, options);
        }
        if(match){
            files = realloc(files, (totalFiles + 1) * sizeof(char*));
            files[totalFiles] = strdup(dir->d_name);
            totalFiles++;
        }
        if(options[0] == '\0') {
            files = realloc(files, (totalFiles + 1) * sizeof(char*));
            files[totalFiles] = strdup(dir->d_name);
            totalFiles++;
        }
    }
    closedir(d);
    if (strchr(options, 's') != NULL) {
        setlocale(LC_COLLATE, "");
        qsort(files, totalFiles, sizeof(char*), compare);
    }
    for (int i = 0; i < totalFiles; i++) {
        printf("%s\n", files[i]);
        free(files[i]);
    }
    free(files);
    return 0;
}

char* clearOprions(char* options) {
    int i = 0;
    char* clearedOptions = NULL;
    int totalOptions = 0;
    while (options[i] != '\0') {
        if (options[i] == 'l' || options[i] == 'd' || options[i] == 'f' || options[i] == 's') {
            clearedOptions = realloc(clearedOptions, (totalOptions + 2) * sizeof(char));
            clearedOptions[totalOptions] = options[i];
            totalOptions++;
        }
        i++;
    }
    clearedOptions[totalOptions] = '\0';
    return clearedOptions;
}

int main(int argc, char *argv[]) {
    if (argc < 1) {
        fprintf(stderr, "Not enough arguments passed\n");
        return 1;
    }

    char *path = NULL;
    char *options = malloc(1 * sizeof(char));
    options[0] = '\0';
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            int totalOptions = strlen(argv[i] + 1);
            options = realloc(options, (totalOptions + 1) * sizeof(char));
            strcat(options, argv[i] + 1);
        } else {
            path = argv[i];
        }
    }
    if(options[0] != '\0') {
        options = clearOprions(options);
    }
    if (path == NULL) {
        path = "./";
    } 
    printf("Path: %s\n", path);
    printf("Options: -%s\n", options);
    findAndSort(path, options);
    free(options);
    return 0;
}