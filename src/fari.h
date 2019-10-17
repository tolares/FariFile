/*
 * fari.h
 * RS project
 *
 * Henry Lagarde & Louis Jacotot
 * 14/10/2019
 */

#ifndef _FARI_H
#define _FARI_H

#define FARI_PATH_MAX 4096 /* TODO: POSIX ? */

struct fari {
    char *executable;
    char *flags;
    char *libs;

    int sources_count;
    int headers_count;
    char **sources;
    char **headers;
};

struct fari *fari_create();
void fari_free(struct fari *fari);
/* counts bytes read */
int fari_read(const char *filename, char **buffer);
int fari_analyse(struct fari *fari, const char *buffer);

#endif /* _FARI_H */
