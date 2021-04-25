#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>

#include "commands.h"


int server_fd, client_fd;



int main (int argc, char *argv[]) {
    if (argc < 2) {
        printf("Use: %s [filename] [port]\n", argv[0]);
        return 1;
    }

    pid_t pid;

    pid = fork();

    if (pid < 0) return 1;
    if (pid > 0) return 0;
    if (setsid() < 0) return 1;

    pid = fork();

    if (pid < 0) return 1;
    if (pid > 0) return 0;

    umask(0);

    Init(argv[1]);

    int port = atoi(argv[2]);

    int res;
    struct sockaddr_in server, client;
    const size_t buffer_size = 8192;
    char buffer[buffer_size];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        printf("Could not create socket\n");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    int opt_val = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof opt_val);

    res = bind(server_fd, (struct sockaddr *) &server, sizeof(server));
    if (res < 0) {
        printf("Could not bind socket\n");
        return 1;
    }

    res = listen(server_fd, 128);
    if (res < 0) {
        printf("Could not listen on socket\n");
        return 1;
    }

    printf("Server is listening on %d\n", port);

    while (1) {
        socklen_t client_len = sizeof(client);
        client_fd = accept(server_fd, (struct sockaddr *) &client, &client_len);

        if (client_fd < 0) {
            printf("Could not establish new connection\n");
            continue;
        }

        size_t bytes_read = 0;

        while (1) {
            bytes_read = recv(client_fd, buffer, buffer_size, 0);
            if (bytes_read >= buffer_size) {
                send(client_fd, "Message is too long!\n", 22, 0);
                close(client_fd);
                break;
            }
            if (!bytes_read) break;
            if (bytes_read < 0) {
                printf("Client read failed\n");
                continue;
            }

            if (buffer[bytes_read - 2] == '\r') {
                buffer[bytes_read - 2] = 0;
                bytes_read -= 2;
            }

            if ((strcmp(buffer, "stop\n") == 0 || strcmp(buffer, "q\n") == 0)) {
                send(client_fd, "Stopping\n", 10, 0);
                shutdown(client_fd, SHUT_RDWR);
                close(client_fd);
                close(server_fd);
                return 0;
            } else if (strcmp(buffer, "help") == 0) {
                char* string="";
                send(client_fd, string,strlen(string) , 0);
                continue;}


            size_t first_space;
            for (first_space = 0; buffer[first_space] != ' ' && first_space < bytes_read; first_space++);
            size_t second_space;
            for (second_space = first_space + 1; buffer[second_space] != ' ' && second_space < bytes_read; second_space++);

            if (first_space == 0) {
                send(client_fd, "Unknown command\n", 30, 0);
                continue;
            }

            if (second_space == 0) second_space = bytes_read;

            char* command = malloc(first_space + 1);
            memcpy(command, buffer, first_space);
            command[first_space] = 0;

            char* path = malloc(second_space - first_space + 1);
            memcpy(path, buffer + first_space + 1, second_space - first_space - 1);
            path[second_space - first_space - 1] = 0;

            if (strcmp(command, "ls") == 0) {
                int size = //size
                if (size >= 0) {
                    char* content = malloc(size);
                    handle_error(client_fd, fs_read(&fs, path, content, size));
                    send(client_fd, content, size, 0);
                    free(content);
                } else {
                    handle_error(client_fd, size);
                }
            } else if (strcmp(command, "touch") == 0) {
                res =//touch
                if (res < 0) {
                    handle_error(client_fd, res);
                } else {
                    send(client_fd, "OK\n", 4, 0);
                }
            } else if (strcmp(command, "echo") == 0) {
                res = //echo
                if (res < 0) {
                    handle_error(client_fd, res);
                } else {
                    send(client_fd, "OK\n", 4, 0);
                }
            } else if (strcmp(command, "rm") == 0) {
                res = //remove
                if (res < 0) {
                   return 1
                } else {
                    send(client_fd, "OK\n", 4, 0);
                }
            } else {
                send(client_fd, "Unknown command\n", 17, 0);
            }

            free(path);
            free(command);
        }
    }

    return 0;}


//
// Created by Артур Слепнев on 4/25/21.
//

