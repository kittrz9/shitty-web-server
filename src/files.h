#ifndef FILES_H
#define FILES_H

#include <stddef.h>

void initSiteFiles(char* path);
void uninitSiteFiles(void);
char* readSiteFile(char* path, size_t* s);

#endif
