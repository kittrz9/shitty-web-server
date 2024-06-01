#include "files.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "decompress/filesZlib.h"
#include "decompress/filesBzip2.h"

// https://wiki.osdev.org/USTAR


char* rootName = NULL;
FILE* tarFile = NULL;
enum {
	COMPRESSED_NONE,
	COMPRESSED_BZIP2,
	COMPRESSED_ZLIB,
} compressionType = COMPRESSED_NONE;

void setRootDir(ustarHeader* header) {
	if(header->typeFlag != '5') {
		fprintf(stderr, "provided root not a directory\n");
		exit(1);
	}
	uint8_t len = strlen(header->name);
	if(header->name[len-1] != '/') {
		header->name[len] = '/';
		++len;
	}
	rootName = malloc(len+1);
	strcpy(rootName, header->name);

	return;
}

void initSiteFiles(char* path) {
	if(!strstr(path, ".tar") && !strstr(path, ".pax")) {
		fprintf(stderr, "provided file is not a tar or pax file\n");
		exit(1);
	}

	tarFile = fopen(path, "rb");
	if(tarFile == NULL) {
		perror("fopen");
		exit(1);
	}

	if(strstr(path, ".bz2")) {
		compressionType = COMPRESSED_BZIP2;
	} else if(strstr(path, ".gz")) {
		compressionType = COMPRESSED_ZLIB;
	}

	// first entry should give the root directory name
	switch(compressionType) {
		case COMPRESSED_NONE: {
			ustarHeader header;
			fread(&header, 512, 1, tarFile);
			rewind(tarFile);
			setRootDir(&header);
			break;
		}
		case COMPRESSED_BZIP2:
			getRootDirBzip2(tarFile);
			break;
		case COMPRESSED_ZLIB:
			getRootDirZlib(tarFile);
			break;
		default:
			fprintf(stderr, "invalid compression type\n");
			exit(1);
	}
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
	ustarHeader header;
	size_t bytesRead;
	do {
		bytesRead = fread(&header, 1, 512, tarFile);

		if(memcmp(header.ustarStr, "ustar", 5) != 0) {
			continue;
		}

		size_t size = parseOctStr(header.size);
		if(header.typeFlag != '5' && strcmp(header.name, fullPath) == 0) {
			char* data = malloc(size);
			fread(data, size, 1, tarFile);
			rewind(tarFile);
			*s = size;
			return data;
		}
		fseek(tarFile, size + (512-size%512) - 512, SEEK_CUR);
	} while(bytesRead == 512);
	rewind(tarFile);
	*s = 0;
	return NULL;
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
			// really hate how this breaks the pattern because I have to reset the tar file to then close and open it instead of just rewinding it
			data = fileSearchBzip2(tarFile, fullPath, s);
			break;
		case COMPRESSED_ZLIB:
			data = fileSearchZlib(fullPath, s);
			break;
		default:
			fprintf(stderr, "invalid compression type\n");
			exit(1);
			break;
	}
	free(fullPath);
	return data;
}

void uninitSiteFiles(void) {
	if(rootName) {
		free(rootName);
		switch(compressionType) {
			case COMPRESSED_BZIP2:
				closeBzip2();
				break;
			case COMPRESSED_ZLIB:
				closeZlib();
				break;
			default:
				break;
		}
		fclose(tarFile);
	}
}
