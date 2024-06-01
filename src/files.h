#ifndef FILES_H
#define FILES_H

#include <stddef.h>
#include <stdint.h>

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
	char ustarStr[6];
	char ustarVersion[2];
	char ownerName[32];
	char groupName[32];
	char devMajorNumber[8];
	char devMinorNumber[8];
	char prefix[155];
	char padding[12]; // to get the size to 512 bytes
} ustarHeader;

uint32_t parseOctStr(char* str);

void setRootDir(ustarHeader* header);
void initSiteFiles(char* path);
void uninitSiteFiles(void);
char* readSiteFile(char* path, size_t* s);

#endif
