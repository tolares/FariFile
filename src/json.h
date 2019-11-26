/*
 * Created by Lagarde Henry & Louis Jacotot on 30/10/2019.
*/

#ifndef _JSON_H
#define _JSON_H


#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fari.h"
#include "lib/cJSON.h"
struct fari;
typedef struct json {
    cJSON *root;
    char** commands;
    char* linking_errors;
    char ** commands_compilation;
    int compilation_numbers;
    char* compilation_errors;
    int commands_number;
} json_t;

json_t *json_create();
void json_free(json_t *json);
void json_fill(json_t *json, struct fari *fari, char *message);
#endif
