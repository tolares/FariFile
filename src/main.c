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
    struct fari *fari;
    char *buffer;

    ret = 0;

    if (2 != argc) {
        ret = 1;
        fprintf(stderr, "usage: %s [fari file]\n", argv[0]);
        goto exit;
    }

    fari = fari_create();
    if (NULL == fari) {
        ret = 1;
        fprintf(stderr, "memory allocation failed\n");
        goto exit;
    }

    if (0 >= fari_read(argv[1], &buffer)) {
        ret = 1;
        fprintf(stderr, "\t-> fari file reading failed\n");
        goto free_fari;
    }

    if (fari_analyse(fari, buffer)) {
        ret = 1;
        fprintf(stderr, "\t-> fari file analysis failed\n");
        goto free_all;
    }

    /* XXX: debug */
    printf("executable : \"%s\"\n", fari->executable);
    /* XXX: end of debug */

free_all:
    free(buffer);
free_fari:
    fari_free(fari);
exit:
    if (ret)
            fprintf(stderr, "\033[01;31m-> something went wrong\n\033[0m");
    return ret;
}
