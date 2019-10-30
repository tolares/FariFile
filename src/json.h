/*
 * Created by Lagarde Henry & Louis Jacotot on 30/10/2019.
*/

#ifndef _JSON_H
#define _JSON_H


#include "fari.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fari.h"
#include "lib/cJSON.h"

struct json {
    cJSON *root;
};

struct json *json_create();
void json_free(struct json *json);
void json_fill(struct json *json, struct fari *fari, char* message);
#endif
