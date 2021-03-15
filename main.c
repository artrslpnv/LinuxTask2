#include "commands.h"

typedef enum {
    true =1,
    false=0
} bool;
int main()
{
    char *argv[64];
    int command_len=0;
    int argc=0;
    char cur_output[100000];
    char current_dir_list[1000];
    char COMMAND[100];
    char AbsPATH[100];
    if(Init()<0) {printf("Error\n");return 0;}
    while(true) {
        printf("print help to get a list of commands\n");
        fgets(COMMAND,sizeof(COMMAND),stdin);
        argc=0;
        strcpy(cur_output,"");
        command_len=strlen(COMMAND)-1;
        if(strcmp(COMMAND,"stop\n")==0||strcmp(COMMAND,"q\n")==0){break;}
        for(int i=0; i<=command_len; i++) {
            if(COMMAND[i]==' '||COMMAND[i]=='\n')
            {COMMAND[i]='\0';}}
        argv[argc]=&COMMAND[0];
        argc++;
        for(int i=1; i<=command_len-1; i++) {
            if(COMMAND[i]=='\0'&&COMMAND[i+1]!='\0') {
                argv[argc]=&COMMAND[i+1];
                argc++;}}
        if(strcmp(argv[0],"help")==0) {help();}
        if(strcmp(argv[0],"ls")==0) {ls(argv,command_len,argc,cur_output,current_dir_list,COMMAND,AbsPATH);}
        else if(strcmp(argv[0],"pwd")==0) {printf("%s\n",cur_path);}
        else if(strcmp(argv[0],"cat")==0) {cat(argv,command_len,argc,cur_output,current_dir_list,COMMAND,AbsPATH);}
        else if(strcmp(argv[0],"rm")==0) {rm(argv,command_len,argc,cur_output,current_dir_list,COMMAND,AbsPATH);}
        else if(strcmp(argv[0],"rmdir")==0) {rmdir1(argv,command_len,argc,cur_output,current_dir_list,COMMAND,AbsPATH);}
        else if(strcmp(argv[0],"touch")==0) {touch(argv,command_len,argc,cur_output,current_dir_list,COMMAND,AbsPATH);}
        else if(strcmp(argv[0],"echo")==0) {echo(argv,command_len,argc,cur_output,current_dir_list,COMMAND,AbsPATH);}
        else if(strcmp(argv[0],"cd")==0) {cd(argv,command_len,argc,cur_output,current_dir_list,COMMAND,AbsPATH);}
        else if(strcmp(argv[0],"mkdir")==0) {mkdir1(argv,command_len,argc,cur_output,current_dir_list,COMMAND,AbsPATH);}
        else{printf("See help for avilaible options\n");}}
    return 0;
}