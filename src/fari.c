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

static int read_token(char *c, const char *buffer, int *pos)
{
    while ((*c = buffer[(*pos)++])) {
        switch (*c) {
        case '\x0a':                    /* line feed */
        case '\x0d':    break;          /* carriage return */
        case '#':
        case 'B':
        case 'C':
        case 'E':
        case 'F':
        case 'H':       return 0; break;
        default:        return 1; break;
        }
    }
    return 1;
}

static void read_str(char *str, int maxlen, const char *buffer, int *pos)
{
    char c;
    int i;

    i = 0;
    while (((i + 1) < maxlen) && (c = buffer[(*pos)++])) {
        switch (c) {
        case '\x09':                    /* vertical tab */
        case '\x20':                    /* space */
        case '\x0a':                    /* line feed */
        case '\x0d':                    /* carriage return */
            (*pos)--; /* should not have read this, go back */
            str[i] = '\0';
            return;
            break;
        default:
            str[i++] = c;
            break;
        }
    }
    (*pos)--;
    str[i] = '\0';
}

/* skips all characters until new line */
static void read_until_crlf(const char *buffer, int *pos)
{
    char c;

    while ((c = buffer[(*pos)++])) {
        switch (c) {
        case '\x0a':                    /* line feed */
        case '\x0d':    return; break;  /* carriage return */
        default:        break;
        }
    }
}

/* skips all spaces */
static void read_spaces(const char *buffer, int *pos)
{
    while ('\x20' == buffer[(*pos)++]); /* space */
    (*pos)--; /* go back */
}

static void field_concat(char **field, const char *str)
{
    int len_field;
    int len_str;

    len_str = strlen(str);      /* TODO: strnlen ? */

    if (NULL == *field) {
        *field = malloc((len_str + 1) * sizeof(char));
        strcpy(*field, str);
    } else {
        len_field = strlen(*field); /* TODO: strnlen ? */
        *field = realloc(*field, len_field + len_str + 2); /* '\x0a' + '\0' */
        (*field)[len_field] = '\x20'; /* space separator */
        strcpy((*field) + len_field + 1, str); /* TODO: strncpy ? */
    }
}

int fari_analyse(struct fari *fari, const char *buffer)
{
    int seen_executable;
    int line, line_pos;
    int pos;
    char c;
    char str[FARI_PATH_MAX];

    seen_executable = 0;
    line = 1;
    line_pos = 0;
    pos = 0;

    while (!read_token(&c, buffer, &pos)) {
        switch (c) {
            case 'B': break;
            case 'C': break;
            case 'E':
                if (seen_executable) {
                    fprintf(stderr, "fari file contains 'E' twice\n");
                    return 1;
                }
                seen_executable = 1;
                read_spaces(buffer, &pos);
                read_str(&str[0], sizeof(str)/sizeof(char), buffer, &pos);
                if ('\0' == str[0]) {
                    fprintf(stderr, "fari file syntax error\n");
                    fprintf(stderr, "\t-> argument expected for token 'E'\n");
                    return 1;
                }
                field_concat(&fari->executable, str);
                break;
            case 'F': break;
            case 'H': break;
            default: break;
        }
        read_until_crlf(buffer, &pos);
        line++;
        line_pos = pos;
    }

    if (c) {
        fprintf(stderr, "fari file syntax error\n");
        if ('\x09' == c || '\x20' == c) { /* vertical tab || space */
                fprintf(stderr, "\t-> bad indentation (%d:%d)\n", line,
                        line_pos - pos + 2); /* one read missing + offset */
        } else {
                fprintf(stderr, "\t-> unknown token '%c' (%d:%d)\n", c, line,
                        line_pos - pos + 2); /* one read missing + offset */
        }
        return 1;
    }

    if (!seen_executable) {
        fprintf(stderr, "fari file does not contain 'E'\n");
        return 1;
    }

    return 0;
}

/******************************************************************************/
/*
static char *concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1);
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);
    return result;
}

int fari_read(const char *filename, char *buffer){
    int bytes_read = -1;
    int read;
    char *line_buf = NULL;
    size_t line_buf_size = 0;

    FILE* f = fopen(filename, "r");
    if( f != NULL){
        fseek(f, 0, SEEK_END);
        bytes_read = (int) ftell(f);
        fseek(f, 0, SEEK_SET);

        getline(&buffer, &line_buf_size, f);
        if(buffer[0] == '#'){
            buffer = "";
        }
        while ((read = getline(&line_buf, &line_buf_size, f)) != -1) {
            if(line_buf[0] == '#'){
                line_buf = "";
            }
            buffer = concat(buffer, line_buf);
        }
    }
    return bytes_read;
}
*/
