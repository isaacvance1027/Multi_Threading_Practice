#ifndef FILE_H
#define FILE_H

#include <stdio.h>


FILE* openRead(FILE* fp, char* file);

FILE* openWrite(FILE* fp, char* file);

char* readFile(FILE* fp, char* readValue);

int writeFile(FILE* fp, char* writeValue);

#endif
