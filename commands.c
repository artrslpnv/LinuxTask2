#include "commands.h"
int Init()
{
    if(load_filesystem("FS")<=-1) {
        int ret=create_filesystem("FS");
        if(ret<0){return ret;}
        strcpy(cur_path,init_dir());
        return 0;}
    strcpy(cur_path,init_dir());
    return 0;
}

void str_process(char *string, char **argv, int *argc)
{
    int count=0;
    count=strlen(string)-1;
    (*argc)=0;
    for(int i=0; i<=count; i++) {
        if(string[i]=='/')
        {string[i]='\0';}}
    if(string[0]!='\0') {
        argv[(*argc)]=&string[0];(*argc)++;}
    else if(string[0]=='\0'&&string[1]!='\0') {
        argv[(*argc)]=&string[1];
        (*argc)++;}
    for(int i=1; i<=count-1; i++) {
        if(string[i]=='\0'&&string[i+1]!='\0') {
            argv[(*argc)]=&string[i+1];
            (*argc)++;}}}
void rel_to_abs(char *relative, char *absolute)
{
    if(relative[0]!='/') {
        char **argv[64];
        int argc=0;
        int k=0;
        strcpy(new_path,cur_path);
        str_process(relative,argv,&argc);
        for(int i=0; i<=argc-1; i++) {
            if(strcmp(argv[i],".")==0)
                continue;
            else if(strcmp(argv[i],"..")==0) {
                for(int k=strlen(new_path)-1; k>=0; k--) {
                    if(new_path[k]=='/'&&k!=0) {
                        new_path[k]='\0';
                        break;}
                    else if(new_path[k]=='/'&&k==0) {
                        new_path[k+1]='\0';
                        break;}}}
            else {
                if(new_path[strlen(new_path)-1]!='/')
                    strcat(new_path,"/");
                strcat(new_path,argv[i]);}}
        strcpy(absolute,new_path);
    } else{strcpy(absolute,relative);}}


void ls(char **argv,
int command_len,
int argc,
char* cur_output,
char* current_dir_list,
char* COMMAND,
char* AbsPATH){
    strcpy(cur_output,".");
    strcat(cur_output,":\n");
    list_dir(current_dir_list,cur_path);
    strcat(cur_output,current_dir_list);
    if(argc==1) {
        printf("%s",cur_output);}
    else if(argc>=2) {
        for(int i=1; i<=argc-1; i++) {
            if(strcmp(argv[i],">")==0&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;}
                else {
                    printf("write file error.\n");
                    break;}}
            else if(strcmp(argv[i],">")==0&&i==argc-1) {
                printf("write file error\n");
                break;}
            else if(strcmp(argv[i],">")!=0&&i==1) {
                rel_to_abs(argv[i],AbsPATH);
                if(change_dir(AbsPATH)<0) {
                    strcpy(cur_output,argv[i]);
                    strcat(cur_output,":\n");
                    strcat(cur_output,"No such file or directory.\n");
                } else {
                    strcpy(cur_output,argv[i]);
                    strcat(cur_output,":\n");
                    list_dir(current_dir_list,AbsPATH);
                    strcat(cur_output,current_dir_list);}}
            else if(strcmp(argv[i],">")!=0) {
                rel_to_abs(argv[i],AbsPATH);
                if(change_dir(AbsPATH)<0) {
                    strcat(cur_output,argv[i]);
                    strcat(cur_output,":\n");
                    strcat(cur_output,"No such file or directory.\n");}
                else {
                    strcat(cur_output,argv[i]);
                    strcat(cur_output,":\n");
                    list_dir(current_dir_list,AbsPATH);
                    strcat(cur_output,current_dir_list);}}
            if(i==argc-1) {
                if(strcmp(cur_output,"")!=0)
                    printf("%s",cur_output);}}}



}

void help(void){
    printf("ls {dir}...\t list files which belong to {dir}.\n");
    printf("cd {dir}   \t change directory to {dir}.\n");
    printf("touch {file}... \t create file named {file} .\n");
    printf("mkdir {dir}... \t to create the directory {dir}.\n");
    printf("cat {file}...  \t read {file}.\n");
    printf("rm {file}... \t remove file {file}.\n");
    printf("rmdir {dir}... \t remove directory {dir}.\n");
    printf("pwd   \t the current path from the root.\n");
    printf("echo {str} > {file}   \t write {str} to the {file} .\n");
    printf("stop   \t to end the program\n");
}

void mkdir1(char **argv,
           int command_len,
           int argc,
           char* cur_output,
           char* current_dir_list,
           char* COMMAND,
           char* AbsPATH){
    if(argc==1){printf("Incorrect command provide name\n");}
    else {for(int i=1; i<=argc-1; i++) {
            if(strcmp(argv[i],">")==0&&i==1&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                strcpy(cur_output,"wrong format\n");
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;}
                else {printf("write file error.\n");
                    break;}}
            else if(strcmp(argv[i],">")==0&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;} else {printf("write file error.\n");break;}}
            else if(strcmp(argv[i],">")==0&&i==argc-1) {
                printf("write file error\n");
                break;}
            else if(strcmp(argv[i],">")!=0) {
                rel_to_abs(argv[i],AbsPATH);
                if(change_dir(AbsPATH)<0) {
                    for(int k=strlen(AbsPATH)-1; k>=0; k--) {
                        if(AbsPATH[k]=='/'&&k!=0) {
                            AbsPATH[k]='\0';
                            printf("[create_dir]pwd:%s fname:%s\n",AbsPATH,&AbsPATH[k+1]);
                            int ret=create_dir(AbsPATH,&AbsPATH[k+1]);
                            printf("[create_dir]ret:%d\n",ret);
                            break;}
                        else if(AbsPATH[k]=='/'&&k==0) {
                            AbsPATH[k]='\0';
                            printf("[create_dir]pwd:%s fname:%s\n","/",&AbsPATH[k+1]);
                            int ret=create_dir("/",&AbsPATH[k+1]);
                            printf("[create_dir]ret:%d\n",ret);
                            break;
                        }}}}
            if(i==argc-1) {
                if(strcmp(cur_output,"")!=0)
                    printf("%s",cur_output);}}}
}

void cd(char **argv,
        int command_len,
        int argc,
        char* cur_output,
        char* current_dir_list,
        char* COMMAND,
        char* AbsPATH){
    if(argc==1){strcpy(cur_path,"/");}
    else {for(int i=1; i<=argc-1; i++) {
            if(strcmp(argv[i],">")==0&&(i+1)<=argc-1) {
                rel_to_abs(argv[i + 1], AbsPATH);
                if (i == 1){strcpy(cur_path,"/");}
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;} else {
                    printf("write file error.\n");
                    break;}}
            else if(strcmp(argv[i],">")==0&&i==argc-1) {
                printf("write file error\n");
                break;
            } else if(strcmp(argv[i],">")!=0&&i==1) {
                rel_to_abs(argv[i],AbsPATH);
                if(change_dir(AbsPATH)<0) {
                    strcpy(cur_output,argv[i]);
                    strcat(cur_output,":\n");
                    strcat(cur_output,"No such file or directory.\n");
                } else {strcpy(cur_path,AbsPATH);}}
            if(i==argc-1) {
                if (strcmp(cur_output, "") != 0)
                {printf("%s",cur_output);}}}}}
void rm(char **argv,
        int command_len,
        int argc,
        char* cur_output,
        char* current_dir_list,
        char* COMMAND,
        char* AbsPATH){
    if (argc == 1){printf("wrong format\n");}
    else {
        for(int i=1; i<=argc-1; i++) {
            if(strcmp(argv[i],">")==0&&i==1&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                strcpy(cur_output,"wrong format\n");
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;}
                else {
                    printf("write file error.\n");
                    break;}
            } else if(strcmp(argv[i],">")==0&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;}
                else {
                    printf("write file error.\n");
                    break;}} else if(strcmp(argv[i],">")==0&&i==argc-1) {
                printf("write file error\n");
                break;
            } else if(strcmp(argv[i],">")!=0) {
                rel_to_abs(argv[i],AbsPATH);
                if(search_file(AbsPATH)==-1) {
                    //if(0){
                    strcat(cur_output,argv[i]);
                    strcat(cur_output,":\n");
                    strcat(cur_output,"No such file or directory.\n");}
                else {
                    printf("[delete_file_by_path]path:%s\n",AbsPATH);
                    int ret=delete_file_by_path(AbsPATH);
                    printf("[delete_file_by_path]ret:%d\n",ret);}}
            if(i==argc-1) {
                if(strcmp(cur_output,"")!=0)
                    printf("%s",cur_output);}}}
}
void rmdir1(char **argv,
            int command_len,
            int argc,
            char* cur_output,
            char* current_dir_list,
            char* COMMAND,
            char* AbsPATH){
    if(argc==1){printf("wrong format\n");}
    else {
        for(int i=1; i<=argc-1; i++) {
            if(strcmp(argv[i],">")==0&&i==1&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                strcpy(cur_output,"wrong format\n");
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;}
                else {
                    printf("write file error.\n");
                    break;}
            } else if(strcmp(argv[i],">")==0&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;}
                else {
                    printf("write file error.\n");
                    break;}}
            else if(strcmp(argv[i],">")==0&&i==argc-1) {
                printf("write file error\n");
                break;}
            else if(strcmp(argv[i],">")!=0) {
                rel_to_abs(argv[i],AbsPATH);
                if(change_dir(AbsPATH)<0) {
                    strcat(cur_output,argv[i]);
                    strcat(cur_output,":\n");
                    strcat(cur_output,"No such file or directory.\n");}
                else {
                    for(int k=strlen(AbsPATH)-1; k>=0; k--) {
                        if(AbsPATH[k]=='/'&&k!=0) {
                            AbsPATH[k]='\0';
                            printf("[delete_dir]pwd:%s foldername=%s\n",AbsPATH,&AbsPATH[k+1]);
                            int ret=delete_dir(AbsPATH,&AbsPATH[k+1]);
                            printf("[delete_dir]ret:%d\n",ret);
                            break;}
                        else if(AbsPATH[k]=='/'&&k==0) {
                            AbsPATH[k]='\0';
                            printf("[delete_dir]pwd:%s foldername=%s\n","/",&AbsPATH[k+1]);
                            int ret=delete_dir("/",&AbsPATH[k+1]);
                            printf("[delete_dir]ret:%d\n",ret);
                            break;}}}}
            if(i==argc-1) {
                if(strcmp(cur_output,"")!=0){printf("%s",cur_output);}}}}}

void echo(char **argv,
          int command_len,
          int argc,
          char* cur_output,
          char* current_dir_list,
          char* COMMAND,
          char* AbsPATH){
    if(argc==1){printf("wrong format\n");}
    else {
        for(int i=1; i<=argc-1; i++) {
            if(strcmp(argv[i],">")==0&&i==1&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                strcpy(cur_output,"wrong format\n");
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;}
                else {printf("write file error.\n");
                    break;
                }
            } else if(strcmp(argv[i],">")==0&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;}
                else {
                    printf("write file error.\n");
                    break;}
            } else if(strcmp(argv[i],">")==0&&i==argc-1) {
                printf("write file error\n");
                break;}
            else if(strcmp(argv[i],">")!=0) {
                if(argv[i][0]=='\"'&&argv[i][strlen(argv[i])-1]=='\"') {
                    argv[i][strlen(argv[i])-1]='\0';
                    argv[i]=&argv[i][1];}
                if(i!=1){strcat(cur_output," ");}
                strcat(cur_output,argv[i]);
            }
            if(i==argc-1) {
                if(strcmp(cur_output,"")!=0){printf("%s\n",cur_output);}}
        }}}
void cat(char **argv,
         int command_len,
         int argc,
         char* cur_output,
         char* current_dir_list,
         char* COMMAND,
         char* AbsPATH){
    if(argc==1){printf("wrong format\n");}
    else {
        for(int i=1; i<=argc-1; i++) {
            if(strcmp(argv[i],">")==0&&i==1&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                strcpy(cur_output,"wrong format\n");
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;}
                else {
                    printf("write file error.\n");
                    break;}
            } else if(strcmp(argv[i],">")==0&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;}
                else {
                    printf("write file error.\n");
                    break;}
            } else if(strcmp(argv[i],">")==0&&i==argc-1) {
                printf("write file error\n");
                break;}
            else if(strcmp(argv[i],">")!=0) {
                rel_to_abs(argv[i],AbsPATH);
                if(search_file(AbsPATH)==-1) {
                    strcat(cur_output,argv[i]);
                    strcat(cur_output,":\n");
                    strcat(cur_output,"No such file or directory.\n");}
                else {
                    void *buf=NULL;
                    printf("[read_file_by_path]path:%s buf=%s\n",AbsPATH,buf);
                    int ret=read_file_by_path(AbsPATH,&buf);
                    printf("[read_file_by_path]ret:%d buf=%s\n",ret,(char*)buf);
                    strcat(cur_output,argv[i]);
                    strcat(cur_output,":\n");
                    strcat(cur_output,(char*)buf);}}if(i==argc-1) {
                if (strcmp(cur_output, "") != 0){printf("%s\n",cur_output);}}}}
}

void touch(char **argv,
           int command_len,
           int argc,
           char* cur_output,
           char* current_dir_list,
           char* COMMAND,
           char* AbsPATH){
    if(argc==1){printf("wrong format\n");}
    else {
        for(int i=1; i<=argc-1; i++) {
            if(strcmp(argv[i],">")==0&&i==1&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                strcpy(cur_output,"wrong format\n");
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;}
                else {
                    printf("write file error.\n");
                    break;}}
            else if(strcmp(argv[i],">")==0&&(i+1)<=argc-1) {
                rel_to_abs(argv[i+1],AbsPATH);
                printf("[write_file_by_path]path:%s buf=%s size=%d\n",AbsPATH,cur_output,strlen(cur_output));
                if(write_file_by_path(AbsPATH,cur_output,strlen(cur_output))>=0) {
                    printf("%s finish\n",cur_output);
                    break;}
                else {printf("write file error.\n");
                    break;}}
            else if(strcmp(argv[i],">")==0&&i==argc-1) {
                printf("write file error\n");
                break;
            } else if(strcmp(argv[i],">")!=0) {
                rel_to_abs(argv[i],AbsPATH);
                if(search_file(AbsPATH)==-1) { //找不到就直接創建
                    for(int k=strlen(AbsPATH)-1; k>=0; k--) {
                        if(AbsPATH[k]=='/'&&k!=0) {
                            AbsPATH[k]='\0';
                            printf("[create_file]pwd:%s fname:%s\n",AbsPATH,&AbsPATH[k+1]);
                            int ret=create_file(AbsPATH,&AbsPATH[k+1]);
                            printf("[create_file]ret:%d\n",ret);
                            break;}
                        else if(AbsPATH[k]=='/'&&k==0) {
                            AbsPATH[k]='\0';
                            printf("[create_file]pwd:%s fname:%s\n","/",&AbsPATH[k+1]);
                            int ret=create_file("/",&AbsPATH[k+1]);
                            printf("[create_file]ret:%d\n",ret);
                            break;}}}}
            if(i==argc-1) {
                if(strcmp(cur_output,"")!=0){printf("%s",cur_output);}}}}}