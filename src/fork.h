#ifndef _FORK_H
#define _FORK_H

#define GCC "gcc"

/* status code returned */
int fork_gcc(int flags_count, char **flags,
             const char *filename_c, const char *filename_o);
int fork_ld(int flags_count, char **flags, int libs_count, char **libs,
            const char *executable, int objs_count, char **objs);
#endif /* _FORK_H */
