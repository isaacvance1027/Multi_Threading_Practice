#ifndef STACK_H
#define STACK_H
#include "constants.h"

/* instantiate stack data structure for contigious memory */

struct Stack
{
	int head;
	int size;
	
	char* buff[ARRAY_SIZE];
};



/* Function Prototypes */

struct Stack* stackConstructor();


// destructor returns 0 on success, -1 on fail
int stackDestructor(struct Stack* stack);


// stack ops push and pop follow same error return values as destructor
// return new size of stack if operation successfull
int push(struct Stack* stack, char* stackValue);

int pop(struct Stack* stack);


// returns value on top of the stack
char* peek(struct Stack* stack);





#endif
