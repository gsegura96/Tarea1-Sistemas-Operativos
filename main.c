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
#include <dirent.h>
#include <sys/stat.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "uini.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


// Constants
#define CONFIG_FILE "./server.conf"
// Struct to contain the configuration
typedef struct
{
    int port;
    const char *save_dir;
    const char *colors_dir;
    const char *histo_dir;
    const char *log_file;
} ConfParams;

// Function definitions
int parseHTTP(char *buffer);

void *image_main(void *context);
void *server_main(void *context);
void RGB_to_grayscale(stbi_uc *image,int height, int width);





// Program running variable
bool running = true;
struct sigaction action;
// Handle stop
void handle_stop(int signum)
{
    puts("Exiting from ImageServer...");
    running = false;
}
// Handle the conf file
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
    char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!\n";

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

        // char buffer[3000000] = {0};
        char *buffer = (char *)malloc(300000);
        memset(buffer, '\0', sizeof(char) * 300000);

        valread = read(new_socket, buffer, 300000);
        
        parseHTTP(buffer);

        // printf("________________________\n\n\n%s\n", buffer);
        free(buffer);
        write(new_socket, hello, strlen(hello));
        printf("------------------Hello message sent-------------------");
        close(new_socket);
    }
}

int parseHTTP(char *buffer)
{
    printf("XXXXXXXXXXXXXXXXXXXXXX BUFFER XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n\n%s\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx\n", buffer);
    char *s0, *s1;
    s0 = strstr(buffer, "boundary=");
    int l0 = strlen("boundary=");
    printf("funcion llamada\n");

    if (s0 != NULL)
    {
        
        printf("boundary encontrado\n");
        s1 = strstr(s0, "\n");
        char boundary[45];
        memset(boundary, '\0', sizeof(boundary));
        strncpy(boundary, s0 + l0, s1 - s0 - l0);

        printf("BOUNDARY: %s\n", boundary);
        int boundarylen = strlen(boundary);
        char *file_start = strstr(buffer, "Content-Disposition:");
        //         char *file_start =  strstr(buffer, "PNG");
        1+1;
        if (file_start != NULL)
        {
            printf("XXXXXXXXXXXXXXXXXXXXXX INICIO ENCONTRADO XXXXXXXXXXXXXXXx\n");
            file_start = strstr(file_start + 1, "\n");
            file_start = strstr(file_start + 1, "\n");

            //             printf("XXXXXXXXXXXXXXXXXXXXXX BUFFER FS XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n\n%s\nXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx\n", buffer);
            //             char *file_end = strstr(file_start ,boundary);
            //             char *file_end = memmem(file_start, sizeof(char)*(300000), boundary, sizeof(char)*boundarylen);
            1 + 1;
            //             if (file_end!= NULL){
            FILE *fp;
            fp = fopen("test_file.png", "wb");
            fwrite(file_start + 3, sizeof(char), 300000 - (file_start - buffer), fp);
            // fwrite(boundary,sizeof(char),boundarylen,fp);
            fclose(fp);
            //             }
            //             else{
            //                 printf("no encontró el borde\n");
            //             }
        }
        else
        {
            printf("file start error\n");
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

void get_pixel(stbi_uc *image, size_t imageWidth, size_t x, size_t y, stbi_uc *r, stbi_uc *g, stbi_uc *b)
{
    *r = image[4 * (y * imageWidth + x) + 0];
    *g = image[4 * (y * imageWidth + x) + 1];
    *b = image[4 * (y * imageWidth + x) + 2];
}

void set_pixel(stbi_uc *image, size_t imageWidth, size_t x, size_t y, stbi_uc r, stbi_uc g, stbi_uc b)
{
    image[4 * (y * imageWidth + x) + 0] = r;
    image[4 * (y * imageWidth + x) + 1] = g;
    image[4 * (y * imageWidth + x) + 2] = b;
}

char *get_smallest_file(char *directory)
{
    DIR *d;
    struct dirent *dir;
    struct stat st;
    unsigned long int max_size = 0;
    char *current_file = malloc(sizeof(char) * 512);
    d = opendir(directory);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_DIR)
            {
                continue;
            }
            char path[512];
            strcpy(path, directory);
            strcat(path, "/");
            strcat(path, dir->d_name);
            stat(path, &st);

            if (max_size == 0 || st.st_size < max_size)
            {
                max_size = st.st_size;
                strcpy(current_file, path);
            }
        }
        closedir(d);
    }
    return current_file;
}

void *image_main(void *context)
{
    ConfParams *params = context;

    printf("PORT FROM IMG THREAD: %i\n", params->port);
    printf("LOGFILE FROM IMG THREAD: %s\n", params->log_file);

    printf("Smallest file: %s\n", get_smallest_file("."));

    while (running)
    {
        int width, height; // image width, heigth,
        stbi_uc *image = stbi_load("test_img/img.png", &width, &height, NULL, 4);
        printf("w: %i\n", width);
        printf("h: %i\n", height);
        
        
//         stbi_write_png("test_img/img_out.png", width,  height, 4, image,4*width);
        RGB_to_grayscale(image, height,  width);
         
        stbi_uc r, g, b;
        unsigned long int r_sum = 0, g_sum = 0, b_sum = 0;

        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
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
        sleep(10);
    }
}

// https://stackoverflow.com/questions/15686277/convert-rgb-to-grayscale-in-c#15686412
double sRGB_to_linear(double x)
{
    if (x < 0.04045)
        return x / 12.92;
    return pow((x + 0.055) / 1.055, 2.4);
}

void RGB_to_grayscale(stbi_uc *image,int height, int width)
{
    stbi_uc r, g, b;
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            get_pixel(image, width, x, y, &r, &g, &b);
            double R_linear = sRGB_to_linear(r / 255.0);
            double G_linear = sRGB_to_linear(g / 255.0);
            double B_linear = sRGB_to_linear(b / 255.0);
            double gray_linear = 0.2126 * R_linear + 0.7152 * G_linear + 0.0722 * B_linear;
            
             r = round(gray_linear*255);
             g = round(gray_linear*255);
             b = round(gray_linear*255);
            
            set_pixel(image, width,  x,  y,  r,  g,  b);

        }
    }
    stbi_write_png("test_img/img_out_gs.png", width,  height, 4, image,4*width);
}
