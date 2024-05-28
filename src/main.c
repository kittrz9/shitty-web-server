#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "files.h"
#include "http.h"

int main(int argc, char** argv) {
	if(argc != 2) {
		printf("usage:\n\t%s (tar file with all the site's files)\n", argv[0]);
		return 1;
	}

	initSiteFiles(argv[1]);

	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	memset(&clientAddr, 0, sizeof(clientAddr));
	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket < 0) {
		perror("socket");
		exit(1);
	}

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddr.sin_port = htons(80);

	if(bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("bind");
		exit(1);
	}

	
	listen(serverSocket, 5);
	uint32_t clientLen = sizeof(clientAddr);
	while(1) {
		int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen); 	

		char buffer[2048];
		read(clientSocket, buffer, 2048);

		if(strncmp(buffer, "GET", 3) == 0) {
			// GET / 
			if(buffer[5] == ' ') {
				httpHandleRequest(clientSocket, "index.html");
			} else {
				uint8_t len = 0;
				char* c = buffer+5;
				while(*c != ' ') {
					++len;
					++c;
					if(len > 80) { // semi-arbitrary
						fprintf(stderr, "get request too long\n");
						// have to do this to skip this in the outer loop
						c = NULL;
						break;
					}
				}
				if(c == NULL) {
					httpHandleRequest(clientSocket, "404");
					continue;
				}
				char* target = malloc(len + 11); // need room for index.html just in case
				strncpy(target, buffer+5, len);
				target[len] = '\0';
				if(*(c-1) == '/') {
					strcat(target, "index.html");
					len += 10;
				}
				httpHandleRequest(clientSocket, target);
				free(target);
			}
		}
	}
	close(serverSocket);
	uninitSiteFiles();
	
	return 0;
}
