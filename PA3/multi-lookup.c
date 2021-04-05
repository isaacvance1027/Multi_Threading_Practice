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
													OPEN LOG FILES FOR WRITING
*******************************************************************************/
	FILE* requesterLog = NULL;
	FILE* resolverLog = NULL;
	requesterLog = openWrite(requesterLog, argv[3]);
	resolverLog = openWrite(resolverLog, argv[4]);


/*******************************************************************************
														DATA STRUCTURE UPDATE
*******************************************************************************/
	// Instantiate struct containing mutexes and stack
	struct Protector* protector = NULL;
	protector = protectorConstructor(argv, argc, requesters);

	// Initialize arrays of thread pointers
	pthread_t reqThreads[requesters];
	pthread_t resThreads[resolvers];

	struct ThreadArg* threadArg = malloc(sizeof(struct ThreadArg));
	threadArg->requestLog = requesterLog;
	threadArg->resolveLog = resolverLog;

/*******************************************************************************
														BEGIN THREAD POOLS
*******************************************************************************/
/*	for (int i = 0; i < requesters; i++) {
	  if (pthread_create(&reqThreads[i], NULL, &requester, (void *) tArgs) != 0)
		{
	      perror("Requester thread pool failed.\n");
	      exit(EXIT_FAILURE);
	  }
	}
*/
/*******************************************************************************
												FREE MEMORY AND END PROGRAM
*******************************************************************************/
	//free the mutex protector struct
	protectorDestructor(protector);

	//free the thread argument struct
	free(threadArg);

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
	pthread_mutex_init(&p->threadLock, NULL);
	pthread_mutex_init(&p->requesterLock, NULL);
	pthread_mutex_init(&p->requesterLogLock, NULL);
	pthread_mutex_init(&p->errLock, NULL);
	pthread_mutex_init(&p->outputLock, NULL);
	pthread_mutex_init(&p->filesLock, NULL);
	sem_init(&p->emptyStack, 0, ARRAY_SIZE);
	sem_init(&p->fullStack, 0, 0);
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
	sem_destroy(&p->emptyStack);
	sem_destroy(&p->fullStack);
	free(p);
}



/*******************************************************************************
										REQUESTER AND RESOLVER DEFINTIONS
*******************************************************************************/
// Both the request and resovle functions

void* request(void* arg){

	// parse void argument struct
	FILE* requesterLog = ((struct ThreadArg *)arg)->requestLog;
	FILE* inputFile = NULL;
	char* fileName = NULL;
	char* domain = NULL;
	int domainSize = 0;
	int numOfFiles = 0;
	int whichFile = 0;

	struct Protector* protector = ((struct ThreadArg *)arg)->p;

	// initialize memory to store domain name within
	char name[MAX_NAME_LENGTH];

	// synchronize access to the Protector
	while(1){
		// lock access to protector for determining which file
		pthread_mutex_lock(&protector->filesLock);
			if (protector->whichFile < protector->files){
				whichFile = protector->whichFile;
				protector->whichFile = protector->whichFile + 1;
			}else{
				whichFile = 0;
			}
		pthread_mutex_unlock(&protector->filesLock);

		// if the if else indicates that we are at the end of the files,
		// break to the end of the request
		if (whichFile == 0){
			break;
		}

		// we know which file to get from the shared buffer now, so we open in in
		// read mode using function from file.h
		fileName = protector->arguments[whichFile];
		inputFile = openRead(inputFile, fileName);

		// check for file validity
		if (inputFile != NULL){
			while(fgets(name, MAX_NAME_LENGTH, inputFile) != NULL){

				// strncpy() auto mallocs, remember the TODO in resolve to free the
				// space that has been malloced.... resolve needs to be able to access
				// domain memory space because threads share heap
				strncpy(domain, name, MAX_NAME_LENGTH);
				domainSize = strlen(domain);
				if (domain[domainSize - 1] == '\n'){
					// lock mutex for writing to request log
					pthread_mutex_lock(&protector->requesterLogLock);
						fprintf(requesterLog, "%s", domain);
					pthread_mutex_unlock(&protector->requesterLogLock);
					domain[domainSize - 1] = '\0';
					// use of semaphore with mutexes to implment a push onto the stack
					// had a push function that wasnt thread safe, refactored here.
					// TODO:: get rid of unnecessary functions from old iterations
					sem_wait(&protector->emptyStack);
						pthread_mutex_lock(&protector->stackLock);
							protector->stack->head++;
							protector->stack->size++;
							protector->stack->buff[protector->stack->head] = domain;
						pthread_mutex_unlock(&protector->stackLock);
					sem_post(&protector->fullStack);
				}
				else {
					// error message for out of bounds files
					pthread_mutex_lock(&protector->errLock);
						fprintf(stderr, "Host: %s out of bounds.\n", domain);
					pthread_mutex_unlock(&protector->errLock);
				}
				numOfFiles = numOfFiles + 1;
			}
		}
		else{
			// Display error message if thread is unable to access file
			pthread_mutex_lock(&protector->errLock);
				fprintf(stderr, "File: %s invalid\n", fileName);
			pthread_mutex_unlock(&protector->errLock);
		}
		fclose(inputFile);
	}

	// output success message to display number of services performed by thread
	pthread_t tid = pthread_self();
	pthread_mutex_lock(&protector->outputLock);
		fprintf(stdout, "Thread # %lu serviced %d files\n", tid, numOfFiles);
	pthread_mutex_unlock(&protector->outputLock);

	// Update the thread count after requester thread has finished a request
	pthread_mutex_lock(&protector->threadLock);
		protector->threads = protector->threads - 1;
	pthread_mutex_unlock(&protector->threadLock);

	return NULL;
}


void* resolve(void* arg){
	// TODO:: FREE DOMAIN ALLOCATED BY STRNCPY

	// parse through threadArg struct as in request()
	// and Instantiate necessary variables on threads stack
	FILE* resolverLog = ((struct ThreadArg *)arg)->resolveLog;
	char* address = NULL;
	char* domain = NULL;
	int numOfFiles = 0;
	int success = -1;
	int checkThreads = 0;

	struct Protector* protector = ((struct ThreadArg *)arg)->p;

	// allocate memory for address pointer to account for maximum possible size
	address = (char*)malloc(sizeof(char)*MAX_IP_LENGTH);

	// begin synchronization of resolver threads in resolver pool
	while(1){
		// need to block access to threadcount to check boundary condition
		pthread_mutex_lock(&protector->threadLock);
			checkThreads = protector->threads;
		pthread_mutex_unlock(&protector->threadLock);

		// need to block access to stack head to see if stack is empty
		pthread_mutex_lock(&protector->stackLock);
			if ((protector->stack->head == -1) && (checkThreads == 0)){
				pthread_mutex_unlock(&protector->stackLock);
				break;
			}
			else{
				pthread_mutex_unlock(&protector->stackLock);
			}


	}

}
