// Tecnológico de Costa Rica
// Area Académica de Ingeniería en Computadores
// CE4303 Sistemas Operativos - Tarea 1
// Por Joseph Vargas y Gustavo Segura
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include "config.h"
#include <gnu/libc-version.h>

// #include "multipartparser.h"

// Constants
#define CONFIG_FILE "./server.conf"

// Function definitions
int server_main(int port, const char *save_folder, const char *log_file);
int image_main(const char *save_folder, const char *colors_folder, const char *histo_folder, const char *log_file);
int parseHTTP(char* buffer);

void *memmem(const void *haystack, size_t hlen, const void *needle, size_t nlen);






int main(int argc, char const *argv[])
{
    



    // Create conf object
    ini_table_s *config = ini_table_create();
    // Check if exists
    if (!ini_table_read_from_file(config, CONFIG_FILE))
    {
        puts("Error: Config file not found");
        return EXIT_FAILURE;
    }

    // Load configuration in variables
    const char *save_dir = ini_table_get_entry(config, "ImageServer", "save_dir");
    const char *colors_dir = ini_table_get_entry(config, "ImageServer", "colors_dir");
    const char *histo_dir = ini_table_get_entry(config, "ImageServer", "histo_dir");
    const char *log_file = ini_table_get_entry(config, "ImageServer", "log_file");
    int *port = malloc(sizeof(int));
    ini_table_get_entry_as_int(config, "ImageServer", "port", port);

    // Debug
    printf("save_dir is: %s\n", save_dir);
    printf("colors_dir is: %s\n", colors_dir);
    printf("histo_dir is: %s\n", histo_dir);
    printf("log_file is: %s\n", histo_dir);
    printf("port is: %i\n", *port);

    // TODO: Threads
    server_main(*port, save_dir, log_file);
    image_main(save_dir, colors_dir, histo_dir, log_file);

    // Destroy when the program exits
    ini_table_destroy(config);
}

int server_main(int port, const char *save_folder, const char *log_file)
{
    int server_fd, new_socket;
    long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Only this line has been changed. Everything is same.
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!\n";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("In socket");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

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

        // char buffer[3000000] = {0};
        char *buffer = (char *) malloc(300000);
        memset(buffer, '\0', sizeof(char)*300000);

        valread = read(new_socket, buffer, 300000);

        parseHTTP(buffer);

        // printf("________________________\n\n\n%s\n", buffer);
        free(buffer);
        write(new_socket, hello, strlen(hello));
        printf("------------------Hello message sent-------------------");
        close(new_socket);
    }
    return 0;
}

int parseHTTP(char* buffer){

    char *s0, *s1;
    s0 = strstr(buffer, "boundary=");
    int l0 = strlen("boundary=");
    printf("funcion llamada\n");

    if(s0 != NULL){

        printf("boundary encontrado\n");
        s1 = strstr(s0, "\n");
        char boundary[45];
        memset(boundary, '\0', sizeof(boundary));
        strncpy(boundary, s0 + l0, s1 - s0 - l0);
    
        printf("BOUNDARY: %s\n", boundary);
        int boundarylen= strlen(boundary);
        char *file_start =  strstr(buffer, "Content-Disposition:");
//         char *file_start =  strstr(buffer, "PNG");
        
        if(file_start != NULL){
            printf("XXXXXXXXXXXXXXXXXXXXXX INICIO ENCONTRADO XXXXXXXXXXXXXXXx\n");
            file_start = strstr(file_start+1 ,"\n");
            file_start = strstr(file_start+1 ,"\n");

//             printf("XXXXXXXXXXXXXXXXXXXXXX BUFFER FS XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n\n%s\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx\n", file_start);
//             char *file_end = strstr(file_start ,boundary);
//             char *file_end = memmem(file_start, sizeof(char)*(300000), boundary, sizeof(char)*boundarylen);
            1+1;
//             if (file_end!= NULL){
            FILE *fp;
            fp = fopen("test_file.png", "wb");
            fwrite(file_start+3,sizeof(char),300000-(file_start-buffer),fp);
            // fwrite(boundary,sizeof(char),boundarylen,fp);
            fclose(fp);
//             }
//             else{
//                 printf("no encontró el borde\n");
//             }
        }
        else{
            printf("file start error\n");
        }
        return 1;
    }
    else{
        return 0;
    }
}



/*
 * The memmem() function finds the start of the first occurrence of the
 * substring 'needle' of length 'nlen' in the memory area 'haystack' of
 * length 'hlen'.
 *
 * The return value is a pointer to the beginning of the sub-string, or
 * NULL if the substring is not found.
 */
void *memmem(const void *haystack, size_t hlen, const void *needle, size_t nlen)
{
    int needle_first;
    const void *p = haystack;
    size_t plen = hlen;

    if (!nlen)
        return NULL;

    needle_first = *(unsigned char *)needle;

    while (plen >= nlen && (p = memchr(p, needle_first, plen - nlen + 1)))
    {
        if (!memcmp(p, needle, nlen))
            return (void *)p;

        p++;
        plen = hlen - (p - haystack);
    }

    return NULL;
}


int image_main(const char *save_folder, const char *colors_folder, const char *histo_folder, const char *log_file)
{
    return 0;
}
