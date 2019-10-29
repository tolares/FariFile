#ifndef _FILE_H
#define _FILE_H

/* 1 if file exists */
int file_exists(const char *filename);
/* 1 if file has the right extension */
int file_has_extension(const char *filename, const char *extension);
char *file_change_extension(const char *filename, const char *extension);
/* 1 if file a is newer than file b */
int file_newer_than(const char *filename_a, const char *filename_b);

#endif /* _FILE_H */
