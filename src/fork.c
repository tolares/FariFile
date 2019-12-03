#include <stdlib.h>
#include <string.h>

#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include "fork.h"

int fork_gcc(int flags_count, char **flags,
             const char *filename_c, const char *filename_o, struct json *json) {
    int pid;
    int wstatus;
    char **pargv;
    int i;
    char* final;
    int fd[2];
    char* buffer = malloc(65536 * sizeof(char));

    /* execvp */
    pargv = malloc((6 + flags_count) * sizeof(char *));
    pargv[0] = malloc((strlen(GCC) + 1) * sizeof(char));
    for (i = 0; i < flags_count; ++i) {
        pargv[1 + i] = malloc((strlen(flags[i]) + 1) * sizeof(char));
    }
    pargv[1 + flags_count] = malloc(3 * sizeof(char));
    pargv[2 + flags_count] = malloc((strlen(filename_c) + 1) * sizeof(char));
    pargv[3 + flags_count] = malloc(3 * sizeof(char));
    pargv[4 + flags_count] = malloc((strlen(filename_o) + 1) * sizeof(char));
    if (!pargv)
        exit(1);
    for (i = 0; i < (5 + flags_count); ++i) {
        if (!pargv[i])
            exit(1);
    }
    strcpy(pargv[0], GCC);
    for (i = 0; i < flags_count; ++i) {
        strcpy(pargv[1 + i], flags[i]);
    }
    strcpy(pargv[1 + flags_count], "-c");
    strcpy(pargv[2 + flags_count], filename_c);
    strcpy(pargv[3 + flags_count], "-o");
    strcpy(pargv[4 + flags_count], filename_o);
    pargv[5 + flags_count] = NULL;
    pipe(fd);
    pid = fork();
    if (0 > pid) {
        return 1;
    } else if (0 == pid) {
        dup2(fd[1],2);
        execvp(GCC, pargv);
        return 1; /* impossible */
    } else {

        final = get_command(pargv, 5 + flags_count);
        read(fd[1], buffer, 65536);
        strcpy(json->linking_errors,buffer);
        field_add(&json->commands_compilation, json->compilation_numbers++, final);
        /* wait for status code */
        wait(&wstatus);
        if (WIFEXITED(wstatus))
            return WEXITSTATUS(wstatus);
        else
            return 1;
    }
}

int fork_ld(int flags_count, char **flags, int libs_count, char **libs,
            const char *executable, int objs_count, char **objs, struct json *json) {
    int pid;
    int wstatus;
    char **pargv;
    int i;
    char* final;
    int fd[2];

    /*GCC Command*/
    pargv = malloc((4 + flags_count + libs_count + objs_count) * sizeof(char *));
    pargv[0] = malloc((strlen(GCC) + 1) * sizeof(char));
    for (i = 0; i < flags_count; ++i) {
        pargv[1 + i] = malloc((strlen(flags[i]) + 1) * sizeof(char));
    }
    pargv[1 + flags_count] = malloc(3 * sizeof(char));
    pargv[2 + flags_count] = malloc((strlen(executable) + 1) * sizeof(char));
    for (i = 0; i < objs_count; ++i) {
        pargv[3 + flags_count + i] = malloc((strlen(objs[i]) + 1) * sizeof(char));
    }
    for (i = 0; i < libs_count; ++i) {
        pargv[3 + flags_count + objs_count + i] = malloc((strlen(libs[i]) + 1) * sizeof(char));
    }
    if (!pargv)
        exit(1);
    for (i = 0; i < (3 + flags_count + objs_count + libs_count); ++i) {
        if (!pargv[i])
            exit(1);
    }
    strcpy(pargv[0], GCC);
    for (i = 0; i < flags_count; ++i) {
        strcpy(pargv[1 + i], flags[i]);
    }
    strcpy(pargv[1 + flags_count], "-o");
    strcpy(pargv[2 + flags_count], executable);
    for (i = 0; i < objs_count; ++i) {
        strcpy(pargv[3 + flags_count + i], objs[i]);
    }
    for (i = 0; i < libs_count; ++i) {
        strcpy(pargv[3 + flags_count + objs_count + i], libs[i]);
    }
    pargv[3 + flags_count + objs_count + libs_count] = NULL;

    pipe(fd);
    char * buffer = malloc(65536 * sizeof(char));
    pid = fork();
    if (0 > pid) {
        return 1;
    } else if (0 == pid) {
        dup2(fd[1],2);
        /* execvp */
        execvp(GCC, pargv);
        printf("cc\n");
        return 1; /* impossible */
    } else {
        final = get_command(pargv, 3 + flags_count + objs_count + libs_count);
        read(fd[1], buffer, 65536);
        strcpy(json->linking_errors,buffer);
        field_add(&json->commands, json->commands_number++, final);
        /* wait for status code */
        wait(&wstatus);
        if (WIFEXITED(wstatus)){
            return WEXITSTATUS(wstatus);
        }
        else
            return 1;
    }
}

char* get_command( char **pargv, const int size) {
    char *final;
    int n, i;
    cJSON *string;
    n = 0;
    for (i = 0; i < size; ++i) {
        n = strlen(pargv[i]) + 1;
    }
    final = malloc(sizeof(char) * 4098);
    for (i = 0; i < size - 1; ++i) {
        strcat(final, pargv[i]);
        strcat(final, " ");
    }
    strcat(final, pargv[size - 1]);

    return final;
}
