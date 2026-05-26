#include "https_client.h" 
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <curl/curl.h>
#include "apikey.h"

typedef struct DownloadMemory {
    char *data;
    size_t size;
}DownloadMemory;

typedef struct HttpsClient{
    CURL *curl_handle;
    DownloadMemory chunk_data; 
}HttpsClient;

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userptr);

HttpsClient* https_init()
{
    HttpsClient *client = malloc(sizeof(HttpsClient));
    if(client == nullptr)
        return nullptr;

    curl_global_init(CURL_GLOBAL_ALL);
    client->curl_handle = curl_easy_init();
    if(client->curl_handle == nullptr) {
        https_cleanup(client);
        return nullptr;
    }

    client->chunk_data = (DownloadMemory){}; 

    return client;
};

void https_cleanup(HttpsClient *client)
{
    if(client->chunk_data.data){
        free(client->chunk_data.data);
        client->chunk_data.data = nullptr;
    }
    
    if(client->curl_handle)
        curl_easy_cleanup(client->curl_handle);

    if(client) {
        free(client);
        client = nullptr;
    }
 
    curl_global_cleanup();
};

char* https_get_nasa_apod(HttpsClient *client)
{
    char *api_key = apikey_get("NASA_API_KEY"); 
    if(api_key == nullptr){
        return nullptr;
    }

    const char *nasa_address = "https://api.nasa.gov/planetary/apod?api_key=";
    int address_size = strlen(nasa_address) + strlen(api_key) + 1;
    char *nasa_address_with_key = calloc(1, address_size);
    if(nasa_address_with_key == nullptr) {
        fprintf(stderr, "Can't allocate memory for NASA API KEY!");
        return nullptr;
    }
    strcat(nasa_address_with_key, nasa_address);
    strcat(nasa_address_with_key, api_key);

    client->chunk_data.data= malloc(1);
    client->chunk_data.size = 0;

    curl_easy_setopt(client->curl_handle, CURLOPT_URL, nasa_address_with_key);
    curl_easy_setopt(client->curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(client->curl_handle, CURLOPT_WRITEDATA, (void*)&client->chunk_data);
    curl_easy_setopt(client->curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

    free(nasa_address_with_key);
    free(api_key);
    nasa_address_with_key = nullptr;
    api_key = nullptr;

    printf("Connecting with NASA servers...\n");

    CURLcode res;
    res = curl_easy_perform(client->curl_handle);
    if(res != CURLE_OK) {
        fprintf(stderr, "Download error: %s\n", curl_easy_strerror(res));
        return nullptr;
    }    

    return client->chunk_data.data;
};


bool https_download_file(HttpsClient *client, const char *url, const char *filename)
{
    if(client == nullptr || client->curl_handle == nullptr || url == nullptr || filename == nullptr) 
        return false;

    FILE *file = fopen(filename, "wb");
    if(file == NULL) {
        fprintf(stderr,"Can't open file to read %s %s \n", filename, strerror(errno));

        return false;
    }

    //CURLOPT_URL is sets for download url
    curl_easy_setopt(client->curl_handle, CURLOPT_URL, url);
    
    //set callback to fwrite()
    curl_easy_setopt(client->curl_handle, CURLOPT_WRITEFUNCTION, fwrite);

    //user point to write data
    curl_easy_setopt(client->curl_handle, CURLOPT_WRITEDATA, file);

    //download file
    CURLcode res = curl_easy_perform(client->curl_handle);

    fclose(file);

    return res == CURLE_OK;
};

char* https_download_image(HttpsClient *client, const char *url_photo)
{
    char *file_extension = strrchr(url_photo, '.') + 1;
    if(file_extension == NULL) {
        printf("Today Apod isn't photo.\n");
        return nullptr;
    }
    
    if(strncmp(file_extension, "jpg", 3))
    {
        printf("Today Apod isn't photo.\n");
        return nullptr;
    }

    char *image_name = strrchr(url_photo, '/') + 1;
    const char *temp_path = getenv("TMPDIR");
    if(temp_path == NULL)
        temp_path = "/tmp/";

    size_t image_file_path_size = strlen(temp_path) + strlen(image_name) + 1;
    char *image_file_path = malloc(image_file_path_size);
    if(image_file_path == nullptr) {
        fprintf(stderr, "Can't allocate memory for image_file_path");
        return nullptr;
    }

    snprintf(image_file_path, image_file_path_size, "%s%s", temp_path, image_name);

    bool download_ret = https_download_file(client, url_photo, image_file_path);
    if(!download_ret) {
        printf("Can't download image.");
        free(image_file_path);

        return nullptr;
    }
 
    return image_file_path;
};

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userptr)
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
