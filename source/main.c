#include "https_client.h"
#include <stdio.h>

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

cleanup:
    https_cleanup(client);

    return error_code;
}
