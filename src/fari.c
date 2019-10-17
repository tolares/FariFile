/*
 * fari.c
 * RS project
 *
 * Henry Lagarde & Louis Jacotot
 * 14/10/2019
 */

/*
gcc -std=c90 -pedantic-errors -Wall -Wextra -Wswitch-default -Wwrite-strings \
-pipe -Os -o main fari.c main.c

valgrind --tool=memcheck -s ./main resources/farifile
*/

/*
 * XXX: Questions
 * Peut-on écrire : `E "un programme en plusieurs mots"` ?
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fari.h"

struct fari *fari_create()
{
    struct fari *fari;

    fari = calloc(1, sizeof(*fari));
    return fari;
}
 
void fari_free(struct fari *fari)
{
    if (NULL == fari)
        return;
    free(fari->headers);
    free(fari->sources);
    free(fari->libs);
    free(fari->flags);
    free(fari->executable);
    free(fari);
}

int fari_read(const char *filename, char **buffer)
{
    int ret;
    FILE *file;
    long file_len;

    ret = 0;

    file = fopen(filename, "r");
    if (NULL == file) {
        ret = -1;
        fprintf(stderr, "fari filename does not exist\n");
        goto exit;
    }

    fseek(file, 0, SEEK_END);
    file_len = ftell(file);
    fseek(file, 0, SEEK_SET);

    *buffer = malloc((file_len + 1) * sizeof(char));
    if (NULL == *buffer) {
        ret = -1;
        fprintf(stderr, "memory allocation failed\n");
        goto close_file;
    }
    (*buffer)[file_len] = '\0';

    if (file_len != (long)fread(*buffer, 1, file_len, file)) {
        ret = -1;
        fprintf(stderr, "fari file reading failed\n");
        goto free_all;
    }

    /* check for overflow */
    if (file_len > INT_MAX) {
        ret = -1;
        fprintf(stderr, "fari file too long (max %d bytes)\n", INT_MAX);
        goto free_all;
    }
    ret = (int)file_len;
    goto close_file;

free_all:
    free(*buffer);
close_file:
    fclose(file);
exit:
    return ret;
}
