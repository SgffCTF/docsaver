#ifndef FILE_H
#define FILE_H

#include <dirent.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *path;
} fileinfo_t;

// getFiles put pathes into files and return count of files
int32_t getFiles(const char* dirPath, fileinfo_t** files) {
    DIR* dir = opendir(dirPath);
    if (dir == NULL) {
        return -1;
    }

    int32_t count = 0;
    fileinfo_t* fileList = NULL;

    struct dirent* entry = readdir(dir);
    while ((entry != NULL)) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            entry = readdir(dir);
            continue;
        };

        count++;
        fileList = (fileinfo_t*)realloc(fileList, count * sizeof(fileinfo_t));
        if (fileList == NULL) {
            closedir(dir);
            return -1;
        }

        fileList[count - 1].path = (char*)malloc(strlen(dirPath) + strlen(entry->d_name) + 1);
        if (fileList[count - 1].path == NULL) {
            closedir(dir);
            for(int i = 0; i < count - 1; i++){
                free(fileList[i].path);
            }
            free(fileList);
            return -1;
        }
        sprintf(fileList[count - 1].path, "%s%s", dirPath, entry->d_name);

        entry = readdir(dir);
    }

    closedir(dir);
    *files = fileList;

    return count;
}

#endif