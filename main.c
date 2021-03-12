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
#include <libgen.h>

#include "uini.h"
#include "log.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


// Constants
#define CONFIG_FILE "./server.conf"
#define PATH_MAX_STRING_SIZE 512
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
void equalization(stbi_uc *image,int height, int width);
int mkdir_p(const char *dir, const mode_t mode);

// Program running variable
bool running = true;
struct sigaction action;
// Handle stop
void handle_stop(int signum)
{
    log_info("Exiting from ImageServer...");
    running = false;
    exit(0);
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
    if (!f)
    {
        log_fatal("Error: Config file not found");
        return EXIT_FAILURE;
    }

    uini_parse(f, conf_handler, &params);
    fclose(f);

    // Setup logging
    FILE *log_file = fopen(params.log_file, "a");
    log_add_fp(log_file, 0);

    // Debug
    mkdir_p(params.save_dir, 0755);
    log_info("save_dir is: %s", params.save_dir);
    mkdir_p(params.colors_dir, 0755);
    log_info("colors_dir is: %s", params.colors_dir);
    mkdir_p(params.histo_dir, 0755);
    log_info("histo_dir is: %s", params.histo_dir);
    log_info("port is: %i", params.port);

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

    log_debug("PORT FROM SERVER THREAD: %i", params->port);
    log_debug("LOGFILE FROM SERVER THREAD: %s", params->log_file);

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
        log_info("+++++++ Waiting for new connection ++++++++");
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
        log_info("------------------Hello message sent-------------------");
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
    char *current_file = malloc(sizeof(char) * PATH_MAX_STRING_SIZE);
    d = opendir(directory);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (dir->d_type == DT_DIR)
            {
                continue;
            }
            char path[PATH_MAX_STRING_SIZE];
            strcpy(path, directory);
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
    if (max_size == 0)
        return "\0";
    return current_file;
}

char *concat_path(char *color, char *basepath)
{
    char *output = malloc(sizeof(char) * PATH_MAX_STRING_SIZE);
    strcpy(output, basepath);
    strcat(output, color);
    return output;
}

char *output_path_builder(char *filename, char *color, char *basepath)
{
    char *output = malloc(sizeof(char) * PATH_MAX_STRING_SIZE);
    strcpy(output, basepath);
    strcat(output, color);
    strcat(output, "/");
    strcat(output, filename);
    strcat(output, ".png");
    return output;
}

char *histo_output_path_builder(char *filename, char *basepath)
{
    char *output = malloc(sizeof(char) * PATH_MAX_STRING_SIZE);
    strcpy(output, basepath);
    strcat(output, filename);
    strcat(output, ".png");
    return output;
}

/* recursive mkdir */
/* FROM: https://gist.github.com/ChisholmKyle/0cbedcd3e64132243a39 */
int mkdir_p(const char *dir, const mode_t mode)
{
    char tmp[PATH_MAX_STRING_SIZE];
    char *p = NULL;
    struct stat sb;
    size_t len;

    /* copy path */
    len = strnlen(dir, PATH_MAX_STRING_SIZE);
    if (len == 0 || len == PATH_MAX_STRING_SIZE)
    {
        return -1;
    }
    memcpy(tmp, dir, len);
    tmp[len] = '\0';

    /* remove trailing slash */
    if (tmp[len - 1] == '/')
    {
        tmp[len - 1] = '\0';
    }

    /* check if path exists and is a directory */
    if (stat(tmp, &sb) == 0)
    {
        if (S_ISDIR(sb.st_mode))
        {
            return 0;
        }
    }

    /* recursive mkdir */
    for (p = tmp + 1; *p; p++)
    {
        if (*p == '/')
        {
            *p = 0;
            /* test path */
            if (stat(tmp, &sb) != 0)
            {
                /* path does not exist - create directory */
                if (mkdir(tmp, mode) < 0)
                {
                    return -1;
                }
            }
            else if (!S_ISDIR(sb.st_mode))
            {
                /* not a directory */
                return -1;
            }
            *p = '/';
        }
    }
    /* test path */
    if (stat(tmp, &sb) != 0)
    {
        /* path does not exist - create directory */
        if (mkdir(tmp, mode) < 0)
        {
            return -1;
        }
    }
    else if (!S_ISDIR(sb.st_mode))
    {
        /* not a directory */
        return -1;
    }
    return 0;
}

char *filename_without_extension(char *file_name)
{
    char *remaining_str;
    char *last_ext;
    if (file_name == NULL)
        return NULL;
    if ((remaining_str = malloc(strlen(file_name) + 1)) == NULL)
        return NULL;
    strcpy(remaining_str, basename(file_name));
    last_ext = strrchr(remaining_str, '.');
    if (last_ext != NULL)
        *last_ext = '\0';
    return remaining_str;
}

void *image_main(void *context)
{
    ConfParams *params = context;

    log_debug("PORT FROM IMG THREAD: %i", params->port);
    log_debug("LOGFILE FROM IMG THREAD: %s", params->log_file);

    // Create folders if they don't exist
    mkdir_p(concat_path("r", params->colors_dir), 0755);
    mkdir_p(concat_path("g", params->colors_dir), 0755);
    mkdir_p(concat_path("b", params->colors_dir), 0755);

    while (running)
    {
        char *smallest_file = get_smallest_file(params->save_dir);
        if (smallest_file[0] == '\0')
        {
            log_info("No file found in save_dir, skipping");
            sleep(5);
            continue;
        }
        log_info("Smallest file: %s\n", get_smallest_file(params->save_dir));
        int width, height; // image width, heigth,
        stbi_uc *image = stbi_load(get_smallest_file(params->save_dir), &width, &height, NULL, 4);
        log_debug("w: %i", width);
        log_debug("h: %i", height);

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
        log_debug("r: %li", r_sum);
        log_debug("g: %li", g_sum);
        log_debug("b: %li", b_sum);
        log_debug("output colors: %s", output_path_builder(filename_without_extension(smallest_file), "b", params->colors_dir));

        // Case Red
        if (r_sum > g_sum && r_sum > b_sum)
        {
            log_info("RED!!");
            stbi_write_png(output_path_builder(filename_without_extension(smallest_file), "r", params->colors_dir), width, height, 4, image, 4 * width);
        }
        // Case Green
        else if (g_sum > r_sum && g_sum > b_sum)
        {
            log_info("GREEN!!");
            stbi_write_png(output_path_builder(filename_without_extension(smallest_file), "g", params->colors_dir), width, height, 4, image, 4 * width);
        }
        // Case Blue
        else
        {
            log_info("BLUE!!");
            stbi_write_png(output_path_builder(filename_without_extension(smallest_file), "b", params->colors_dir), width, height, 4, image, 4 * width);
        }

        equalization(image, height, width);
        log_debug("Equalization done");
        log_debug("output eq: %s", histo_output_path_builder(filename_without_extension(smallest_file), params->histo_dir));
        stbi_write_png(histo_output_path_builder(filename_without_extension(smallest_file), params->histo_dir), width, height, 4, image, 4 * width);

        remove(smallest_file);
        sleep(1);
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
}

void equalization(stbi_uc *image,int height, int width){
    int fi[256]={0},cuf[256]={0}, cufeq[256]={0}, i_out[256]={0};
    stbi_uc r, g, b;
    RGB_to_grayscale(image, height,  width);
    
    for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            get_pixel(image, width, x, y, &r, &g, &b);
            fi[r]=fi[r]+1;
//             set_pixel(image, width,  x,  y,  r,  g,  b);
        }
    }
    
    int sum=0;
    int i;
   for( i = 0; i < 256; i = i + 1 ){
       sum= sum+ fi[i];
       cuf[i]=sum;
   }
   int mean = sum>>8;
   int cuf_sum=0;
   if((sum&255)!=0){
       cuf_sum=1;
    }
    for( i = 0; i < 256; i = i + 1 ){
       cuf_sum= mean + cuf_sum;
       cufeq[i]=cuf_sum;
   }
   
   for( i = 0; i < 256; i = i + 1 ){
        int closest=0;
        if(cuf[i]<=cufeq[0]){
           i_out[i]=0;
           continue;
        }
        int j;
        for( j = 1; j < 256; j = j + 1 ){
            if(cuf[i]==cufeq[j]){
                i_out[i]=j; 
                break;
            }
            if(cuf[i]<cufeq[j]){
                if( (cuf[i]-cufeq[j])  >  (cuf[i]<cufeq[j-1])    ){
                    i_out[i]=j-1; 
                }
                else{
                    i_out[i]=j;
                }
                break;
            }
            if(j==255){
                i_out[i]=255;
            }
        }
   }
   for (int x = 0; x < width; x++)
    {
        for (int y = 0; y < height; y++)
        {
            get_pixel(image, width, x, y, &r, &g, &b);
            set_pixel(image, width,  x,  y,  i_out[r],   i_out[r],   i_out[r]);
        }
    }
}
