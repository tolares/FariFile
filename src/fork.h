#ifndef _FORK_H
#define _FORK_H

#define GCC "gcc"

#include "json.h"
#include "lib/cJSON.h"
#include "fari.h"
/* status code returned */
int fork_gcc(int flags_count, char **flags,
             const char *filename_c, const char *filename_o, struct json *json);
int fork_ld(int flags_count, char **flags, int libs_count, char **libs,
            const char *executable, int objs_count, char **objs, struct json *json);
char* get_command(char** pargv, const int size);
#endif /* _FORK_H */
