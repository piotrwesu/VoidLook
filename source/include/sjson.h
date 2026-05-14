#pragma once

#include <stddef.h>

typedef enum {
    JSON_NULL,
    JSON_BOOL,
    JSON_NUMBER,
    JSON_STRING,
    JSON_ARRAY,
    JSON_OBJECT
}JsonType;

typedef struct SjsonNode SjsonNode;
typedef struct SjsonPair SjsonPair;

void Sjson_free_root(SjsonNode *root);
SjsonNode* Sjson_parse(char **text); //remember use Sjson_free_root after Sjson_parse
