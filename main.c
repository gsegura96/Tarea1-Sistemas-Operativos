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
#include <signal.h>
#include <pthread.h>
#include <stdbool.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "uini.h"

// Constants
#define CONFIG_FILE "./server.conf"
typedef struct
{
    int port;
    const char *save_dir;
    const char *colors_dir;
    const char *histo_dir;
    const char *log_file;
} ConfParams;

// Function definitions
void *image_main(void *context);
void *server_main(void *context);

// Program running variable
bool running = true;
struct sigaction action;
// Handle stop
void handle_stop(int signum)
{
    puts("Exiting from ImageServer...");
    running = false;
}

void conf_handler(const char *section, const char *key, const char *value, void *params_pointer)
{
    ConfParams *params = params_pointer;
    if (strcmp(key, "save_dir") == 0)
    {
        params->save_dir = strdup(value);
    }
    else if (strcmp(key, "colors_dir") == 0)
    {
        params->colors_dir = strdup(value);
    }
    else if (strcmp(key, "histo_dir") == 0)
    {
        params->histo_dir = strdup(value);
    }
    else if (strcmp(key, "log_file") == 0)
    {
        params->log_file = strdup(value);
    }
    else if (strcmp(key, "port") == 0)
    {
        params->port = atoi(value);
    }
}

int main(int argc, char const *argv[])
{
    signal(SIGTERM, handle_stop);

    ConfParams params;

    // Create conf object
    FILE *f = fopen(CONFIG_FILE, "r");
    uini_parse(f, conf_handler, &params);
    fclose(f);

    // TODO: Check if exists
    if (false)
    {
        puts("Error: Config file not found");
        return EXIT_FAILURE;
    }

    // Debug
    printf("save_dir is: %s\n", params.save_dir);
    printf("colors_dir is: %s\n", params.colors_dir);
    printf("histo_dir is: %s\n", params.histo_dir);
    printf("log_file is: %s\n", params.log_file);
    printf("port is: %i\n", params.port);

    // Main threads
    pthread_t image_thread;
    pthread_t server_thread;
    pthread_create(&image_thread, NULL, image_main, &params);
    pthread_create(&server_thread, NULL, server_main, &params);

    pthread_join(image_thread, NULL);
    pthread_join(server_thread, NULL);

    return 0;
}

void *server_main(void *context)
{
    ConfParams *params = context;

    printf("PORT FROM SERVER THREAD: %i\n", params->port);
    printf("LOGFILE FROM SERVER THREAD: %s\n", params->log_file);

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
    address.sin_port = htons(params->port);

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
    while (running)
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
}

void get_pixel(stbi_uc *image, size_t imageWidth, size_t x, size_t y, stbi_uc *r, stbi_uc *g, stbi_uc *b)
{
    *r = image[4 * (y * imageWidth + x) + 0];
    *g = image[4 * (y * imageWidth + x) + 1];
    *b = image[4 * (y * imageWidth + x) + 2];
}

void *image_main(void *context)
{
    ConfParams *params = context;

    printf("PORT FROM IMG THREAD: %i\n", params->port);
    printf("LOGFILE FROM IMG THREAD: %s\n", params->log_file);

    while (running)
    {
        int width, height; // image width, heigth,
        stbi_uc *image = stbi_load("test_img/img.png", &width, &height, NULL, 4);
        printf("w: %i\n", width);
        printf("h: %i\n", height);

        stbi_uc r, g, b;
        unsigned long int r_sum = 0, g_sum = 0, b_sum = 0;

        for (int x = 0; x < 30; x++)
        {
            for (int y = 0; y < 30; y++)
            {
                get_pixel(image, width, x, y, &r, &g, &b);
                r_sum += r;
                g_sum += g;
                b_sum += b;
            }
        }

        printf("r: %li\n", r_sum);
        printf("g: %li\n", g_sum);
        printf("b: %li\n", b_sum);
        sleep(1);
    }
}