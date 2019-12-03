/*
 * fari.h
 * RS project
 *
 * Henry Lagarde & Louis Jacotot
 * 14/10/2019
 */

#ifndef _FARI_H
#define _FARI_H

#define FARI_PATH_MAX 4096 /* POSIX */

#include "lib/cJSON.h"
#include "json.h"
struct json;
struct CH {
    char** sources;
    char** headers;
};
struct fari {
    char *executable;
    char **flags;
    char **libs;
    char **sources;
    char **headers;
    int flags_count;
    int libs_count;
    int sources_count;
    int headers_count;
    int is_java; /* 0: C, else: Java */
    int continuation_on_error; /* 0: False, else: 1 */
};



struct fari *fari_create();
void fari_free(struct fari *fari);
/* counts bytes read */
int fari_read(const char *filename, char **buffer);
int fari_analyse(struct fari *fari, const char *buffer);
int fari_check(struct fari *fari);
int fari_compile(struct fari *fari, struct json *json);
void field_add(char ***field, int field_size, const char *str);

#endif /* _FARI_H */
