#include "apikey.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stddef.h>

typedef struct FileBuffer {
    char *data;
    long size;
}FileBuffer; 

static int file_read(const char *filename, FileBuffer *buffer);
static void file_free_buffer(FileBuffer *buffer);

static int file_read(const char *filename, FileBuffer *buffer)
{
    int ERROR_STATUS = 0;

    FILE *file = fopen(filename, "rb");
    if(file == NULL) {
        fprintf(stderr,"Can't open file to read %s %s \n", filename, strerror(errno));

        return -1;
    }

    if(fseek(file, 0, SEEK_END) != 0) {
        fprintf(stderr, "Can't find end of file %s %s\n", filename, strerror(errno));

        ERROR_STATUS = -1;
        goto close_file;
    }

    long size = ftell(file);
    if(size == -1L) {
        perror("ftell in library");

        ERROR_STATUS = -1;
        goto close_file;
    }
    rewind(file);

    buffer->size = size + 1;
    buffer->data = malloc(buffer->size);
    if(buffer->data == NULL){
        fprintf(stderr, "Allocate memory failed \n");
        perror(NULL);

        ERROR_STATUS = -1;
        goto close_file;
    }

    if(fread(buffer->data, size, 1, file) != 1) {
        fprintf(stderr, "Cant't read library file %s \n", filename);

        ERROR_STATUS = -1;
        goto close_file;
    }
    buffer->data[size] = '\0';

    close_file:
    if(fclose(file) == EOF) {
        fprintf(stderr, "Can't close file ! \n");
        perror(NULL);
        return 1;
    }

    return ERROR_STATUS;
};


static void file_free_buffer(FileBuffer *buffer)
{
    free(buffer->data);
    buffer->data = nullptr;
};

char* apikey_get(const char *key_type)
{
    char *key_api = nullptr;
    FileBuffer fbuffer;
    if(file_read(".env", &fbuffer) < 0) {
        puts("Can't read .env file with API KEY!");
        return nullptr;
    }

    char *found_type = strstr(fbuffer.data, key_type);
    if(found_type == NULL) {
        fprintf(stderr, "Can't find %s in .env file\n", key_type);
        goto cleanup; 
    }

    char *start = strchr(fbuffer.data, '"');
    start++;
    while(isspace(*start))
        (*start)++;

    char *end = strchr(start, '"');

    ptrdiff_t size_key_api = end - start + 1; 

    key_api = malloc((int)size_key_api);
    if(key_api == nullptr) {
        fprintf(stderr, "Can't allocate memory for API KEY!\n");
        goto cleanup;
    }

    strncpy(key_api, start, size_key_api);
    key_api[size_key_api - 1] = '\0';
    
cleanup:
    file_free_buffer(&fbuffer);

    return key_api;
};
