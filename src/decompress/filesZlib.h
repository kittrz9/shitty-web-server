#ifndef FILES_ZLIB
#define FILES_ZLIB

#include <stdio.h>

void getRootDirZlib(FILE* tarFile);
char* fileSearchZlib(char* fullPath, size_t* s);
void closeZlib(void);

#endif
