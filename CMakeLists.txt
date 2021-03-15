cmake_minimum_required(VERSION 3.15)
project(FS C)

set(CMAKE_C_STANDARD 99)

add_executable(FS main.c filesystem_and_blocks.h inode.h filesystem_and_blocks.c inode.c directories_and_files.c directories_and_files.h commands.h commands.c)