#include "files.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// https://wiki.osdev.org/USTAR

typedef struct {
	char name[100];
	char mode[8];
	char ownerId[8];
	char groupId[8];
	char size[12];
	char modifyTime[12];
	char checksum[8];
	char typeFlag;
	char linkedFile[100];
	char ustarStr[5];
	char ustarVersion[2];
	char ownerName[32];
	char groupName[32];
	char devMajorNumber[8];
	char devMinorNumber[8];
	char prefix[155];
} ustarHeader;

char* rootName = NULL;
FILE* tarFile = NULL;
enum {
	COMPRESSED_BZIP2,
	COMPRESSED_NONE,
} compressionType = COMPRESSED_NONE;

#ifdef WITH_BZIP2
#include <bzlib.h>

BZFILE* bzFile = NULL;
int bzerror = -1;

void resetBZip2File(void) {
	BZ2_bzReadClose(&bzerror, bzFile);
	fseek(tarFile, 0, SEEK_SET);
	bzFile = BZ2_bzReadOpen(&bzerror, tarFile, 0, 0, NULL, 0);
}

#endif

void initSiteFiles(char* path) {
	if(!strstr(path, ".tar")) {
		printf("provided file is not a tar file\n");
		exit(1);
	}

	tarFile = fopen(path, "rb");
	if(tarFile == NULL) {
		perror("fopen");
		exit(1);
	}

	if(strstr(path, ".bz2")) {
		compressionType = COMPRESSED_BZIP2;
	}

	// first entry should give the root directory name
	char buffer[512];
	switch(compressionType) {
		case COMPRESSED_BZIP2:
#ifdef WITH_BZIP2
		{
			bzerror = -1;
			bzFile = BZ2_bzReadOpen(&bzerror, tarFile, 0, 0, NULL, 0);
			if(!bzFile) {
				printf("could not open bz2 file: %i\n", bzerror);
				exit(1);
			}
			BZ2_bzRead(&bzerror, bzFile, buffer, 512);
			if(bzerror < BZ_OK) {
				printf("bzRead error: %i\n", bzerror);
				exit(1);
			}
			fseek(tarFile, 0, SEEK_SET);
			resetBZip2File();
			break;
		}
#else
			printf("no bzip2 support\n");
			exit(1);
			break;
#endif
		default: 
			fread(buffer, 512, 1, tarFile);
			fseek(tarFile, 0, SEEK_SET);
			break;
	}
	uint8_t len = strlen(buffer);
	rootName = malloc(len+1);
	strcpy(rootName, buffer);
}

uint32_t parseOctStr(char* str) {
	uint32_t value = 0;
	size_t len = strlen(str);
	char* c = str;
	while(len-- > 0) {
		value *= 8;
		value += *c - '0';
		++c;
	}
	return value;
}

char* fileSearchNoComp(char* fullPath, size_t* s) {
	char ustarHeaderBuffer[512];
	size_t bytesRead;
	do {
		bytesRead = fread(ustarHeaderBuffer, 1, 512, tarFile);
		ustarHeader* header = (ustarHeader*)ustarHeaderBuffer;

		if(memcmp(header->ustarStr, "ustar", 5) != 0) {
			continue;
		}

		size_t size = parseOctStr(header->size);
		if(strcmp(header->name, fullPath) == 0) {
			char* data = malloc(size);
			fread(data, size, 1, tarFile);
			fseek(tarFile, 0, SEEK_SET);
			*s = size;
			return data;
		}
		fseek(tarFile, size + (512-size%512) - 512, SEEK_CUR);
	} while(bytesRead == 512);
	fseek(tarFile, 0, SEEK_SET);
	*s = 0;
	return NULL;
}

char* fileSearchBZ2(char* fullPath, size_t* s) {
#ifdef WITH_BZIP2
	char ustarHeaderBuffer[512];
	size_t bytesRead;
	do {
		bytesRead = BZ2_bzRead(&bzerror, bzFile, ustarHeaderBuffer, 512);
		ustarHeader* header = (ustarHeader*)ustarHeaderBuffer;

		if(memcmp(header->ustarStr, "ustar", 5) != 0) {
			continue;
		}

		size_t size = parseOctStr(header->size);
		if(strcmp(header->name, fullPath) == 0) {
			char* data = malloc(size);
				BZ2_bzRead(&bzerror, bzFile, data, size);
				fseek(tarFile, 0, SEEK_SET);
				resetBZip2File();
			*s = size;
			return data;
		}
	} while(bytesRead == 512);
	resetBZip2File();
	*s = 0;
	return NULL;
#else
	(void)fullPath;
	(void)s;
	printf("no bzip2 support\n");
	exit(1);
#endif
}

char* readSiteFile(char* name, size_t* s) {
	size_t len = strlen(name) + strlen(rootName) + 1;
	char* fullPath = malloc(len);
	strcpy(fullPath, rootName);
	strcat(fullPath, name);
	char* data;
	switch(compressionType) {
		case COMPRESSED_NONE:
			data = fileSearchNoComp(fullPath, s);
			break;
		case COMPRESSED_BZIP2:
			data = fileSearchBZ2(fullPath, s);
			break;
		default:
			printf("invalid compression type\n");
			exit(1);
			break;
	}
	free(fullPath);
	return data;
}

void uninitSiteFiles(void) {
	free(rootName);
	if(compressionType == COMPRESSED_BZIP2) {
#ifdef WITH_BZIP2
		BZ2_bzReadClose(&bzerror, bzFile);
#endif
	}
	fclose(tarFile);
}
