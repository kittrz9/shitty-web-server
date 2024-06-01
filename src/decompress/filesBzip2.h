#ifndef FILES_BZIP2
#define FILES_BZIP2

#include <stdio.h>

void getRootDirBzip2(FILE* tarFile);
char* fileSearchBzip2(FILE* tarFile, char* fullPath, size_t* s);
void closeBzip2(void);

#endif
