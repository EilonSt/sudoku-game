/*used for implementing the undo-redo list
 * we build a doubly linked list*/

#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_
#include "Board.h"
/*every node in the undo/redo list contain 4 attributes.
 * we use doubly linked list so every node has pointer to the next node in the list
 * also every node has a pointer to the previous node in the list
 * every node has a string that describes wich command created it
 * every node saves board that is exactly like the game board after the command executed successfully*/
typedef struct Node{
	struct Node * next;
	struct Node * prev;
	char * command;
	Board *board;
}Node;
/*the list has maintain 3 pointers, pointer to the head of the list witch saves the initial board state,
 * pointer to the tail of the list- the last command that changed the board state,
 * and pointer that tells us where are we in the undo/redo list so we can know if we can execute undo or redo command
 * or if we need to delete nodes from the list for example if we do a couple of undo moves and than use a command that changes the board like set command*/
typedef struct LinkedList{
	Node * currPtr;
	Node * head;
	Node * tail;
} LinkedList;

#endif /* LINKEDLIST_H_ */
