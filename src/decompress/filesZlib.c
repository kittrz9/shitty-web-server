#include "filesZlib.h"

#include <stdlib.h>

#ifdef WITH_ZLIB

#include <zlib.h>
#include <string.h>
#include "files.h"

gzFile gzTarFile = NULL;

void getRootDirZlib(FILE* tarFile) {
	ustarHeader header;
	gzTarFile = gzdopen(fileno(tarFile), "rb");
	gzread(gzTarFile, &header, 512);
	gzrewind(gzTarFile);
	setRootDir(&header);
	return;
}


char* fileSearchZlib(char* fullPath, size_t* s) {
	ustarHeader header;
	size_t bytesRead;
	do {
		bytesRead = gzread(gzTarFile, &header, 512);

		if(memcmp(header.ustarStr, "ustar", 5) != 0) {
			continue;
		}

		size_t size = parseOctStr(header.size);
		if(header.typeFlag != '5' && strcmp(header.name, fullPath) == 0) {
			char* data = malloc(size);
			gzread(gzTarFile, data, size);
			gzrewind(gzTarFile);
			*s = size;
			return data;
		}
		gzseek(gzTarFile, size + (512-size%512) - 512, SEEK_CUR);
	} while(bytesRead == 512);
	gzrewind(gzTarFile);
	*s = 0;
	return NULL;
}

void closeZlib(void) {
	gzclose(gzTarFile);
}

#else
void getRootDirZlib(FILE* tarFile) {
	(void)tarFile;
	fprintf(stderr, "no zlib support\n");
	exit(1);
}
char* fileSearchZlib(char* fullPath, size_t* s) {
	(void)fullPath;
	(void)s;
	fprintf(stderr, "no zlib support\n");
	exit(1);
}
void closeZlib(void) {
	fprintf(stderr, "no zlib support\n");
	exit(1);
}
#endif
