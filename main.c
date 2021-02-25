
#include "MainAux.h"
#include "Parser.h"
#include "Board.h"
#include "LinkedList.h"
#include "SPBufferset.h"
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

char mark_errors=1; /*the mark errors parameter*/
char mode=1; /*1 is Init, 2 is Edit, 3 is Solve*/
Board * board;
LinkedList * list;

/*implementation of Sudoku game. Board * board is the current game board, LinkedList * list is the game board's undo\redo list*/
int main()
{
	char exitcom=0;
	srand(time(NULL));
	printf("WELCOME TO SUDOKU.\n");
	printf("Created by Eilon Storzi and Inbal Cohen.\n");
	list=(LinkedList*)malloc(sizeof(LinkedList));
	if (list==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	board=(Board*)malloc(sizeof(Board));
	if (board==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	/*initializing the game board and the undo/redo list*/
	list->head=NULL;
	list->tail=NULL;
	list->currPtr=NULL;
	board->size=0;
	board->rowsinblock=0;
	board->colsinblock=0;
	board->cells=NULL;
	while (!exitcom){
		exitcom=getCommand();
	}
    return 0;
}

