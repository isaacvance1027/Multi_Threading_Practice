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


	// Instantiate struct containing mutexes and stack
	struct Protector* protector = NULL;
	protector = malloc(sizeof(struct Protector));
	protector->stack = stackConstructor();

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
		printf(stderr, "Too FEW requesters or resolvers\n");
		exit(EXIT_FAILURE);
	}
	if (requesters > MAX_REQUESTER_THREADS || resolvers > MAX_RESOLVER_THREADS){
		printf(stderr, "Too MANY requesters or resolvers\n");
		exit(EXIT_FAILURE);
	}

/*******************************************************************************
														DATA STRUCTURE UPDATE
*******************************************************************************/





	//free the stack
	stackDestructor(protector->stack);
	//free the mutex protector struct
	free(protector);

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
	struct Protector* p = (struct Protector*) malloc

}
