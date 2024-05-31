#include "http.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "files.h"
#include "handlers.h"

void httpSendResponse(int socket, uint16_t status, char* data, size_t size) {
	// HTTP/1.1 200\r\n\r\n(data)
	char* response = malloc(size+17);
	uint8_t statusLen = sprintf(response, "HTTP/1.1 %i\r\n\r\n", status);
	memcpy(response+statusLen, data, size);
	size += statusLen;
	send(socket, response, size, 0);
	free(response);
}

void httpSendFile(int socket, char* file) {
	size_t responseSize;
	char* response;
	size_t filenameLen = strlen(file);
	response = readSiteFile(file, &responseSize);
	if(response == NULL) {
		char* indexPath = malloc(filenameLen + 12);
		if(file[0] == '\0' || file[filenameLen-1] == '/') {
			sprintf(indexPath, "%sindex.html", file);
		} else {
			sprintf(indexPath, "%s/index.html", file);
		}
		response = readSiteFile(indexPath, &responseSize);
		if(response == NULL) {
			httpHandleRequest(socket, "404"); // if there's no way to handle 404s then this just infinitely recurses through both functions lmao
			return;
		}
	}
	httpSendResponse(socket, 200, response, responseSize);
	close(socket);
	free(response);
}

void httpHandleRequest(int socket, char* request) {
	for(uint16_t i = 0; i < HANDLER_COUNT; ++i) {
		if(strcmp(request, httpHandlerStrings[i]) == 0) {
			httpHandlers[i](socket);
			close(socket);
			return;
		}
	}
	httpSendFile(socket, request);
	return;
}
