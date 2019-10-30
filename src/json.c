/*
 *  Created by Henry LAGARDE & Louis JACOTOT
 */

#include "json.h"
struct json *json_create()
{
    struct json *json;

    json = calloc(1, sizeof(*json));
    return json;
}

void json_free(struct json *json)
{

    if (NULL == json)
        return;

    if (NULL != json->root) {
        cJSON_Delete(json->root);
    }
    free(json);
}

void json_fill(struct json *json, struct fari *fari, char* message){
    cJSON *compilation;
    char *out;
    FILE *stream = fopen("test.json", "w");
    int i;
    json->root = cJSON_CreateObject();
    compilation = cJSON_CreateObject();
    cJSON_AddItemToObject(json->root, "source", cJSON_CreateStringArray(fari->sources, fari->sources_count));
    cJSON_AddItemToObject(json->root, "headers", cJSON_CreateStringArray(fari->sources, fari->headers_count));
    cJSON_AddItemToObject(json->root, "librairies", cJSON_CreateStringArray(fari->libs, fari->libs_count));
    cJSON_AddItemToObject(json->root, "executable_name", cJSON_CreateString(fari->executable));
    cJSON_AddItemToObject(json->root, "fari_error_msg", cJSON_CreateString("test"));
    cJSON_AddItemToObject(compilation, "commands", cJSON_CreateStringArray(fari->libs, fari->libs_count));  /*TODO: Get all commands and replace in the code */
    cJSON_AddItemToObject(compilation, "error_msg", cJSON_CreateString("test"));
    cJSON_AddItemToObject(json->root, "compilation", compilation);
    cJSON_AddItemToObject(json->root, "fari_msg", cJSON_CreateString(message));

    out = cJSON_PrintUnformatted(json->root);
    json_free(json);
    fprintf(stream,"%s", out);
    fclose(stream);
}