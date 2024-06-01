#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>

#include "files.h"
#include "http.h"

int serverSocket = 0;
int clientSocket = 0;	

void endServer(void) {
	printf("ending server\n");
	uninitSiteFiles();
	if(serverSocket) {
		close(serverSocket);
	}
	if(clientSocket) {
		close(clientSocket);
	}
	return;
}

// can't just put endServer as the signal handler due to atexit needing different a function pointer type
void handleSignal(int signalID) {
	(void)signalID;
	exit(0);
}

int main(int argc, char** argv) {
	atexit(endServer);
	signal(SIGINT, handleSignal);
	if(argc != 2) {
		printf("usage:\n\t%s (tar file with all the site's files)\n", argv[0]);
		return 1;
	}

	initSiteFiles(argv[1]);

	struct sockaddr_in serverAddr;
	struct sockaddr_in clientAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	memset(&clientAddr, 0, sizeof(clientAddr));
	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket < 0) {
		perror("socket");
		exit(1);
	}

	int opt = 1;
	if(setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		perror("setsockopt");
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
		clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);

		char buffer[2048];
		read(clientSocket, buffer, 2048);

		if(strncmp(buffer, "GET /", 5) == 0) {
			// GET /
			uint8_t len = 0;
			char* c = buffer+5;
			while(*c != ' ' && *c != '\0') {
				++len;
				++c;
				if(len > 80) { // semi-arbitrary
					fprintf(stderr, "get request too long\n");
					break;
				}
			}
			*c = '\0';
			if(len > 80) {
				httpHandleRequest(clientSocket, "404");
				continue;
			}
			char* target = malloc(len + 1);
			strcpy(target, buffer+5);
			target[len] = '\0';
			httpHandleRequest(clientSocket, target);
			free(target);
		} else {
			close(clientSocket);
		}
	}
	
	return 0;
}
