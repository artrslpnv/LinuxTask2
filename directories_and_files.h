//
// Created by Артур Слепнев on 3/15/21.
//

#ifndef FS_DIRECTORIES_AND_FILES_H
#define FS_DIRECTORIES_AND_FILES_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


int read_file_by_path(const char *path, void **buf);

int _read_file_by_path(const char *path, void *buf, int size);

int write_file_by_path(const char *path, void *buf, int size);

int delete_file_by_path(const char *path);

int create_file(const char *pwd, const char *fname);

int search_file(const char *fpath);

int search_empty(void);

#endif
