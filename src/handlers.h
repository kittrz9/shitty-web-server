#ifndef HANDLERS_H
#define HANDLERS_H

// just so I don't have to make sure everything has their indices lined up
enum {
	HANDLER_404,
	HANDLER_COUNTER,
	HANDLER_COUNT, // maybe should change this name to avoid confusion
};
extern void (*httpHandlers[])(int);
extern char* httpHandlerStrings[];

#endif 
