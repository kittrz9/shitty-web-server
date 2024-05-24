#ifndef FILES_H
#define FILES_H

#include <stddef.h>

void initSiteFiles();
void uninitSiteFiles();
char* readSiteFile(char* path, size_t* s);

#endif
