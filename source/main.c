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

    SjsonNode *url_photo = Sjson_get_value(node, "url");
    printf("Url: %s\n", Sjson_get_string_value(url_photo));
    
    node = Sjson_get_value(node, "explanation");
    printf("Explanation: %s", Sjson_get_string_value(node));

    Sjson_free_root(root);    

cleanup:
    https_cleanup(client);

    return error_code;
}
