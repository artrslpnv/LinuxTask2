
#ifndef FS_COMMANDS_H
#define FS_COMMANDS_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include "directories_and_files.h"
#include "filesystem_and_blocks.h"
#include "inode.h"

char cur_path[100];
char new_path[100];
int Init();
void str_process(char *string, char **argv, int *argc);
void rel_to_abs(char *relative, char *absolute);
void help(void);
void ls(char **argv,
        int command_len,
        int argc,
        char* cur_output,
        char* current_dir_list,
        char* COMMAND,
        char* AbsPATH);
void mkdir1(char **argv,
           int command_len,
           int argc,
           char* cur_output,
           char* current_dir_list,
           char* COMMAND,
           char* AbsPATH);
void cd(char **argv,
int command_len,
int argc,
char* cur_output,
char* current_dir_list,
char* COMMAND,
char* AbsPATH);
void rm(char **argv,
int command_len,
int argc,
char* cur_output,
char* current_dir_list,
char* COMMAND,
char* AbsPATH);
void rmdir1(char **argv,
int command_len,
int argc,
char* cur_output,
char* current_dir_list,
char* COMMAND,
char* AbsPATH);
void echo(char **argv,
int command_len,
int argc,
char* cur_output,
char* current_dir_list,
char* COMMAND,
char* AbsPATH);
void cat(char **argv,
         int command_len,
         int argc,
         char* cur_output,
         char* current_dir_list,
         char* COMMAND,
         char* AbsPATH);
void touch(char **argv,
int command_len,
int argc,
char* cur_output,
char* current_dir_list,
char* COMMAND,
char* AbsPATH);


#endif //FS_COMMANDS_H
