#pragma once

typedef struct HttpsClient HttpsClient;

HttpsClient* https_init();
void https_cleanup(HttpsClient* client);

char* https_get_nasa_apod(HttpsClient *client);
