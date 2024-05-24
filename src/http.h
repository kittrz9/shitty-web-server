#ifndef HTTP_H
#define HTTP_H

void httpSendFile(int socket, char* file);
void httpHandleRequest(int socket, char* request);

#endif
