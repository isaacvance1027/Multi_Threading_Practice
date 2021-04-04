#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>

#include "constants.h"
#include "stack.h"
#include "multi-lookup.h"
#include "util.h"
#include "file.h"


int main(int argc, char* argv[]){
/*******************************************************************************
														PROGRAM INITIALIZATION
*******************************************************************************/
	// Start Timer for calculating program run time
	// gettimeofday accepts struct by reference and updates values there
	// returns 0 on success, -1 on fail as per tradition
	struct timeval start, stop;
	gettimeofday(&start, NULL);

	//initialize variables for tracking requesters and resolvers
	int requesters = 0;
	int resolvers = 0;
	char** requesterPtr = NULL;
	char** resolverPtr = NULL;

	//check argument bounds
	if (argc < 5){
		printf("TOO FEW Arguments. Please supply the correct input as specified "
						"in the document\n");
						exit(EXIT_FAILURE);
	} else if (argc > 105){
		printf("TOO MANY Arguments. Please supply the correct input as specified "
						"in the document\n");
						exit(EXIT_FAILURE);
	}
/*******************************************************************************
														INPUT VALIDATION
*******************************************************************************/
	//check number of requesters and readers
	requesters = (int)strtol(argv[1], requesterPtr, 10);
	resolvers = (int)strtol(argv[2], resolverPtr, 10);

	if (requesters <= 0 || resolvers <= 0){
		fprintf(stderr, "Too FEW requesters or resolvers\n");
		exit(EXIT_FAILURE);
	}
	if (requesters > MAX_REQUESTER_THREADS || resolvers > MAX_RESOLVER_THREADS){
		fprintf(stderr, "Too MANY requesters or resolvers\n");
		exit(EXIT_FAILURE);
	}

/*******************************************************************************
														DATA STRUCTURE UPDATE
*******************************************************************************/
	// Instantiate struct containing mutexes and stack
	struct Protector* protector = NULL;
	protector = protectorConstructor(argv, argc, requesters);

	// Initialize arrays of thread pointers
	pthread_t reqThreads[requesters];
	pthread_t resThreads[resolvers];

/*******************************************************************************
													OPEN LOG FILES FOR WRITING
*******************************************************************************/
	FILE* requesterLog = NULL;
	FILE* resolverLog = NULL;
	requesterLog = openWrite(requesterLog, argv[3]);
	resolverLog = openWrite(resolverLog, argv[4]);



	//free the mutex protector struct
	protectorDestructor(protector);

	// Stop timer and output elapsed time
	gettimeofday(&stop, NULL);

	// timeval struct tracks seconds and microseconds, needed to convert them
	// to appropriate units then add them together to get total time elapsed
	double dur = (stop.tv_sec - start.tv_sec) +
		((stop.tv_usec - start.tv_usec) / 1000000.0);
	fprintf(stdout, "./multi-lookup: total time is %lf seconds.\n", dur);

	return 0;

}



/*******************************************************************************
														PROTECTOR DEFINTIONS
*******************************************************************************/

struct Protector* protectorConstructor(char** argv, int argc, int requesters){
	struct Protector* p = (struct Protector*) malloc(sizeof(struct Protector));

	p->stack = stackConstructor();
	p->arguments = argv;
	p->files = argc;
	p->threads = requesters;
	pthread_mutex_init(&p->stackLock, NULL);
	pthread_mutex_init(&p->requesterLock, NULL);
	pthread_mutex_init(&p->requesterLogLock, NULL);
	pthread_mutex_init(&p->errLock, NULL);
	pthread_mutex_init(&p->outputLock, NULL);
	pthread_mutex_init(&p->filesLock, NULL);
	return p;

}

void protectorDestructor(struct Protector* p){
	stackDestructor(p->stack);
	pthread_mutex_destroy(&p->stackLock);
	pthread_mutex_destroy(&p->requesterLock);
	pthread_mutex_destroy(&p->requesterLogLock);
	pthread_mutex_destroy(&p->errLock);
	pthread_mutex_destroy(&p->outputLock);
	pthread_mutex_destroy(&p->filesLock);
	free(p);
}
/*
void* request(FILE* rqLog, FILE* rsLos, Protector* p){

}
*/
