#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "constants.h"

/* Function Defintions From Prototypes in "stack.h" */

struct Stack* stackConstructor(){
	struct Stack* stack = (struct Stack*) malloc(sizeof(struct Stack));

	stack->head = -1;
	stack->size = 0;

	for (int i=0; i<ARRAY_SIZE; i++){
		stack->buff[i] = NULL;
	}

	printf("Stack Instantiated\n");

	return stack;
}


int stackDestructor(struct Stack* stack){
	free(stack);
	stack = NULL;

	if (stack != NULL){
		printf("Stack destructor failed\n");
		return -1;
	}

	printf("Stack destructor success\n");

	return 0;
}


int push(struct Stack* stack, char* pushValue){
	if (stack->size == ARRAY_SIZE){
		printf("Stack is full\n");
		return -1;
	}

	stack->head++;
	stack->buff[stack->head] = pushValue;
	stack->size++;

	return stack->size;
}


int pop(struct Stack* stack){
	if (stack->size ==0){
		printf("Stack is empty\n");
		return -1;
	}

	stack->buff[stack->head] = NULL;
	stack->head--;
	stack->size--;

	return stack->size;
}

char* peek(struct Stack* stack){
	char* peek = NULL;

	if (stack->head == -1){
		return peek;
	}

	peek = stack->buff[stack->head];

	return peek;
}



/* old test 
int main(void){

	struct Stack* testStack = NULL;
	testStack = stackConstructor();

	// Just checking to see that memset() worked correctly
	//	in stackConstructor()

	//for(int i=0; i<ARRAY_SIZE; i++){
	//	if (testStack->buff[i] == NULL){
	//		printf("YaY\n");
	//	}
	//}



	push(testStack, "Test");

	// test push success
	printf("%s\n", testStack->buff[testStack->head]);
	// test size increment
	printf("%d\n", testStack->size);


	pop(testStack);

	// test pop
	printf("%d\n", testStack->size);

	// test peek
	push(testStack, "peekTest");

	if (peek(testStack) == NULL){
		printf("Peek Error\n");
	}
	else{ printf("%s\n", peek(testStack)); }

	pop(testStack);


	// stack destructor
	stackDestructor(testStack);

	return 0;
}
*/
