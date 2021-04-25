#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>

#define NO_SPACE -1
#define READ_FAILURE -2
#define WRITE_FAILURE -3
#define TOO_SMALL_BUFFER -4
#define WRONG_FILE_TYPE -5
#define NOT_FOUND -6
#define WRONG_INPUT -7



void handle_error(int client_fd, int res) {
    switch (res) {
        case NO_SPACE:
            send(client_fd, "No space left, file may be too big\n", 36, 0);
            break;
        case READ_FAILURE:
            send(client_fd, "Read failure\n", 14, 0);
            break;
        case WRITE_FAILURE:
            send(client_fd, "Write failure, file system may be corrupted\n", 45, 0);
            break;
        case TOO_SMALL_BUFFER:
            send(client_fd, "Technical error: too small buffer\n", 35, 0);
            break;
        case WRONG_FILE_TYPE:
            send(client_fd, "Wrong file type\n", 17, 0);
            break;
        case NOT_FOUND:
            send(client_fd, "File not found\n", 16, 0);
            break;
        case WRONG_INPUT:
            send(client_fd, "Wrong input\n", 13, 0);
            break;
        default:
            break;
    }
}
//
// Created by Артур Слепнев on 4/25/21.
//

