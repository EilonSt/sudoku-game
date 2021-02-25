/* used to implement the backtracking algorithm.
 * the stack is used to simulate recursion.
 */

#ifndef STACK_H_
#define STACK_H_

typedef struct element{
	int* data;
    struct element* next;
}element;

void push(int* data, struct element** stack);
void pop(struct element** stack);
int* top(struct element* stack);
#endif /* STACK_H_ */
