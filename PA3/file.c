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


/* ******************************************************** */
//  					   FILE OPERATION FUNCTIONS

// open file for reading
FILE* openRead(FILE* fp, char* file){
	int fileCheck;
	fileCheck = access(file, R_OK);

	if (fileCheck == 0){
		fp = fopen(file, "r");
		return fp;
	}

	fprintf(stderr, "Unable to access file: %s\n", file);

	return fp;
}

// open file for writing
FILE* openWrite(FILE* fp, char* file){
	int fileCheck;
	fileCheck = access(file, W_OK);

	if (fileCheck == 0){
		fp = fopen(file, "w+");
		return fp;
	}

	fprintf(stderr, "Unable to access file: %s\n", file);

	return fp;
}

// read from a file
char* readFile(FILE* fp, char* readValue){

	fgets(readValue, MAX_NAME_LENGTH, fp);
	return readValue;
}

// write to a file
int writeFile(FILE* fp, char* writeValue){

	if (fprintf(fp, "%s", writeValue) >= 0){
		return 0;
	}
	return -1;
}
