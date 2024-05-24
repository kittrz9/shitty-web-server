#include "files.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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

char* siteFiles = NULL;
size_t siteFilesSize;
char* rootName = NULL;

void initSiteFiles(void) {
	FILE* f = fopen("html.tar", "rb");
	if(f == NULL) {
		perror("fopen");
		exit(1);
	}
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);
	fseek(f, 0, SEEK_SET);
	siteFiles = malloc(size);
	fread(siteFiles, size, 1, f);
	siteFilesSize = size;
	fclose(f);

	// first entry should give the root directory name
	size = strlen(siteFiles);
	rootName = malloc(size+1);
	strcpy(rootName, siteFiles);
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

char* readSiteFile(char* name, size_t* s) {
	size_t len = strlen(name) + strlen(rootName) + 1;
	char* fullPath = malloc(len);
	strcpy(fullPath, rootName);
	strcat(fullPath, name);
	//fullPath[len-1] = '\0';
	uint32_t i = 0; 
	while(i < siteFilesSize) {
		ustarHeader* header = (ustarHeader*)&siteFiles[i];

		if(memcmp(header->ustarStr, "ustar", 5) != 0) {
			i+=512;
			continue;
		}

		size_t size = parseOctStr(header->size);
		if(strcmp(header->name, fullPath) == 0) {
			free(fullPath);
			char* data = malloc(size);
			memcpy(data, &siteFiles[i+512], size);
			*s = size;
			return data;
		}
		i += 512;
	}

	free(fullPath);
	*s = 0;
	return NULL;
}

void uninitSiteFiles(void) {
	free(siteFiles);
	free(rootName);
}
