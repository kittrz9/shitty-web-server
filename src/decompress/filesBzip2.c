#include "filesBzip2.h"

#include <stdlib.h>

#ifdef WITH_BZIP2

#include <bzlib.h>
#include <stdio.h>
#include <string.h>

#include "files.h"

BZFILE* bzFile = NULL;
int bzerror = -1;

void resetBZip2File(FILE* tarFile) {
	BZ2_bzReadClose(&bzerror, bzFile);
	rewind(tarFile);
	bzFile = BZ2_bzReadOpen(&bzerror, tarFile, 0, 0, NULL, 0);
}

void getRootDirBzip2(FILE* tarFile) {
	ustarHeader header;
	bzerror = -1;

	bzFile = BZ2_bzReadOpen(&bzerror, tarFile, 0, 0, NULL, 0);
	if(!bzFile) {
		fprintf(stderr, "could not open bz2 file: %i\n", bzerror);
		exit(1);
	}
	BZ2_bzRead(&bzerror, bzFile, &header, 512);
	if(bzerror < BZ_OK) {
		fprintf(stderr, "bzRead error: %i\n", bzerror);
		exit(1);
	}
	resetBZip2File(tarFile);
	setRootDir(&header);
}

char* fileSearchBzip2(FILE* tarFile, char* fullPath, size_t* s) {
	ustarHeader header;
	size_t bytesRead;
	do {
		bytesRead = BZ2_bzRead(&bzerror, bzFile, &header, 512);

		if(memcmp(header.ustarStr, "ustar", 5) != 0) {
			continue;
		}

		size_t size = parseOctStr(header.size);
		if(header.typeFlag != '5' && strcmp(header.name, fullPath) == 0) {
			char* data = malloc(size);
			BZ2_bzRead(&bzerror, bzFile, data, size);
			resetBZip2File(tarFile);
			*s = size;
			return data;
		}
	} while(bytesRead == 512);
	resetBZip2File(tarFile);
	*s = 0;
	return NULL;
}
void closeBzip2(void) {
	BZ2_bzReadClose(&bzerror, bzFile);
	return;
}

#else
void getRootDirBzip2(FILE* tarFile) {
	(void)tarFile;
	fprintf(stderr, "no bzip2 support\n");
	exit(1);
}
char* fileSearchBzip2(FILE* tarFile, char* fullPath, size_t* s) {
	(void)tarFile;
	(void)fullPath;
	(void)s;
	fprintf(stderr, "no bzip2 support\n");
	exit(1);
}
void closeBzip2(void) {
	fprintf(stderr, "no bzip2 support\n");
	exit(1);
}
#endif
