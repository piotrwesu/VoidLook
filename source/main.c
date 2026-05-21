#include "https_client.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sjson.h"
#include "image.h"

int main(void)
{
    int error_code = 0;

    HttpsClient *client = https_init();
    if(client == nullptr) {
        puts("Can't init HttpsClient.");
        return 1;
    }
    
    char *json = https_get_nasa_apod(client);
    if(json == nullptr) {
        puts("Can't get json file from NASA APOD.");
        error_code = 1;
        goto clean_client;
    }

    //printf("%s\n", json);

    SjsonNode *root = Sjson_parse(&json);
    if(root == nullptr){
        puts("Parsing error.\n");
        goto clean_client;
    }

    SjsonNode *node = Sjson_get_index_array(root, 0);
    if(node == nullptr)
        node = root;

    SjsonNode *photo_node = Sjson_get_value(node, "url");
    const char *url_photo = Sjson_get_string_value(photo_node);
    printf("Url: %s\n", url_photo);
    
    SjsonNode *explanation_node = Sjson_get_value(node, "explanation");
    const char *explanation = Sjson_get_string_value(explanation_node);
    printf("Explanation: %s\n", explanation);
    
    char *image_name = strrchr(url_photo, '/') + 1;
    size_t image_name_size = strlen(image_name);
    const char temp_path[] = "/tmp/";
    const int temp_path_size = strlen(temp_path);

    char *image_file_path = malloc(temp_path_size + image_name_size + 1);
    if(image_file_path == nullptr) {
        fprintf(stderr, "Can't allocate memory for image_file_path");
        error_code = 1;
        goto cleanup;
    }

    memcpy(image_file_path , temp_path, temp_path_size);
    memcpy(image_file_path + temp_path_size, image_name, image_name_size);
    image_file_path[temp_path_size + image_name_size] = '\0';

    bool download_ret = https_download_file(client, url_photo, image_file_path);
    if(!download_ret) {
        printf("Can't download image.");
        error_code = 1;
        goto cleanup;
    }

    if(image_display_image_in_sizel(image_file_path) < 0)
        error_code = 1;

cleanup:
    free(image_file_path);

    Sjson_free_root(root);    

clean_client:
    https_cleanup(client);

    return error_code;
}
