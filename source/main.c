#include "https_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sjson.h"
#include "image.h"

int main(int argc, char **argv)
{
    for(int i = 1; i < argc; i++) {
        if ((strcmp(argv[i], "--version") == 0) || (strcmp(argv[i], "-v") == 0)) {
            printf("VoidLook version 0.0.9\n");
            printf("https://github.com/piotrwesu/VoidLook\n");

            return 0;
        }
    }

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
    
    char* image_file_path = https_download_image(client, url_photo);
    if(image_file_path == nullptr) {
        error_code = 1;
        goto clean_extension;
    }
       
    if(image_display_image_in_sixel(image_file_path) < 0)
        error_code = 1;

    free(image_file_path);

clean_extension:
    Sjson_free_root(root);    

clean_client:
    https_cleanup(client);

    return error_code;
}
