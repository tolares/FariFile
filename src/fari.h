#ifndef _FARI_H
#define _FARI_H

/*
 * S'il y a plusieurs lignes F et B, on concatène ?
 * Passe-t-on les headers à gcc ou juste pour vérifier la date ?
 * S'il y a plusieurs fichiers .c sur la même ligne C, doit-on produire
 * plusieurs .o ou un seul ?
 * Un fichier header x.h doit-il s'appliquer uniquement à x.c ou à tous les
 * autres fichiers .c ?
 * Comparer la date du .h à quel fichier .o ? le plus ancien ? le plus récent ?
 */

struct fari {
        unsigned int nb_sources;
        unsigned int nb_headers;
        char *executable;
        char *flags;
        char *libs;
        char **sources;
        char **headers;
};

/*
 * Returns the number of bytes read in buffer from filename
 * Returns < 0 if error
 */
int fari_read(const char *filename, char *buffer);

struct fari *fari_parse(const char *buffer);

int fari_compile(const struct fari *fari);

void fari_free(struct fari *fari);

#endif /* _FARI_H */
