#ifndef MULTI_LOOKUP_H
#define MULTI_LOOKUP_H

#include "stack.h"
#include "constants.h"
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

// Mutex struct definition

struct Protector{
	struct Stack* stack;
	char** arguments;
	int threads;
	int whichFile;
	int files;
	pthread_mutex_t stackLock;
	pthread_mutex_t requesterLock;
	pthread_mutex_t requesterLogLock;
	pthread_mutex_t errLock;
	pthread_mutex_t outputLock;
	pthread_mutex_t	filesLock;
	sem_t emptyStack;
	sem_t fullStack;
};

// Thread Argument Struct

struct ThreadArg{
	FILE* requestLog;
	FILE* resolveLog;
	struct Protector* p;
};

struct Protector* protectorConstructor(char** argv, int argc, int requesters);
void protectorDestructor(struct Protector* p);

void* request(void* arg);

#endif
