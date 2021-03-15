
#define BYTE_SIZE 8
#define SIZE_OF_BLOCK 2048
#define BLOCKS_NUMBER 4096
#define TOTAL_NUMBER_OF_BYTES SIZE_OF_BLOCK * BLOCKS_NUMBER
#define SIZE_OF_SUPER_BLOCK 1024 - 10 - BLOCKS_NUMBER/BYTE_SIZE
#define blocks_from 514

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


int file_state = -1;//only open or load file system once*/
static int num_blocks = 0;
uint8_t block_map[BLOCKS_NUMBER / BYTE_SIZE] = {0};
char file_system_path[100] = "\0";
static int number_of_blocks = 0;
#define HEADER "filesystem\0"

typedef enum bool {
    true=1,
    false=0
} bool;

void map_set(int id){block_map[id >> 3] |= (unsigned long long) 1 << (id % BYTE_SIZE);}

void map_clear(int id){block_map[id>>3] &= ~((unsigned long long) 1 << (id % BYTE_SIZE));}

char map_check(int id)
{
    uint8_t map[BLOCKS_NUMBER/BYTE_SIZE] = {0};
    if(lseek(file_state, strlen(HEADER), SEEK_SET) < 0) {return false;}
    if(read(file_state, map, BLOCKS_NUMBER/BYTE_SIZE) != BLOCKS_NUMBER/BYTE_SIZE) {return true;}
    return (bool) (map[id>>3] & (1ULL << id%BYTE_SIZE)) >> id%BYTE_SIZE;}
    
int update_super_block(int fd)
{
    if(lseek(fd, strlen(HEADER), SEEK_SET) < 0) {return 0;}
    if(write(fd, block_map, BLOCKS_NUMBER/BYTE_SIZE) != BLOCKS_NUMBER/BYTE_SIZE) {return 0;}
    return 1;}

int release_block(int id)
{
    map_clear(id);
    if(!update_super_block(file_state)) {return 0;} 
    else {return 1;}
}

int delete_block(const int block_id)
{
    if (file_state > 0) {
        if ((file_state = open(file_system_path, O_RDWR)) < 0) { return -1; }
        if (block_id >= SIZE_OF_BLOCK || block_id <= 0 || map_check(block_id) == false) { return -2; }
        if (lseek(file_state, block_id * SIZE_OF_BLOCK, SEEK_SET) < 0) { return -3; }
        release_block(block_id);
        close(file_state);
    } else { return -1; }
    return 0;
}
int assign_block(void)
{int i;
    for(i = blocks_from; i < BLOCKS_NUMBER; i++) {
        if(map_check(i) == false) {map_set(i);
            if(update_super_block(file_state)) {return i;} else {return 0;}}}
    return 0;
}

int load_super_block(int fd)
{
    if(lseek(fd, strlen(HEADER), SEEK_SET) < 0) {return 0;}
    if(read(fd, block_map, BLOCKS_NUMBER/BYTE_SIZE) != BLOCKS_NUMBER/BYTE_SIZE) {return 0;}
    return 1;}
int create_filesystem(const char *path)
{
    struct stat st;
    if(file_state < 0) {
        file_state = open(path, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        if(file_state < 0) {return -1;}
        if(fstat(file_state, &st) < 0) {return -2;}
        long i;
        uint8_t zero = 0;
        for(i = 0; i < TOTAL_NUMBER_OF_BYTES; i++) {
            if(write(file_state,(void *)&zero, sizeof(zero)) != 1) {break;}}
        if(lseek(file_state, 0, SEEK_SET) < 0) { return -3;}
        write(file_state, HEADER, strlen(HEADER));
        snprintf(file_system_path, 100, "%s", path);
        for(i = 0; i <= 513; i++) {map_set(i);}//reverse the first 514 blocks for inodes.
        update_super_block(file_state);
        close(file_state);}
    num_blocks = BLOCKS_NUMBER;
    return num_blocks;
}

int load_filesystem(const char *path)
{
    struct stat st;
    if(file_state < 0) {
        if((file_state = open(path, O_RDWR)) < 0) {return -1;}
        if(fstat(file_state, &st) < 0) {return -2;}
        char checker[11] = {0};
        if(read(file_state, checker, strlen(HEADER)) != strlen(HEADER)){return -3;}
        else {if(strcmp(checker, HEADER) != 0) {return -4;}}
        memset(block_map, 0, BLOCKS_NUMBER/BYTE_SIZE);
        load_super_block(file_state);
        uint8_t i;
        snprintf(file_system_path, 100, "%s", path);
        close(file_state);
        return 0;} else {return -1;}
}

int modify_super_block(void *block, int block_input_length)
{
    int byte_written;
    if(file_state > 0) {
        if((file_state = open(file_system_path, O_RDWR)) < 0) {return -1;}
        if(lseek(file_state, strlen(HEADER) + BLOCKS_NUMBER/BYTE_SIZE, SEEK_SET) < 0) {return -3;}
        if(block_input_length <SIZE_OF_SUPER_BLOCK) {
            if(write(file_state, block, block_input_length) != block_input_length) {return -4;}
            byte_written = block_input_length;} else {
            if(write(file_state, block, SIZE_OF_SUPER_BLOCK) != SIZE_OF_SUPER_BLOCK) {return -4;}
            byte_written = SIZE_OF_SUPER_BLOCK;}
        close(file_state);
    } else {return -1;}
    return byte_written;
}

int read_super_block(void *block, int block_output_length)
{
    int byte_read;
    if(file_state > 0) {
        if((file_state = open(file_system_path, O_RDWR)) < 0) {return -1;}
        if(lseek(file_state, strlen(HEADER) + BLOCKS_NUMBER/BYTE_SIZE, SEEK_SET) < 0) {return -3;}
        if(read(file_state, block, block_output_length) != block_output_length) {return -4;}
        byte_read = block_output_length;
        close(file_state);
    } else {return -1;}
    return byte_read;
}

int modify_block(const int block_id, void *block, int block_input_length)
{
    int byte_written = 0;
    if(file_state > 0) {
        if((file_state = open(file_system_path, O_RDWR)) < 0) {return -1;}
        if(block_id >= SIZE_OF_BLOCK ||  block_id == 0 || map_check(block_id) == false) {return -2;}
        if(lseek(file_state, block_id * SIZE_OF_BLOCK, SEEK_SET) < 0) {return -3;}
        if(block_input_length < SIZE_OF_BLOCK) {
            if(write(file_state, block, block_input_length) != block_input_length) {return -4;}
            byte_written = block_input_length;}
        else {if(write(file_state, block, SIZE_OF_BLOCK) != SIZE_OF_BLOCK) {return -4;}
            byte_written = SIZE_OF_BLOCK;}
        close(file_state);} else {return -1;}
    return byte_written;
}

int write_block(int *block_id, void *block, int block_input_length) {
    int byte_written = 0;
    if (file_state > 0) {
        if ((file_state = open(file_system_path, O_RDWR)) < 0) {return -1;}
        if ((*block_id = assign_block()) != 0) {
            if (lseek(file_state, (*block_id) * SIZE_OF_BLOCK, SEEK_SET) < 0) {return -3;}
            if (block_input_length < SIZE_OF_BLOCK) {
                if (write(file_state, block, block_input_length) != block_input_length) {return -4;}
                char zero = 0x00;
                for (int i = 0; i < SIZE_OF_BLOCK - block_input_length; i++) {write(file_state, &zero, sizeof(zero));}
                byte_written = block_input_length;} else {if (write(file_state, block, SIZE_OF_BLOCK) != SIZE_OF_BLOCK) {return -4;}
                byte_written = SIZE_OF_BLOCK;}}
        else {return -2;}
        close(file_state);}
    else {return -1;}return byte_written;
}

int read_block(const int block_id, void *block)
{
    int byte_read = 0;
    if(file_state > 0) {
        if((file_state = open(file_system_path, O_RDWR)) < 0) {return -1;}
        if(block_id >= BLOCKS_NUMBER || block_id == 0 || map_check(block_id) == false) {return -2;}
        if(lseek(file_state, block_id * SIZE_OF_BLOCK, SEEK_SET) < 0) {return -3;}
        char tmp[SIZE_OF_BLOCK] = {0};
        if((byte_read = read(file_state, tmp, SIZE_OF_BLOCK)) != SIZE_OF_BLOCK) {return -4;}
        memcpy(block, tmp, SIZE_OF_BLOCK);
        close(file_state);} else {return -1;}
    return byte_read;
}


//
// Created by Артур Слепнев on 3/12/21.
//

