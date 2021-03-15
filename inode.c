#include "filesystem_and_blocks.h"
#include "inode.h"
int init_superblock()
{
    memset (&superblock_inode, 0,  sizeof(superblock_inode));
    read_block(SUPERBLOCK_ID, &superblock_inode);
    return 0;}

int init_inode(inode *inode_, size_t length){memset (inode_, 0, length);return 0;}

int set_inode_bitmap (int id)
{
    superblock_inode.inode_map[id>>3] |= (unsigned long long) 1 << (id%BYTE_SIZE);
    modify_block(SUPERBLOCK_ID, &superblock_inode, (int)(sizeof (superblock_inode)));
    return 0;}

int clear_inode_bitmap (int id)
{
    superblock_inode.inode_map[id>>3] &= ~((unsigned long long) 1 << (id%BYTE_SIZE));
    modify_block(SUPERBLOCK_ID, &superblock_inode, (int)(sizeof (superblock_inode)));
    return 0;}

int query_inode_bitmap (int id)
{
    int target_bit;
    target_bit = superblock_inode.inode_map[id>>3] >> ( id % 8 );
    target_bit = target_bit % 2;
    return target_bit;}

int query_inode (inode *inode_)
{
    int inode_block_id = inode_->inode_id / (SIZE_OF_BLOCK / INODE_SIZE) + 2;
    int inode_block_seq = inode_->inode_id % (SIZE_OF_BLOCK / INODE_SIZE);
    inode_group inode_group;
    if(!query_inode_bitmap(inode_->inode_id)) {
        return -1;
    }
    read_block(inode_block_id, &inode_group);
    if(inode_->inode_id == inode_group.inode_array[inode_block_seq].inode_id) {
        memcpy(inode_, &inode_group.inode_array[inode_block_seq], sizeof(inode));
    } else {return -1;}
    return 0;
}

int update_inode (inode *inode_)
{
    int ret;
    int inode_block_id = inode_->inode_id / (SIZE_OF_BLOCK / INODE_SIZE) + 2;
    int inode_block_seq = inode_->inode_id % (SIZE_OF_BLOCK / INODE_SIZE);
    inode_group inode_group;
    read_block (inode_block_id, (void *)&inode_group);
    inode_group.inode_array[inode_block_seq] = *inode_;
    ret = modify_block( inode_block_id, &inode_group, (int)(sizeof(inode_group)) );
    return 0;
}

int create_inode (inode *inode_)
{
    int target_id;
    int block_ret;
    inode_->file_id = 0;
    inode_->group_id = 0;
    int i;
    for(i = 0; i < 15; i++) {inode_->num[i] = 0;}
    i = 0;
    while (1) {
        if (query_inode_bitmap(i) == 0) {
            target_id = i;
            set_inode_bitmap(target_id);
            inode_->inode_id = target_id;
            break;} else {i++;}
        if (i >= INODE_NUM) {return -1;}}
    modify_block(SUPERBLOCK_ID, &superblock_inode, (int)(sizeof (superblock_inode)));
    update_inode(inode_);
    return 0;
}

int delete_inode (inode *inode_){clear_inode_bitmap(inode_->inode_id);return 0;}

int delete_file (inode *inode_)
{
    struct single_indirect_block single_indirect_block;
    int block_inuse;
    int read_block_return;
    if(inode_->num[INDIRECT_BLOCK_GROUP_SIZE - 1] != 0) {
        read_block_return = read_block(inode_->num[INDIRECT_BLOCK_GROUP_SIZE - 1], (void *)&single_indirect_block);
    }
    for (int i = 0; i < INDIRECT_BLOCK_GROUP_SIZE; i++) {if (inode_->num[i] != 0) {delete_block(inode_->num[i]);}
    else {break;}}
    if(inode_->num[INDIRECT_BLOCK_GROUP_SIZE - 1] != 0) {
        for (int i = 0; i < SIZE_OF_BLOCK / 8; i++) {
            if (single_indirect_block.num12[i] != 0) {
                delete_block(single_indirect_block.num12[i]);
            } else {break;}}}
    clear_inode_bitmap(inode_->inode_id);
    return 0;
}

int read_file (inode *inode_, void* file)
{
    single_indirect_block single_indirect_block;
    int block_inuse;
    int read_block_return;
    int next_block_id;
    char file_buffer[SIZE_OF_BLOCK];
    if(inode_->file_size == 0) {
        file_buffer[0] = '\0';
        memmove(file + inode_->file_size, &file_buffer[0], 1 );
        return 0;}
    if(inode_->num[INDIRECT_BLOCK_GROUP_SIZE - 1] != 0) {
        read_block_return = read_block(inode_->num[INDIRECT_BLOCK_GROUP_SIZE - 1], (void *)&single_indirect_block);}
    int i = 0;
    next_block_id = inode_->num[i];
    while(next_block_id != 0) {
        read_block(next_block_id, file_buffer);
        if(i+1 < INDIRECT_BLOCK_GROUP_SIZE) {
            next_block_id = inode_->num[i+1];}
        else {next_block_id = single_indirect_block.num12[i-12 + 1];}
        if(next_block_id != 0) {memmove(file + i * SIZE_OF_BLOCK, file_buffer, SIZE_OF_BLOCK);
            memset(file_buffer, 0, sizeof(file_buffer));}
        else {memmove(file + i * SIZE_OF_BLOCK, file_buffer, inode_->file_size - i*SIZE_OF_BLOCK );
            memset(file_buffer, 0, sizeof(file_buffer));
            file_buffer[0] = '\0';
            memmove(file + inode_->file_size, &file_buffer[0], 1 );
            memset(file_buffer, 0, sizeof(file_buffer));}
        i++;}
    return 0;
}

int write_file (inode *inode_, void* file)
{
    single_indirect_block single_indirect_block;
    int block_inuse;
    int block_return;
    int read_block_return;
    char file_buffer[SIZE_OF_BLOCK];
    int block_id_buf;
    int block_data_len;
    if(inode_->num[INDIRECT_BLOCK_GROUP_SIZE] == 0) {
        for (int i = 0; i < INDIRECT_BLOCK_GROUP_SIZE; i++) {
            if (inode_->num[i] == 0) {
                block_inuse = i;
                break;}}}
    else {
        for (int i=0; i < SIZE_OF_BLOCK / 8; i++) {if (single_indirect_block.num12[i] == 0) {
                block_inuse = i + (INDIRECT_BLOCK_GROUP_SIZE);
                break;}
        }
    }
    int i;
    if(block_inuse < inode_->file_size / SIZE_OF_BLOCK + 1) { // block num will increase
        // modify blocks until there is no block available
        for(i = 0; i < block_inuse; i++) {
            memcpy(file_buffer, file + i*SIZE_OF_BLOCK, SIZE_OF_BLOCK);
            if(i < INDIRECT_BLOCK_GROUP_SIZE) {modify_block(inode_->num[i], &file_buffer, SIZE_OF_BLOCK);}
            else {modify_block(
                    single_indirect_block.num12[i - INDIRECT_BLOCK_GROUP_SIZE], &file_buffer, SIZE_OF_BLOCK);}}
        for(; i < inode_->file_size / SIZE_OF_BLOCK + 1; i++) {
            // determine the length of block and copy block into buffer
            if(i+1 == inode_->file_size / SIZE_OF_BLOCK + 1) { // last block of file
                block_data_len = inode_->file_size % SIZE_OF_BLOCK;
                //LOG_DEBUG("Block_num increase - Write Blocks. file_size is %d. block_data_len is %d", inode_->file_size, block_data_len);
            } else {block_data_len = SIZE_OF_BLOCK;}
            memcpy(file_buffer,file + i*SIZE_OF_BLOCK, block_data_len);
            write_block(&block_id_buf, &file_buffer, block_data_len);
            if(i < INDIRECT_BLOCK_GROUP_SIZE) {inode_->num[i] = block_id_buf;}
            else {single_indirect_block.num12[i - INDIRECT_BLOCK_GROUP_SIZE] = block_id_buf;}}
        if(inode_->num[INDIRECT_BLOCK_GROUP_SIZE] != 0) {modify_block(inode_->num[12], &single_indirect_block,
                (int)(sizeof(single_indirect_block)));} else {
            if(inode_->file_size / SIZE_OF_BLOCK >= INDIRECT_BLOCK_GROUP_SIZE) {
                write_block(&block_id_buf, &single_indirect_block, (int)(sizeof(single_indirect_block)));
                inode_->num[INDIRECT_BLOCK_GROUP_SIZE] = block_id_buf;}}}
    else if(block_inuse > inode_->file_size / SIZE_OF_BLOCK + 1){
        for(i = block_inuse - (inode_->file_size / SIZE_OF_BLOCK + 1); i < block_inuse; i++ ) {
            if(i < INDIRECT_BLOCK_GROUP_SIZE) {
                delete_block(inode_->num[i]);
                inode_->num[i] = 0;
            } else {delete_block(single_indirect_block.num12[i - INDIRECT_BLOCK_GROUP_SIZE]);
                single_indirect_block.num12[i - INDIRECT_BLOCK_GROUP_SIZE] = 0;}}
        for(i = 0; i < inode_->file_size / SIZE_OF_BLOCK + 1; i++) {
            if(i == inode_->file_size / SIZE_OF_BLOCK + 1) {
                block_data_len = inode_->file_size % SIZE_OF_BLOCK;
            } else {block_data_len = SIZE_OF_BLOCK;}
            memcpy(file_buffer, file + i*SIZE_OF_BLOCK, block_data_len);
            if(i < INDIRECT_BLOCK_GROUP_SIZE) {
                modify_block(inode_->num[i], &file_buffer, block_data_len);
            } else {
                modify_block(single_indirect_block.num12[i - INDIRECT_BLOCK_GROUP_SIZE], &file_buffer, block_data_len);
            }}
        if(inode_->file_size / SIZE_OF_BLOCK + 1 < INDIRECT_BLOCK_GROUP_SIZE) {
            delete_block(inode_->num[INDIRECT_BLOCK_GROUP_SIZE]);
            inode_->num[INDIRECT_BLOCK_GROUP_SIZE] = 0;} else {
            if(inode_->num[INDIRECT_BLOCK_GROUP_SIZE] != 0) {modify_block(inode_->num[INDIRECT_BLOCK_GROUP_SIZE],
                    &single_indirect_block, (int)(sizeof(single_indirect_block)));}
            else {write_block(&block_id_buf, &single_indirect_block, (int)(sizeof(single_indirect_block)));
                inode_->num[INDIRECT_BLOCK_GROUP_SIZE] = block_id_buf;}}
    } else if(block_inuse == inode_->file_size / SIZE_OF_BLOCK + 1) {
        for(i = 0; i < inode_->file_size / SIZE_OF_BLOCK + 1; i++) {
            if (i + 1 == inode_->file_size / SIZE_OF_BLOCK + 1) { block_data_len = inode_->file_size % SIZE_OF_BLOCK; }
            else { block_data_len = SIZE_OF_BLOCK; }
            memcpy(file_buffer, file + i * SIZE_OF_BLOCK, block_data_len);
            if (i < INDIRECT_BLOCK_GROUP_SIZE) { modify_block(inode_->num[i], &file_buffer, block_data_len); }
            else {
                modify_block(single_indirect_block.num12[i - INDIRECT_BLOCK_GROUP_SIZE], &file_buffer, block_data_len);
            }
        }}
    update_inode(inode_);
    return 0;
}



//
// Created by Артур Слепнев on 3/13/21.
//

