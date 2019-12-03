/*
 * main.c
 * RS project
 *
 * Henry Lagarde & Louis Jacotot
 * 14/10/2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fari.h"

int main(int argc, char **argv)
{
    int ret;
    const char *filename = "farifile";
    struct fari *fari;
    char *buffer;
    int continuation;

    struct json *json;
    json = json_create();

    ret = 0;

    if (4 <= argc) {
        ret = 1;
        fprintf(stderr, "usage: %s [fari file]\n", argv[0]);
        goto exit;
    }

    if (3 <= argc){
        filename = argv[1];
        continuation =0;
        if(2 == argc){
            if(strcmp(argv[2],"k")){
                continuation = 1;
            }else{
                ret = 1;
                fprintf(stderr, "usage: %s [fari file] [options: k]\n", argv[0]);
                goto exit;
            }
        }
    }
    fari = fari_create();
    fari->continuation_on_error = continuation;
    if (NULL == fari) {
        ret = 1;
        fprintf(stderr, "memory allocation failed\n");
        goto exit;
    }

    if (0 >= fari_read(filename, &buffer)) {
        ret = 1;
        fprintf(stderr, "\t-> fari file reading failed\n");
        json_fill(json, fari, "-> fari reading failed");
        goto free_fari;
    }

    if (fari_analyse(fari, buffer)) {
        ret = 1;
        fprintf(stderr, "\t-> fari analysis failed\n");
        json_fill(json, fari, "-> fari analysis failed");
        goto free_all;
    }

    if (fari_check(fari)) {
        ret = 1;
        fprintf(stderr, "\t-> fari checking failed\n");
        json_fill(json, fari, "-> fari checking failed");
        goto free_all;
    }
    if (fari_compile(fari, json) && fari->continuation_on_error) {
        ret = 1;
        json_fill(json, fari, "-> fari compilation failed");
        fprintf(stderr, "\t-> fari compilation failed\n");
    }

    json_fill(json, fari, "Compilation terminée avec succès !");
    printf("Compilation terminée avec succès !\n");

free_all:
    free(buffer);
free_fari:
    fari_free(fari);
    json_free(json);
exit:
    if (ret)
            fprintf(stderr, "\033[01;31m-> something went wrong\n\033[0m");
    return ret;
}
