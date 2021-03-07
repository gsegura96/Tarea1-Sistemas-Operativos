// Server side C program to demonstrate HTTP Server programming
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>

#include "config.h"

#define PORT 8085

int server_main(int port, char *save_folder, char *log_folder);
int image_main(char *save_folder, char *colors_folder, char *histo_folder, char *log_folder);

int main(int argc, char const *argv[])
{
    ini_table_s *config = ini_table_create();

    char *test = "HOLA MUNDO TAVO TE QUIERO";

    server_main(PORT, test, test);
    image_main(test, test, test, test);
}

int server_main(int port, char *save_folder, char *log_folder)
{
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Only this line has been changed. Everything is same.
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    memset(address.sin_zero, '\0', sizeof address.sin_zero);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("In bind");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)
    {
        perror("In listen");
        exit(EXIT_FAILURE);
    }
    while (1)
    {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("In accept");
            exit(EXIT_FAILURE);
        }

        char buffer[3000000] = {0};
        valread = read(new_socket, buffer, 3000000);

        char *s0, *s1;
        s0 = strstr(buffer, "boundary="); // search for string "hassasin" in buff
        int l0 = strlen("boundary=");

        if (s0 != NULL) // if successful then s now points at "hassasin"
        {
            printf("Found string at index = %d\n", s0 - buffer);

            s1 = strstr(s0, "\n"); // search for string "hassasin" in buff

            char boundary[40];
            memset(boundary, '\0', sizeof(boundary));
            strncpy(boundary, s0 + l0, s1 - s0 - l0);

            //             printf("%s\n",s );
            //             printf("%.*s\n",s1-s0-l0,s0+l0 );
            printf("BOUNDARY: %s\n", boundary);

        } // index of "hassasin" in buff can be found by pointer subtraction
        else
        {
            printf("String not found\n"); // `strstr` returns NULL if search string not found
        }

        printf("________________________\n\n\n%s\n", buffer);
        write(new_socket, hello, strlen(hello));
        printf("------------------Hello message sent-------------------");
        close(new_socket);
    }
    return 0;
}

int image_main(char *save_folder, char *colors_folder, char *histo_folder, char *log_folder)
{
    return 0;
}