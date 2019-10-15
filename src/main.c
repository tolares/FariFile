//
//  main.c
//  RS Project
//
//  Created by Lagarde Henry on 14/10/2019.
//  Copyright © 2019 Lagarde Henry. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fari.h"

int main(){
   
    char* buffer = NULL;
    int size = fari_read("/Users/henry/Documents/Lab/RS/RS Project/src/resources/farifile", buffer);
    printf("%d\n", size);
    return 0;
}
