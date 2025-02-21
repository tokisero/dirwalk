#include <dirent.h> 
#include <sys/types.h> 
#include <sys/param.h> 
#include <sys/stat.h> 
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <string.h> 
#include <locale.h>



int printFiles(char** files) {
    int size = 0;
    while (files[size] != NULL) size++;
    for (int i = 0; i < size; i++) {
        printf("%s\n", files[i]);
        free(files[i]);
    } 
    free(files);
    return 0;
}

int compare(const void *a, const void *b) {  
    const char* aa = *(const char**)a;
    const char* bb = *(const char**)b;
    return strcoll(aa, bb);
}

char* strdup(const char* str) {
    size_t size = strlen(str) + 1;
    char* lnk = malloc(size);
    if (lnk != NULL){
        strcpy(lnk, str);
    }
    return lnk;
}

char** uniteArrays(char** files, char** newFiles) {
    if (files == NULL) {
        return newFiles;
    }
    int size1 = 0;
    while (files[size1] != NULL) size1++;
    int size2 = 0;
    while (newFiles[size2] != NULL) size2++;
    char** res = (char**)malloc((size1 + size2 + 1) * sizeof(char*));
    for (int i = 0; i < size1; i++) res[i] = strdup(files[i]);
    for (int i = 0; i < size2; i++) res[size1 + i] = strdup(newFiles[i]);
    res[size1 + size2] = NULL;
    for (int i = 0; i < size1; i++) free(files[i]);
    free(files);
    for (int i = 0; i < size2; i++) free(newFiles[i]);
    free(newFiles);
    return res;
}

char** findAndSort(char* path, char* options){
    DIR *d = opendir(path);
    if (d == NULL) {
        return NULL;
    }
    struct dirent *dir;
    struct stat fileStat;
    char fullpath[MAXPATHLEN];
    char **files = NULL;
    while ((dir = readdir(d)) != NULL){
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") ==0){
            continue;
        }
        snprintf(fullpath, MAXPATHLEN, "%s/%s", path, dir->d_name);
        if (stat(fullpath, &fileStat) == -1) {
            continue;
        } 
        int match = 0;
        if(strchr(options, 'l') != NULL && S_ISLNK(fileStat.st_mode)) {
            match = 1;
        } else if(strchr(options, 'd') != NULL && S_ISDIR(fileStat.st_mode)) {
            match = 1;
        } else if(strchr(options, 'f') != NULL && S_ISREG(fileStat.st_mode)) {
            match = 1;
        } else if(options[0] == 's' && options[1] == '\0'){
            match = 1;
        }
        if (match == 1 || options[0] == '\0')  {
            if (files == NULL){
                files = malloc(2*sizeof(char*));
                files[0] = strdup(fullpath);
                files[1] = NULL;
            }else {
                int size = 0;
                while (files[size] != NULL) size++;
                files = realloc(files, (size + 2) * sizeof(char*));
                files[size] = strdup(fullpath);
                files[size + 1] = NULL;
            }
        } 
        if (S_ISDIR(fileStat.st_mode)) {
            char **newFiles = findAndSort(fullpath, options);
            if (newFiles != NULL) {
                files = uniteArrays(files, newFiles);
            }
        }
    }
    closedir(d);
    return files;
}

char* clearOprions(char* options) {
    int i = 0;
    char* clearedOptions = malloc(sizeof(char));
    int totalOptions = 0;
    int l = 0;
    int d = 0;
    int f = 0;
    int s = 0;
    while (options[i] != '\0') {
        if (options[i] == 'l' && l == 0){
            l = 1;
            clearedOptions = realloc(clearedOptions, (totalOptions + 2) * sizeof(char));
            clearedOptions[totalOptions] = options[i];
            totalOptions++;
            continue;
        } 
        if (options[i] == 'd' && d == 0){
            d = 1;
            clearedOptions = realloc(clearedOptions, (totalOptions + 2) * sizeof(char));
            clearedOptions[totalOptions] = options[i];
            totalOptions++;
            continue;
        } 
        if (options[i] == 'f' && f == 0){
            f = 1;
            clearedOptions = realloc(clearedOptions, (totalOptions + 2) * sizeof(char));
            clearedOptions[totalOptions] = options[i];
            totalOptions++;
            continue;
        } 
        if (options[i] == 's' && s == 0){
            s = 1;
            clearedOptions = realloc(clearedOptions, (totalOptions + 2) * sizeof(char));
            clearedOptions[totalOptions] = options[i];
            totalOptions++;
            continue;
        }
        i++;
    }

    clearedOptions[totalOptions] = '\0';
    return clearedOptions;
}

int main(int argc, char *argv[]) {
    setlocale(LC_COLLATE, "ru_RU.UTF-8");
    if (argc < 1) {
        fprintf(stderr, "Not enough arguments passed\n");
        return 1;
    }
    char *path = NULL;
    char *options = malloc(sizeof(char));
    options[0] = '\0';
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            int len = strlen(argv[i] + 1);
            options = realloc(options, (strlen(options) + len + 1) * sizeof(char));
            if(options == NULL) return 1;
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
    char** files = findAndSort(path, options);
    int size = 0;
    while (files[size] != NULL) size++;
    if (strchr(options, 's') != NULL && size > 1) {
        setlocale(LC_COLLATE, "");
        qsort(files, size, sizeof(char*), compare);
    }
    printFiles(files);
    free(options);
    return 0;
}