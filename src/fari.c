//
//  fari.c
//  RS Project
//
//  Created by Lagarde Henry on 14/10/2019.
//  Copyright © 2019 Lagarde Henry. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


char* concat(const char *s1, const char *s2)
{
    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = malloc(len1 + len2 + 1);
    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);
    return result;
}


int fari_read(const char *filename, char *buffer){
    int bytes_read;
    int read;
    char *line_buf = NULL;
    size_t line_buf_size = 0;
    
    FILE* f = fopen(filename, "r");
    fseek(f, 0, SEEK_END);
    bytes_read = (int) ftell(f);
    fseek(f, 0, SEEK_SET);
    
    getline(&line_buf, &line_buf_size, f);
    char* result = line_buf;
    if(line_buf[0] == '#'){
        result = "";
    }
    
    while ((read = getline(&line_buf, &line_buf_size, f)) != -1) {
        if(line_buf[0] == '#'){
            line_buf = "";
        }
        result = concat(result, line_buf);
        
    }
    return bytes_read;
}
