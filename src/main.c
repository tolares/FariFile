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

    ret = 0;

    if (3 <= argc) {
        ret = 1;
        fprintf(stderr, "usage: %s [fari file]\n", argv[0]);
        goto exit;
    }

    if (2 == argc)
        filename = argv[1];

    fari = fari_create();
    if (NULL == fari) {
        ret = 1;
        fprintf(stderr, "memory allocation failed\n");
        goto exit;
    }

    if (0 >= fari_read(filename, &buffer)) {
        ret = 1;
        fprintf(stderr, "\t-> fari file reading failed\n");
        goto free_fari;
    }

    if (fari_analyse(fari, buffer)) {
        ret = 1;
        fprintf(stderr, "\t-> fari analysis failed\n");
        goto free_all;
    }

    if (fari_check(fari)) {
        ret = 1;
        fprintf(stderr, "\t-> fari checking failed\n");
        goto free_all;
    }

    if (fari_compile(fari)) {
        ret = 1;
        fprintf(stderr, "\t-> fari compilation failed\n");
    }

    printf("Compilation terminée avec succès !\n");

free_all:
    free(buffer);
free_fari:
    fari_free(fari);
exit:
    if (ret)
            fprintf(stderr, "\033[01;31m-> something went wrong\n\033[0m");
    return ret;
}
