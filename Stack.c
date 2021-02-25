/* used to implement the backtracking algorithm.
 * the stack is used to simulate recursion.
 */


#include<stdio.h>
#include<stdlib.h>
/* Struct to hold the data and the pointer to the next element.*/
typedef struct element{
	int* data;
    struct element* next;
} element;

/* Append the new element to the start of the stack*/
void push(int* data, struct element** stack){
    struct element* Element = (struct element*)malloc(sizeof(struct element));
	if (Element==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
    Element -> data = data;
    Element -> next = *stack;
    (*stack) = Element;
}
/* Remove element from the top of the stack*/
void pop(struct element** stack){
    if(*stack != NULL){
        struct element* tempPtr = *stack;
        *stack = (*stack) -> next;
        free(tempPtr);
    }
    else{
        printf("The stack is empty.\n");
    }
}
/* Display the element at the top of the stack*/
int* top(struct element* stack){
    if(stack != NULL){
     return stack -> data;
    }
    else{
        printf("The stack is empty.\n");
    }
    return NULL;
}
