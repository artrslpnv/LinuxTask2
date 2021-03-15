#ifndef INODE_H
#define INODE_H

#include "filesystem_and_blocks.h"

#define NUMBER_OF_BLOCKS_AND_INODES 4096
#define INODE_SIZE  256
#define BYTE_SIZE 8
#define INODE_SIZE 128
#define INODE_NUM 4096
#define SUPERBLOCK_ID 1
#define INDIRECT_BLOCK_GROUP_SIZE 12
typedef struct inode {
    int inode_id;
    int file_size;
    int file_type;
    int name_len;
    int file_id;
    char filename[32];
    int group_id;
    int num[15];
    char to_align[12]
} inode;


typedef struct superblock {
    short block_used;
    short block_unuse;
    short inode_used;
    short inode_unuse;
    uint8_t inode_map[NUMBER_OF_BLOCKS_AND_INODES];
} superblock;

typedef struct single_indirect_block {
    short num12[SIZE_OF_BLOCK/8];
} single_indirect_block;

typedef struct inode_group{
    struct inode inode_array[SIZE_OF_BLOCK/INODE_SIZE];
} inode_group;

superblock superblock_inode;

int init_superblock();

int init_inode(inode *inode, size_t length);

int set_inode_bitmap (int id);

int clear_inode_bitmap (int id);

int query_inode_bitmap (int id);

int dump_inode_bitmap ();

int dump_inode (inode *inode);

int dump_block (int block_id, void *address, size_t len);

int query_inode (inode *inode);

int update_inode (inode *inode);

int create_inode (inode *inode);

int insert_inode_into_table(inode* node);
int delete_inode_from_table(inode* node);
inode* get_inode_from_path(const char *path);

int delete_inode (inode *inode);

int read_file (inode *inode, void* file);

int delete_file (inode *inode);

int write_file (inode *inode, void* file);

int bitmap_set_inode(int id);

int bitmap_clear_inode (int id);

int bitmap_inode_query (int id);

char* init_dir();

int list_dir(char* ls_list, const char *filename);

int search_dir(const char *pwd, const char *foldername);

int create_dir(const char *pwd, const char *foldername);

int rename_dir(const char *pwd, const char *foldername, const char *newname);

int change_dir(const char* destination);

int delete_dir(const char *pwd, const char *foldername);

#endif //FS_INODE_H
