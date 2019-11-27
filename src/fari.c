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

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fari.h"
#include "file.h"
#include "fork.h"
#include "lib/cJSON.h"

struct fari *fari_create()
{
    struct fari *fari;

    fari = calloc(1, sizeof(*fari));
    return fari;
}

void fari_free(struct fari *fari)
{
    int i;

    if (NULL == fari)
        return;

    if (NULL != fari->flags) {
        for (i = 0; i < fari->flags_count; ++i)
            free(fari->flags[i]);
        free(fari->flags);
    }
    if (NULL != fari->libs) {
        for (i = 0; i < fari->libs_count; ++i)
            free(fari->libs[i]);
        free(fari->libs);
    }
    if (NULL != fari->headers) {
        for (i = 0; i < fari->headers_count; ++i)
            free(fari->headers[i]);
        free(fari->headers);
    }
    if (NULL != fari->sources) {
        for (i = 0; i < fari->sources_count; ++i)
            free(fari->sources[i]);
        free(fari->sources);
    }
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
        fprintf(stderr, "fari file does not exist\n");
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
        case 'H':
        case 'J':       return 0; break;
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

static void field_set(char **field, const char *str)
{
    int str_len;

    str_len = strlen(str);
    *field = malloc((str_len + 1) * sizeof(char));
    strcpy(*field, str);
}

void field_add(char ***field, int field_size, const char *str)
{
    int len_str;

    len_str = strlen(str);      /* TODO: strnlen ? */

    if (NULL == *field) {

        *field = malloc(sizeof(char *));
        if (NULL == *field)
                return;
        (*field)[field_size] = malloc((len_str + 1) * sizeof(char));
        if (NULL == (*field)[field_size])
                return;
        strcpy(*field[field_size], str);
    } else {
        *field = realloc(*field, (field_size + 1) * sizeof(char *));
        if (NULL == *field)
                return;
        (*field)[field_size] = malloc((len_str + 1) * sizeof(char));
        strcpy((*field)[field_size], str);
    }
}

int fari_analyse(struct fari *fari, const char *buffer)
{
    int seen_executable;
    int seen_c;
    int seen_java;
    int line, line_pos;
    int pos;
    char c;
    char str[FARI_PATH_MAX];

    seen_executable = 0;
    seen_c = 0;
    seen_java = 0;
    line = 1;
    line_pos = 0;
    pos = 0;

    while (!read_token(&c, buffer, &pos)) {
        switch (c) {
            case 'B':
                while((buffer[pos] != '\x0a') && (buffer[pos] != '\x0d')){
                    read_spaces(buffer, &pos);
                    read_str(&str[0], sizeof(str) / sizeof(char), buffer, &pos);
                    if ('\0' == str[0]) {
                        fprintf(stderr, "fari file syntax error\n");
                        fprintf(stderr, "\t-> argument expected for token 'B'\n");
                        return 1;
                    }
                    field_add(&fari->libs, fari->libs_count++, str);
                }
                break;
            case 'C':
                if (seen_java) {
                    fprintf(stderr, "fari file contains C and Java\n");
                    return 1;
                }
                seen_c = 1;
                while((buffer[pos] != '\x0a') && (buffer[pos] != '\x0d')){
                    read_spaces(buffer, &pos);
                    read_str(&str[0], sizeof(str) / sizeof(char), buffer, &pos);
                    field_add(&fari->sources, fari->sources_count++, str);
                }
                break;
            case 'E':
                if (seen_executable) {
                    fprintf(stderr, "fari file contains 'E' twice\n");
                    return 1;
                }
                seen_executable = 1;
                read_spaces(buffer, &pos);
                read_str(&str[0], sizeof(str) / sizeof(char), buffer, &pos);
                if ('\0' == str[0]) {
                    fprintf(stderr, "fari file syntax error\n");
                    fprintf(stderr, "\t-> argument expected for token 'E'\n");
                    return 1;
                }
                field_set(&fari->executable, str);
                break;
            case 'F':
                while((buffer[pos] != '\x0a') && (buffer[pos] != '\x0d')){
                    read_spaces(buffer, &pos);
                    read_str(&str[0], sizeof(str) / sizeof(char), buffer, &pos);
                    if ('\0' == str[0]) {
                        fprintf(stderr, "fari file syntax error\n");
                        fprintf(stderr, "\t-> argument expected for token 'F'\n");
                        return 1;
                    }
                    field_add(&fari->flags, fari->flags_count++, str);
                }
                break;
            case 'H':
                if (seen_java) {
                    fprintf(stderr, "fari file contains C and Java\n");
                    return 1;
                }
                seen_c = 1;
                while((buffer[pos] != '\x0a') && (buffer[pos] != '\x0d')){
                    read_spaces(buffer, &pos);
                    read_str(&str[0], sizeof(str) / sizeof(char), buffer, &pos);
                    field_add(&fari->headers, fari->headers_count++, str);
                }
                break;
            case 'J':
                if (seen_c) {
                    fprintf(stderr, "fari file contains C and Java\n");
                    return 1;
                }
                seen_java = 1;
                fari->is_java = 1;
                while((buffer[pos] != '\x0a') && (buffer[pos] != '\x0d')){
                    read_spaces(buffer, &pos);
                    read_str(&str[0], sizeof(str) / sizeof(char), buffer, &pos);
                    field_add(&fari->sources, fari->sources_count++, str);
                }
                break;
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

    if (!seen_c && !seen_java) {
        fprintf(stderr, "fari file does not contain source file\n");
        return 1;
    }

    return 0;
}

int fari_check(struct fari *fari)
{
    int i;
    char *src_ext;

    if (!fari->is_java)
        src_ext = "c";
    else
        src_ext = "java";

    for (i = 0; i < fari->sources_count; ++i) {
        if (!file_exists(fari->sources[i])) {
            fprintf(stderr, "file \"%s\" does not exist\n", fari->sources[i]);
            return 1;
        }
        if (!file_has_extension(fari->sources[i], src_ext)) {
            fprintf(stderr, "file \"%s\" is not a source file\n", fari->sources[i]);
            return 1;
        }
    }
    for (i = 0; i < fari->headers_count; ++i) {
        if (!file_exists(fari->headers[i])) {
            fprintf(stderr, "file \"%s\" does not exist\n", fari->headers[i]);
            return 1;
        }
        if (!file_has_extension(fari->headers[i], "h")) {
            fprintf(stderr, "file \"%s\" is not a header file\n", fari->headers[i]);
            return 1;
        }
    }

    return 0;
}

int fari_compile(struct fari *fari, struct json *json)
{
    char *newest_header;
    char *oldest_obj;
    int recompile_all;
    const char *filename_c;
    char *filename_o;
    char *newest_obj;
    int objs_count;
    char **objs;
    int i;
    int status;
    char *obj_ext;

    newest_header = NULL;
    oldest_obj = NULL;
    recompile_all = 0;
    newest_obj = NULL;

    if (!fari->is_java)
        obj_ext = "o";
    else
        obj_ext = "class";

    for (i = 0; i < fari->headers_count; ++i) {
        if (!newest_header || file_newer_than(fari->headers[i], newest_header))
            newest_header = fari->headers[i];
    }
    for (i = 0; i < fari->sources_count; ++i) {
        filename_c = fari->sources[i];
        filename_o = file_change_extension(filename_c, obj_ext);
        if (!oldest_obj || file_newer_than(oldest_obj, filename_o)) {
            free(oldest_obj);
            oldest_obj = filename_o;
        } else {
            free(filename_o);
        }
    }
    if (newest_header && oldest_obj && file_newer_than(newest_header, oldest_obj))
        recompile_all = 1;

    for (i = 0; i < fari->sources_count; ++i) {
        filename_c = fari->sources[i];
        filename_o = file_change_extension(filename_c, obj_ext);
        if (NULL == filename_o)
            return 1;
        if (!file_exists(filename_o) ||
            file_newer_than(filename_c, filename_o) ||
            recompile_all) {
            /* recompile */
            /* printf("DEBUG | recompiling %s <- %s\n", filename_o, filename_c); */
            if (!fari->is_java)
                status = fork_gcc(fari->flags_count, fari->flags,
                                  filename_c, filename_o, json);
            else
                status = fork_javac(filename_c, filename_o);
            printf("\tstatus <- %d\n", status);
            if (status) {
                fprintf(stderr, "compilation failed\n");
                free(filename_o);
                free(oldest_obj);
                return 1;
            }
        }
        free(filename_o); /* bof... */
    }

    free(oldest_obj);

    for (i = 0; i < fari->sources_count; ++i) {
        filename_c = fari->sources[i];
        filename_o = file_change_extension(filename_c, obj_ext);
        if (!newest_obj || file_newer_than(filename_o, newest_obj)) {
            free(newest_obj);
            newest_obj = filename_o;
        } else {
            free(filename_o);
        }
    }

    if (!fari->is_java && (!file_exists(fari->executable) ||
        file_newer_than(newest_obj, fari->executable))) {
        /* relink */
        objs_count = fari->sources_count;
        objs = malloc(fari->sources_count * sizeof(char *));
        if (!objs) {
            return 1;
        }
        for (i = 0; i < fari->sources_count; ++i) {
            filename_c = fari->sources[i];
            filename_o = file_change_extension(filename_c, obj_ext);
            objs[i] = filename_o;
        }
        /* printf("DEBUG | relinking %s\n", fari->executable); */
        status = fork_ld(fari->flags_count, fari->flags,
                         fari->libs_count, fari->libs,
                         fari->executable,
                         objs_count, objs, json);
        printf("\tstatus <- %d\n", status);
        printf("%d \n", (json)->commands_number);
        for (i = 0; i < objs_count; ++i)
            free(objs[i]);
        free(objs);
        if (status) {
            fprintf(stderr, "linking failed\n");
            free(newest_obj);
            return 1;
        }
    }

    free(newest_obj);

    return 0;
}
