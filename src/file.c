#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "file.h"

int file_exists(const char *filename)
{
    return !access(filename, F_OK);
}

int file_has_extension(const char *filename, const char *extension)
{
    int filename_len;
    int extension_len;

    /* TODO: strnlen ? strncmp ? */
    filename_len = strlen(filename);
    extension_len = strlen(extension);
    if (filename_len < (extension_len + 1))
        return 0;

    return ((filename[filename_len - extension_len - 1] == '.') &&
            !strcmp(&filename[filename_len - extension_len], extension));
}

char *file_change_extension(const char *filename, const char *extension)
{
    int i;
    int filename_len;
    int extension_len;
    char *mfilename;
    int j;

    filename_len = strlen(filename);
    extension_len = strlen(extension);
    i = filename_len - 1;
    while ((i >= 0) && ('.' != filename[i--]));
    ++i;
    if (filename[i] != '.')
        return NULL;
    ++i;

    mfilename = malloc((i + extension_len + 1) * sizeof(*mfilename));
    if (!mfilename)
        return NULL;
    for (j = 0; j < i; ++j) {
        mfilename[j] = filename[j];
    }
    strcpy(&mfilename[i], extension);
    return mfilename;
}

int file_newer_than(const char *filename_a, const char *filename_b)
{
    struct stat statbuf;
    time_t time_a, time_b;

    stat(filename_a, &statbuf);
    time_a = statbuf.st_mtime;
    stat(filename_b, &statbuf);
    time_b = statbuf.st_mtime;

    return time_a > time_b;
}
