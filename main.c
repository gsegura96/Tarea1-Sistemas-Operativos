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
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Constants
#define CONFIG_FILE "./server.conf"

// Function definitions
int image_main(const char *save_folder, const char *colors_folder, const char *histo_folder, const char *log_file);
int server_main(int port, const char *save_folder, const char *log_file);

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
    image_main(save_dir, colors_dir, histo_dir, log_file);
    server_main(*port, save_dir, log_file);

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
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";

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

void get_pixel(stbi_uc *image, size_t imageWidth, size_t x, size_t y, stbi_uc *r, stbi_uc *g, stbi_uc *b)
{
    *r = image[4 * (y * imageWidth + x) + 0];
    *g = image[4 * (y * imageWidth + x) + 1];
    *b = image[4 * (y * imageWidth + x) + 2];
}

int image_main(const char *save_folder, const char *colors_folder, const char *histo_folder, const char *log_file)
{
    int width, height; // image width, heigth,
    stbi_uc *image = stbi_load("test_img/green.jpg", &width, &height, NULL, 3);
    printf("w: %i\n", width);
    printf("h: %i\n", height);

    stbi_uc r, g, b;

    get_pixel(image, width, 0, 0, &r, &g, &b);

    printf("r: %i\n", r);
    printf("g: %i\n", g);
    printf("b: %i\n", b);

    return 0;
}