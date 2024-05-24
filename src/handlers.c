#include "handlers.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "http.h"

char* redirectURLs[] = {
	"https://www.youtube.com/watch?v=9zd7YnF_Acc",
	"https://www.youtube.com/watch?v=QOM_r5q3tAo",
	"https://www.youtube.com/watch?v=nBVf_hGygXQ",
	"https://www.youtube.com/watch?v=aM4CJppHTW4",
	"https://www.youtube.com/watch?v=SLxcY9amtlM",
	"https://www.youtube.com/watch?v=EFQbltFVXyo",
	"https://www.youtube.com/watch?v=1hp6kMsNStI",
	"https://www.youtube.com/watch?v=BOMHx39CTAk",
	"https://www.youtube.com/watch?v=eXOdvDaV0pQ",
	"https://www.youtube.com/watch?v=-LqPU4132Xg",
};

void httpHandle404(int socket) {
	char start[] = "<html><meta http-equiv=\"Refresh\" content=\"0; ";
	char end[] = "\" /></html>";
	char* redirect = redirectURLs[rand() % (sizeof(redirectURLs)/sizeof(redirectURLs[0]))];
	size_t pageSize = sizeof(start) + sizeof(end) + strlen(redirect) + 1;

	char* fullPage = malloc(pageSize);


	snprintf(fullPage, pageSize, "%s%s%s", start, redirect, end);
	fullPage[pageSize-1] = '\0';
	httpSendResponse(socket, 404, fullPage, strlen(fullPage));
	free(fullPage);
	return;
}

void httpHandleCounter(int socket) {
	static uint16_t i = 0;

	char start[] = "<html><body><h1>";
	char end[] = "</h1></body></html>";
	size_t pageSize = sizeof(start) + sizeof(end) + 6; // add in space for the 16 bit number and null byte

	char* fullPage = malloc(pageSize);

	snprintf(fullPage, pageSize, "%s%i%s", start, i, end);
	fullPage[pageSize-1] = '\0';
	httpSendResponse(socket, 200, fullPage, strlen(fullPage));
	free(fullPage);
	++i;
	return;
}

void (*httpHandlers[])(int) = {
	[HANDLER_404] = httpHandle404,
	[HANDLER_COUNTER] = httpHandleCounter,
};

char* httpHandlerStrings[] = {
	[HANDLER_404] = "404",
	[HANDLER_COUNTER] = "counter",
};
