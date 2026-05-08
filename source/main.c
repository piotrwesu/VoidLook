#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

typedef struct DownloadMemory {
    char *data;
    size_t size;
}DownloadMemory;

size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userptr);

int main(void)
{
    curl_global_init(CURL_GLOBAL_ALL);
    CURL *curl_handle = curl_easy_init();
    CURLcode res;

    DownloadMemory chunk;
    chunk.data= malloc(1);
    chunk.size = 0;

    int error_code = 0;

    if(curl_handle == nullptr){
        error_code = 1;
        goto cleanup;
    }

    curl_easy_setopt(curl_handle, CURLOPT_URL, "https://api.nasa.gov/planetary/apod?api_key=DEMO_KEY");
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&chunk);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    printf("Connecting with NASA servers...\n");

    res = curl_easy_perform(curl_handle);
    if(res != CURLE_OK) {
        fprintf(stderr, "Download error: %s\n", curl_easy_strerror(res));
    }else {
        printf("%s\n", chunk.data);
    }

    curl_easy_cleanup(curl_handle);

cleanup:
    free(chunk.data);
    curl_global_cleanup();

    return error_code;
}

size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userptr)
{
    size_t real_size = size * nmemb;
    DownloadMemory *mem = (DownloadMemory*)userptr;

    char *ptr = realloc(mem->data, mem->size + real_size + 1);
    if(ptr == nullptr){
        fprintf(stderr, "Can't ralloc memory for DownloadMemory!\n");
        return 0;
    }

    mem->data = ptr;
    memcpy(&(mem->data[mem->size]), contents, real_size);
    mem->size += real_size;
    mem->data[mem->size] = '\0';

    return real_size;
};
