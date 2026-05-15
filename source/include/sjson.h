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

SjsonNode* Sjson_parse(char **text); //remember use Sjson_free_root after Sjson_parse
void Sjson_free_root(SjsonNode *root);
SjsonNode* Sjson_get_index_array(SjsonNode *node, size_t index);
SjsonNode* Sjson_get_value(SjsonNode *node, const char *key);
bool Sjson_get_bool_value(SjsonNode *node);
char* Sjson_get_string_value(SjsonNode *node);
double Sjson_get_number_value(SjsonNode *node);
