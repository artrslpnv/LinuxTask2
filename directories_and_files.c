
#include "inode.h"
#include "filesystem_and_blocks.h"
#include "directories_and_files.h"
inode* inode_entries[INODE_NUM];

inode* Inode(int i){return inode_entries[i];}
typedef enum {
    true=1,
    false=0
} bool;
char* init_dir()
{
    init_superblock();
    memset(inode_entries, 0, sizeof(inode_entries));
    char *init_pwd;
    int result;
    short i;
    int flag=0;
    for (i = 0; i < INODE_NUM; i++) {
        inode* tmp = (inode*)malloc(sizeof(inode));
        memset(tmp, 0, sizeof(inode));
        tmp->inode_id = i;
        result = query_inode(tmp);
        if (result < 0 ){tmp->inode_id = -1;}
        inode_entries[i] = tmp;
        if (tmp->filename != NULL && strcmp(tmp->filename, "/")==0) {flag=1;}}
    if (flag==0) {
        snprintf(inode_entries[0]->filename, 32, "/");
        inode_entries[0]->name_len = 1;
        inode_entries[0]->file_type = 2;
        inode_entries[0]->file_size = 0;
        inode_entries[0]->inode_id = create_inode(inode_entries[0]);
    }
    init_pwd = Inode(0)->filename;
    return init_pwd;
}

int insert_inode_into_table(inode* node)
{
    for (int i = 0; i < INODE_NUM; i++) {
        size_t filename_len = strlen(Inode(i)->filename);
        if ( Inode(i)->inode_id == -1 && filename_len < 1) {
            free(Inode(i));
            inode_entries[i] = node;
            return 0;}
    }
    return -1;
}

int delete_inode_from_table(inode* node)
{
    for (int i = 0; i < INODE_NUM; i++) {
        inode *inode1 = Inode(i);
        if (inode1->inode_id == node->inode_id ) {
            inode1->file_id = 0;
            inode1->inode_id = -1;
            inode1->file_size = 0;
            inode1->group_id = 0;
            inode1->file_type = 0;inode1->name_len = 0;
            sprintf(inode1->filename, " ");
            return 0;}}
    return -1;
}

inode* get_inode_from_path(const char *path)
{
    for (int i = 0; i < INODE_NUM; i++) {
        inode* inode1 = Inode(i);
        if (inode1->inode_id == -1) {continue;}
        if (strcmp(inode1->filename, path) == 0) {
            if(inode1->file_type == 1) {
                return inode1;}
        }
    }
    return NULL;
}

int list_dir(char* ls_list, const char *filename)
{
    char *list[1000];
    int count = 0;
    strcpy(ls_list, "");
    int return_flag = -1;
    for (int i = 0; i < INODE_NUM; i++) {
        inode* node = Inode(i);
        if (strncmp(node->filename, filename, strlen(filename)) == 0) {
            return_flag = 0;
            char buf[100], *value, *name;
            if (strcmp(filename, "/") == 0){value = strdup(node->filename + (strlen(filename)));}
            else {value = strdup(node->filename + (strlen(filename) + 1));}
            name = strsep(&value, "/");
            if (strlen(name) > 0) {
                int flag = 0;
                for (int j = 0; j < count; j++) {
                    if (strcmp(list[j], name) == 0) {flag = 1;
                    break;}}
                if (flag == 1) {continue;}
                list[count] = strdup(name);
                count++;
                if(node->file_type == 1){sprintf(buf, "%s\n", name);}
                if(node->file_type == 2){sprintf(buf, "%s/\n", name);}
                strcat(ls_list, buf);
            }
        }
    }
    return return_flag;
}
int search_dir(const char *pwd, const char *foldername)
{
    int i;
    bool is_folder = false;
    char get_files[500];
    char *pch;
    inode* node[INODE_NUM];
    list_dir(get_files, pwd);
    pch = strstr (get_files, foldername);
    if (pch != NULL) {
        for (i = 0; i < INODE_NUM; i++) {
            node[i] = Inode(i);
            if (strcmp(pwd, node[i]->filename) == 0 && node[i]->file_type == 2) {
                is_folder = true;
            }
        }
    }
    if (is_folder == true) {return 0;} else {return -1;}
}

int create_dir(const char *pwd, const char *foldername)
{
    bool is_created = false;
    bool is_same = false;
    char cfilename[32];
    if (strcmp(pwd, "/") == 0) {snprintf(cfilename, 32, "/%s", foldername);}
    else {snprintf(cfilename, 32, "%s/%s", pwd, foldername);}
    size_t cfilenamelen = strlen(cfilename);
    size_t pwdlen = strlen(pwd);
    size_t foldernamelen = strlen(foldername);
    int totallen = pwdlen + foldernamelen;
    if (totallen > 32) {is_created = false;}
    is_same = search_dir(pwd, foldername);
    if (is_same == 0) {is_created = false;}
    else {
        inode* node[INODE_NUM];
        for (int i = 0; i < INODE_NUM; i++) {
            node[i] = Inode(i);
            size_t filename_len = strlen(node[i]->filename);
            if (filename_len < 1) {
                struct stat st;
                stat(cfilename, &st);
                snprintf(node[i]->filename, 32, "%s", cfilename);
                node[i]->name_len = cfilenamelen;
                node[i]->file_type = 2;
                node[i]->file_size = st.st_size;
                int is_create_inode;
                is_create_inode = create_inode(node[i]);
                if (is_create_inode == 0) {
                    is_created = true;
                    if (is_created == true) {return 0;}
                    else {return -1;}}}}}
    if (is_created == true) {return 0;}
    else {return -1;}
}

int change_dir(const char* destination)
{
    for (int i = 0; i < INODE_NUM; i++) {
        inode* node = Inode(i);
        if (strcmp(node->filename, destination) == 0) {
            if (node->file_type == 2) {return 0;}}}
    return -1;
}

int delete_dir(const char *pwd, const char *foldername)
{
    char str[1000];
    if (strcmp(pwd, "/") == 0){sprintf(str, "/%s", foldername);}
    else{sprintf(str, "%s/%s", pwd, foldername);}
    for (int i = 0; i < INODE_NUM; i++) {
        inode* node = Inode(i);
        if (strcmp(node->filename, str) == 0) {
            if (node->file_type == 2) {
                char list[500];
                list_dir(list, str);
                if (strlen(list) > 0) {return -1;}
                else {
                    delete_inode(node);
                    node->name_len = 0;
                    node->file_type = 0;
                    node->inode_id = -1;
                    sprintf(node->filename, "");
                    return 0;}}}}
    return -1;
}
int read_file_by_path(const char *path, void **buf)
{
    inode *file_inode = get_inode_from_path( path );
    if( file_inode==NULL ) {
        printf("No such file.\n");
        return -1;}
    int file_size = file_inode->file_size + 1;
    void *read_buf = (void*)malloc( file_size );
    int ret = read_file ( file_inode, read_buf );
    if(ret<0) {
        free(read_buf);
        return -2;}
    *buf = read_buf;
    return file_size;
}

int write_file_by_path(const char *path, void *buf, int size)
{
    if( size <= 0 )
        return 0;
    inode *file_inode = get_inode_from_path( path );
    if(file_inode == NULL) {
        printf("No such file.\n");
        return -1;}
    void *write_buf = (void*)malloc( size );
    strncpy( (char*)write_buf, (const char*)buf, size );
    file_inode->file_size = size;
    int ret = write_file ( file_inode, write_buf );
    if(ret<0) {return ret;}
    free( write_buf );
    return 0;
}

/*
 * Delete file
 */
int delete_file_by_path(const char *path)
{
    int i=0;
    if( path[strlen(path)-1] == '/' ) {
        printf("It is a directory.\n");
        return -1;}
    inode *file_inode = get_inode_from_path( path );
    if(file_inode==NULL) {
        printf("No such file.\n");
        return -2;}
    int ret = delete_file ( file_inode );
    if(ret<0) {
        printf("fail:delete_file(...).\n");
        return -4;}
    ret = delete_inode_from_table(file_inode);
    if(ret<0) {
        printf("fail:delete_inode_from_table(...).\n");
        return -3;
    }
    return 0;
}

int search_empty(void)
{
    for(int i=0; i<INODE_SIZE; i++) {
        inode* node = Inode(i);
        if(node->inode_id == -1) {return i;}}
    return -1;
}
int search_file(const char *fpath)
{
    inode *my_inode = get_inode_from_path(fpath);
    if(my_inode == NULL) {
        return -1;}
    else if(my_inode->file_type != 1) {
        return -2;}
    else return 0;
}
int create_file(const char *pwd, const char *fname)
{
    inode* inode_ptr = (inode*)malloc(sizeof(inode));
    int pwdlen = strlen(pwd);
    int fnamelen = strlen(fname);
    int totallen = pwdlen+fnamelen;
    if(totallen>32) return -1;
    char cfilename[32];
    if (strcmp(pwd, "/") == 0) {
        snprintf(cfilename, 32, "/%s", fname);
    } else {snprintf(cfilename, 32, "%s/%s", pwd, fname);}
    if(search_file(cfilename)==0) {return -2;}
    inode_ptr->file_size = 0;
    inode_ptr->file_type = 1; //1=File, 2=Directory
    inode_ptr->name_len = totallen;
    snprintf(inode_ptr->filename, 32, "%s", cfilename);
    int err;
    err = create_inode(inode_ptr);
    if(err == 0) {
        insert_inode_into_table(inode_ptr);
        printf("file create success.\n");
        printf("inode id : %d\n",inode_ptr->inode_id);
        printf("filesize : %d\n",inode_ptr->file_size);
    } else {printf("file create failed.\n");}
    return 0;
}


//
// Created by Артур Слепнев on 3/13/21.
//

