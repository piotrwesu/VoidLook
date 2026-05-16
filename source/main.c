#include "https_client.h"
#include <stdio.h>
#include "sjson.h"

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
        goto cleanup;
    }

    printf("%s\n", json);

    SjsonNode *root = Sjson_parse(&json);
    if(root == nullptr){
        puts("Parsing error.\n");
        goto cleanup;
    }

    SjsonNode *node = Sjson_get_index_array(root, 0);
    if(node == nullptr)
        node = root;

    SjsonNode *photo_node = Sjson_get_value(node, "url");
    const char *url_photo = Sjson_get_string_value(photo_node);
    printf("Url: %s\n", url_photo);
    
    SjsonNode *explanation_node = Sjson_get_value(node, "explanation");
    const char *explanation = Sjson_get_string_value(explanation_node);
    printf("Explanation: %s", explanation);

    bool ret = https_download_file(client, url_photo, "/tmp/apod.jpg");
    if(!ret)
        printf("Can't download image.");

    Sjson_free_root(root);    

cleanup:
    https_cleanup(client);

    return error_code;
}
