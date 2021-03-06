/*
 *  Created by Henry LAGARDE & Louis JACOTOT
 */

#include "json.h"
json_t *json_create()
{
    json_t *json;
    json = calloc(1, sizeof(*json));
    json->root = cJSON_CreateObject();
    json->linking_errors = malloc(65536* sizeof(char));
    json->compilation_errors = malloc(65536* sizeof(char));
    return json;
}

void json_free(json_t *json)
{

    if (NULL == json)
        return;

    if (NULL != json->root) {
        cJSON_Delete(json->root);
    }
    free(json);
}

void json_fill(json_t *json, struct fari *fari, char *message){
    char *out;
    cJSON *compilation;
    cJSON *linking;
    FILE *stream;
    char * str = malloc(1* sizeof(char));
    stream= fopen("logs.json", "w");

    compilation = cJSON_CreateObject();
    linking = cJSON_CreateObject();
    cJSON_AddItemToObject(json->root, "sources", cJSON_CreateStringArray((const char **) fari->sources, fari->sources_count));
    cJSON_AddItemToObject(json->root, "headers", cJSON_CreateStringArray((const char **) fari->headers, fari->headers_count));
    cJSON_AddItemToObject(json->root, "librairies", cJSON_CreateStringArray((const char **) fari->libs, fari->libs_count));
    cJSON_AddItemToObject(json->root, "flags", cJSON_CreateStringArray((const char **) fari->flags, fari->flags_count));
    cJSON_AddItemToObject(json->root, "executable_name", cJSON_CreateString(fari->executable));
    cJSON_AddItemToObject(json->root, "fari_error_msg", cJSON_CreateString(" "));
    if(json->commands_number == 0){
        cJSON_AddItemToObject(linking, "commands",cJSON_CreateString(" "));
    }else{
        cJSON_AddItemToObject(linking, "commands",cJSON_CreateString((const char *) json->commands[0]));
    }
    if(json->compilation_numbers == 0){
        cJSON_AddItemToObject(compilation, "commands",cJSON_CreateString(" "));
    }else{
        cJSON_AddItemToObject(compilation, "commands",cJSON_CreateStringArray((const char **) json->commands_compilation, json->compilation_numbers));
    }
    cJSON_AddItemToObject(compilation, "error_msg", cJSON_CreateString(json->compilation_errors));
    cJSON_AddItemToObject(linking, "error_msg", cJSON_CreateString(json->linking_errors));
    cJSON_AddItemToObject(json->root, "compilation", compilation);
    cJSON_AddItemToObject(json->root, "linking", linking);
    cJSON_AddItemToObject(json->root, "fari_msg", cJSON_CreateString(message));

    out = cJSON_PrintUnformatted(json->root);
    fprintf(stream,"%s", out);
    fclose(stream);
}
