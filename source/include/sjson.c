#include "sjson.h"

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

struct SjsonNode{
    JsonType type;

    union {
        bool boolean;
        double number;
        char* string;

        struct {
            SjsonNode **elements;
            size_t count;
        }array;

        struct {
            SjsonPair *pairs;
            size_t count;
        }object;

    }value;
};

struct SjsonPair{
    char *key;
    SjsonNode *value;
};

static char* read_string(char **position);
static long read_number(char **position, bool *result);
static void free_json(SjsonNode *nood);
static void skip_spaces(char **c);

static char* read_string(char **position)
{
    skip_spaces(position);
    if(**position == '\"')
        (*position)++;

    char *start_position = *position;

    while(**position != '\0') {
        *position = strchr(*position, '\"');

        if(*(*position - 1) != '\\')
            break;
        else (*position)++;
    }

    ptrdiff_t length = *position - start_position;
    char *dst = malloc((int)length + 1);
    if(dst == nullptr) {
        fprintf(stderr, "Can't allocate memory for string");
        return nullptr;
    }

    strncpy(dst, start_position, (size_t)length);
    dst[length] = '\0';

    (*position)++; //skip '"' or ']'
    while(**position == ':' || **position == ',')
        (*position)++; //skip ':' or ','

    return dst;
};

static long read_number(char **position, bool *result)
{
    skip_spaces(position);
    char *start_position = *position;

    while((**position) && isdigit(**position))
            (*position)++;

    ptrdiff_t length = *position - start_position;

    char *string_number = malloc((int)length);
    if(string_number == nullptr) {
        fprintf(stderr, "Can't allocate memory for number");
        *result = false;

        return 0;
    }
    strncpy(string_number, start_position, (size_t)length);
    string_number[length - 1] = '\0';
   
    long number = atol(string_number); 
    free(string_number);
    
    while(**position == ':' && **position == ',')
        (*position)++; //skip ':' or ','

    *result = true;

    return number;
};

static void free_json(SjsonNode *node)
{
    if(node == nullptr)
        return;

    switch(node->type){
        case JSON_ARRAY:
            for(size_t i = 0; i != node->value.array.count; i++) {
                free_json(node->value.array.elements[i]);
            }

            free(node->value.array.elements);
            break;
    
        case JSON_STRING:
            free(node->value.string);
            break;

        case JSON_OBJECT:
            for(size_t i = 0; i != node->value.object.count; i++) {
                free(node->value.object.pairs[i].key);
                free_json(node->value.object.pairs[i].value);
            } 

            free(node->value.object.pairs);
            break;

        default:
            break;
    }

    free(node);
};


static void skip_spaces(char **c)
{
    while(isspace(**c) || **c == '\n')
        (*c)++;
};

void Sjson_free_root(SjsonNode *root)
{
    if(root)
        free_json(root);

    root = nullptr;
};

SjsonNode* Sjson_parse(char **text)
{
    char **c = text;
    skip_spaces(c);

    if(**c == '\0')
        return nullptr;
    
    SjsonNode *node = malloc(sizeof(SjsonNode));
    if(node == nullptr)
        return nullptr;

    if(**c == '[') {
        (*c)++;
        node->type = JSON_ARRAY;

        size_t i = 0;
        while(**c != ']') {
            node->value.array.elements[i] = Sjson_parse(c);
            i++;
        }
        node->value.array.count = i;

    }else if(**c == '{') {
        (*c)++;
        node->type = JSON_OBJECT;

        size_t i = 0;
        node->value.object.pairs = nullptr;

        while(**c != '}') {
            SjsonPair *pair = realloc(node->value.object.pairs, (i + 1) * sizeof(SjsonPair));
            if(pair == nullptr) {
                fprintf(stderr, "Can't realloc memory for SjsonPair");
                return nullptr;
            }

            node->value.object.pairs = pair;

            node->value.object.pairs[i] = (SjsonPair){};
            node->value.object.pairs[i].key = read_string(c); 
            node->value.object.pairs[i].value = Sjson_parse(c);
            i++;
        }

        node->value.object.count = i;

    }else if(**c == '\"' || isalpha(**c)) {
        node->type = JSON_STRING;
        node->value.string = read_string(c);
    }
    else if(**c == '-' || isdigit(**c)) {
        node->type = JSON_NUMBER;
        bool res = false;
        node->value.number = read_number(c, &res);
        if(res == false) {
            printf("Parsing error in read_number()");
            free(node);
            return nullptr;
        }
    }
    else if(**c == 't' || **c == 'f') {
        node->type = JSON_BOOL;

        if(**c == 't')
            node->value.boolean = true;
        else
            node->value.boolean = false;

        while(**c == ':' || **c == ',')
            (**c)++;
    }
    else if(**c == 'n') {
        if(strncmp(*c, "null", 4) == 0) {
            node->type = JSON_NULL;
            (*c) += 4;
        }
    }
    else {
        free(node);
        return nullptr;
    }

    return node;
};

SjsonNode* Sjson_get_index_array(SjsonNode *node, size_t index)
{
    if(node == nullptr || node->type != JSON_ARRAY) 
        return nullptr;
    if(index > node->value.array.count)
        return nullptr;

    return node->value.array.elements[index];
};

SjsonNode* Sjson_get_value(SjsonNode *node, const char *key)
{
    if(node == nullptr || node->type != JSON_OBJECT)
        return nullptr;

    for(size_t i = 0; i < node->value.object.count; i++)
        if(strcmp(key, node->value.object.pairs[i].key) == 0)
            return node->value.object.pairs[i].value; 

    return nullptr;
};

bool Sjson_get_bool_value(SjsonNode *node)
{
    return node->value.boolean;
};

char* Sjson_get_string_value(SjsonNode *node)
{
    return node->value.string;
};

double Sjson_get_number_value(SjsonNode *node)
{
    return node->value.number;
};
