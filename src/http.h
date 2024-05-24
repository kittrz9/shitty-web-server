#ifndef HTTP_H
#define HTTP_H

#include <stdint.h>
#include <stddef.h>

void httpSendFile(int socket, char* file);
void httpHandleRequest(int socket, char* request);
void httpSendResponse(int socket, uint16_t status, char* data, size_t size);

#endif
