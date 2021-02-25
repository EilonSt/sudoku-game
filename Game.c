/*
 * Game.c
 * Implements the game's commands. Called by getCommand in Parser.c, after a user entered a correct command.
 *
 */
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "Gurobi_ILP.h"
#include "Gurobi_LP.h"
#include "GameAux.h"
#include "Board.h"
#include "MainAux.h"
#include "LinkedList.h"
#include "Solver.h"
extern char mode;
extern Board * board;
extern LinkedList * list;

/*	Loads a board (only if it is valid) from the provided file path. If the board is valid, mode is changed to solve mode (3).
 *  If the board in the file is invalid (there are not enough parameters\the parameters are not in the correct range\the fixed cells are erroneous..)
 *  prints a message accordingly and stays in the same mode as prior to the command.
 * */
void solve(FILE * fp){
	Board fixedBoard,newBoard;
	char validBoard=0,isValid;
	int i,prevBoardSize;
	newBoard.size=0;
	isValid=isFileFormatValid(fp,&newBoard,&fixedBoard,1);
	if (isValid){
		if (markErroneousCells(&fixedBoard)==1){
			printf("Error: the board provided in the file is erroneous (two or more fixed neighbor cells have the same value).\n"
                    "Please fix the board and try again.\n");
		}
		else{/*board in file is usable*/
			if((list->head) != NULL){
				/*free memory of previous board's undo-redo list*/
				while((list->tail->prev) != NULL){
					deleteNode();
				}
				deleteNode();
			}
			validBoard=1;
			markErroneousCells(&newBoard);
			prevBoardSize=board->size;
			board->rowsinblock=newBoard.rowsinblock;
			board->colsinblock=newBoard.colsinblock;
			/*free memory of previous board*/
			for(i=0;i<prevBoardSize;i++){
				free(board->cells[i]);
			}
			free(board->cells);

			board->cells=(newBoard.cells);
			board->size=newBoard.size;
			mode=3;
			createHead();
			printBoard();
			checkBoardCompleted();
		}
	}
	if(newBoard.size!=0){
		/*We allocated memory for fixedBoard and newBoard in isFileFormatValid*/
		for(i=0;i<fixedBoard.size;i++){
			free(fixedBoard.cells[i]);
		}
		free(fixedBoard.cells);
		if (!validBoard){
			for(i=0;i<newBoard.size;i++){
				free(newBoard.cells[i]);
			}
			free(newBoard.cells);
		}
	}
}

/*	Loads a board (only if it is valid) from the provided file path. If the board is valid, mode is changed to edit mode (2).
 *  If the board in the file is invalid (there are not enough parameters\the parameters are not in the correct range..)
 *  prints a message accordingly and stays in the same mode as prior to the command.
 * */
void edit_file(FILE * fp){
	int prevBoardSize,i,j;
	char isValid;
	Board newBoard;
	Board * nullptr=NULL;
	newBoard.size=0;
	isValid=isFileFormatValid(fp,&newBoard,nullptr,0);
	if (isValid){
		if((list->head) != NULL){
			/*free memory of previous board's undo-redo list*/
			while((list->tail->prev) != NULL){
				deleteNode();
			}
			deleteNode();
		}
		markErroneousCells(&newBoard);
		prevBoardSize=board->size;
		board->rowsinblock=newBoard.rowsinblock;
		board->colsinblock=newBoard.colsinblock;
		/*free memory of previous board*/
		for(i=0;i<prevBoardSize;i++){
			free(board->cells[i]);
		}
		free(board->cells);
		board->cells=newBoard.cells;
		board->size=newBoard.size;
		/*in edit mode, no cells are considered fixed*/
		for(i=0;i<board->size;i++){
			for(j=0;j<board->size;j++){
				board->cells[i][j].fixed=0;
			}
		}
		mode=2;
		createHead();
		printBoard();
	}
	else if(newBoard.size!=0)
	{
		for(i=0;i<newBoard.size;i++){
			free(newBoard.cells[i]);
		}
		free(newBoard.cells);
	}
}

/*Updates the game board to a blank 9x9 board and changes the mode to edit mode (2)*/
void edit(){
	int i,j;
	int prevBoardSize=board->size;
	if((list->head) != NULL){
		/*free memory of previous board's undo-redo list*/
		while((list->tail->prev) != NULL){
			deleteNode();
		}
		deleteNode();
	}
	mode=2;
	/*free memory of previous board*/
	for(i=0;i<prevBoardSize;i++){
		free(board->cells[i]);
	}
	free(board->cells);
	board->size=9;
	board->rowsinblock=3;
	board->colsinblock=3;
	board->cells=malloc(9*sizeof(Cell*));
	if (board->cells==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	for(i=0;i<9;i++){
		board->cells[i]=malloc(9*sizeof(Cell));
		if (board->cells[i]==NULL){
			printf("Error: failed to allocate memory.\n");
			exit(1);
		}
	}
	for(i=0;i<9;i++){
		for(j=0;j<9;j++){
			board->cells[i][j].val=0;
			board->cells[i][j].error=0;
			board->cells[i][j].fixed=0;
		}
	}

		createHead();/*creating new head for the undo-redo list*/
		printBoard();

}

/*Receives 3 parameters in correct range. If the provided cell is fixed, prints and error message, o\w sets it to Z.*/
void set(int X,int Y,int Z){
	int row=Y-1;
	int col=X-1;
	if ((board->cells[row][col].fixed==1)&&(mode==3))
		printf("Error: the provided cell is fixed and can not be updated.\n");
	else{
			board->cells[row][col].val=Z;
			board->cells[row][col].fixed=0;
			markErroneousCells(board);
			printBoard();
			/*free all the memory we allocated for commands to use afterwards if we will choose to use redo function*/
			while((list->tail) != (list->currPtr)){
				deleteNode();
			}
			createNewNode("set");
			if(mode==3){
				checkBoardCompleted();
			}
	}

}

/* checks if board is erroneous or unsolvable or solvable (using Gurobi ILP) and prints message accordingly.
 * returns 1 if the board is solvable, 0 if Erroneous, 2 if unsolvable, -1 if error in gurobi
 * */
char validate(char print){
	char unsolvable,ret=0,gurobires;
	Board backup;
	if (isBoardErroneous(board)){
		ret=0;
	}
	else if(isBoardFull()){
		ret=3;
	}
	else{
		backupBoardGame(&backup);
		unsolvable=fillObviCells(board);/*fills empty cells that have only one legal value. returns 1 if an empty cell with no legal values is found, 0 o\w*/
		if(unsolvable){
			ret=2;
		}
		else if(isBoardFull()){
			/*all empty cells in the board had "obvious" values*/
			ret=1;
		}
		else{
			gurobires=gurobiILP(1);
			if(gurobires==1){/*gurobiILP solved the board*/
				ret=1;
			}
			else if(gurobires==0){/*gurobiILP could not solve the board*/
				ret=2;
			}
			else{/*gurobiILP encountered an error*/
				ret=-1;
			}
		}
		restoreBackup(&backup);
	}
	/*print=1 only when executing the validate command (and not when calling validate from save or from generate)*/
	if(print){
		if(ret==0){
			printBoardErroneous();
		}
		else if(ret==1){
			printf("The board is solvable.\n");
		}
		else if(ret==2){
			printf("The board is unsolvable.\n");
		}
		else if(ret==3){
			printf("The board is solvable - it's current solution is correct.\n");
		}
	}
	if(ret==3){
		ret=1;
	}
	return ret;
}

/*the function gets a number X between 0 to 1
 * checks if the board is solvable using Gurobi LP
 * if we found a solution every empty cell that got at least one legal value with probability greater than X will be filled
 * if there were more than one legal value with probability greater than X than we randomly choose one according to their score
 * we will not fill invalid values along the way but we can possibly made the board unsolvable after the command executed*/
void guess(float X){
	char unsolvable;
	Board backup;
	int i,size;
	size=board->size;
	if (isBoardErroneous(board)){
			printBoardErroneous();
		}
	else{
		backupBoardGame(&backup);
		unsolvable=fillObviCells(board);/*fills empty cells that have only one legal value. returns 1 if an empty cell with no legal values is found, 0 o\w*/
		if(unsolvable){
			printf("Error: the command can not be executed because the board is unsolvable.\n");
			restoreBackup(&backup);
		}
		else{
		markErroneousCells(board);
		if (isBoardErroneous(board)){
			printf("Error: the command can not be executed because the board is unsolvable.\n");
			restoreBackup(&backup);
		}
		else if(isBoardFull()){
			/*free all the memory we allocated for commands to use afterwards if we will choose to use redo function*/
			while((list->tail) != (list->currPtr)){
								deleteNode();
								}
			createNewNode("guess");
			for(i=0;i<size;i++){
				free(backup.cells[i]);
			}
			free(backup.cells);
		}
		else if(gurobiLP(0,0,X,'g')==1){
			/*free all the memory we allocated for commands to use afterwards if we will choose to use redo function*/
			while((list->tail) != (list->currPtr)){
								deleteNode();
								}
			createNewNode("guess");
			for(i=0;i<size;i++){
				free(backup.cells[i]);
			}
			free(backup.cells);
		}
		else{
			restoreBackup(&backup);
		}
		}

		}
	printBoard();
	if(mode==3){
					checkBoardCompleted();
				}
}


/* When calling this function, 0<=X<=numOfEmptyCells, 0<Y<=boardsize^2.
 * The function first checks if the board is erroneous or unsolvable - if it is,
 * prints message that the command can not be executed and exits function.
 * The function tries to randomly fill X random cells, then solve the board with gurobiILP.
 * If it succeeds in less than 1000 tries, the function clears all cells but Y cells which are chosen randomly, and returns the new board.
 * If it fails after 1000 tries, the function prints a message saying the the puzzle generator failed.
 *
 * */
void generate(int X,int Y,int numOfEmptyCells){
	char unsolvable,fillsuccessfully,isSolvable;
	int i=0,j;
	Board backup;
	/*check if the current game board is erroneous or unsolvable. print an error accordingly.*/
	isSolvable=validate(0);
	if(isSolvable==0){
		printBoardErroneous();
	}
	else if(isSolvable==2){
		printf("Error: the command can not be executed because the board is unsolvable.\n");
	}
	else if(isSolvable==1){/*the current game board is solvable*/
		if(isBoardFull()){
			/*The current game board is full. so we only need to clear cells, no need to fill it with ILP*/
			emptyAllButYCells(Y);
		}
		else{
			backupBoardGame(&backup);
			for(i=0;i<1000;i++){/*1000 iterations max*/
				/*randomly choose X empty cells and fill them randomly*/
				fillsuccessfully=fillXRandomEmptyCells(X,numOfEmptyCells);
				if(fillsuccessfully){
					unsolvable=fillObviCells(board);
					if(!unsolvable){
						if(isBoardFull()||gurobiILP(0)){/*the board was filled correctly by gurobi ILP*/
							emptyAllButYCells(Y);
							/*Free memory of backup board cells*/
							for(j=0;j<board->size;j++){
								free(backup.cells[j]);
							}
							free(backup.cells);
							break;
						}
					}
				}
				restoreBackup(&backup);
				backupBoardGame(&backup);
			}
		}
		if(i==1000){/*the function tried 1000 tries with no success*/
			restoreBackup(&backup);
			printf("Error: the puzzle generator failed.\n");
		}
		else{/*generate was executed successfully*/
			printBoard();
			/*add generate to undo-redo list*/
			while((list->tail) != (list->currPtr)){
				deleteNode();
			}
			createNewNode("generate");
		}
	}
}
/*the function undo a previous move done by the user and updates the board correspondingly.
 * the function use the moves list and changes the pointer to point the previous node
 * if we in the initial state of the board we don't do anything and just print to the user the appropriate message*/
void undo(){
	int i,j;
	if(list->currPtr->prev!=NULL){
	list->currPtr=list->currPtr->prev;
	for(i=0;i<board->size;i++){
		for(j=0;j<board->size;j++){
			board->cells[i][j].val=list->currPtr->board->cells[i][j].val;
			board->cells[i][j].fixed=list->currPtr->board->cells[i][j].fixed;
			board->cells[i][j].error=list->currPtr->board->cells[i][j].error;
		}
	}
	for(i=0;i<board->size;i++){
		for(j=0;j<board->size;j++){
			if(list->currPtr->board->cells[i][j].val!=list->currPtr->next->board->cells[i][j].val){
				printf("the cell in row: %d, column: %d has been changed from: %d to: %d\n",
						i+1,j+1,list->currPtr->next->board->cells[i][j].val,list->currPtr->board->cells[i][j].val);
			}
		}
	}
	printBoard();
	}
	else{
		printf("There is no move to undo.\n");
	}

}
/*the function redo a move previously undone by the user and updates the board correspondingly.
 * the function use the moves list and changes the pointer to point the next node*/
void redo(){
	int i,j;
	if(list->currPtr->next!=NULL){
	list->currPtr=list->currPtr->next;
	for(i=0;i<board->size;i++){
		for(j=0;j<board->size;j++){
			board->cells[i][j].val=list->currPtr->board->cells[i][j].val;
			board->cells[i][j].fixed=list->currPtr->board->cells[i][j].fixed;
			board->cells[i][j].error=list->currPtr->board->cells[i][j].error;
		}
	}
	for(i=0;i<board->size;i++){
		for(j=0;j<board->size;j++){
			if(list->currPtr->board->cells[i][j].val!=list->currPtr->prev->board->cells[i][j].val){
				printf("the cell in row: %d, column: %d has been changed from: %d to: %d\n",i+1,j+1,list->currPtr->prev->board->cells[i][j].val,list->currPtr->board->cells[i][j].val);
			}
		}
	}
	printBoard();
	}
	else{
		printf("there is no move to redo.\n");
	}
}

/*Saves the current board to the provided file path X.
 * When in edit mode: (1) validates the board first (if it's unsolvable it can't be saved).
 * (2) If the board is solvable, saves all cells that are not empty as fixed.
 * When in solve mode, saves fixed cells as fixed, unfixed cells as unfixed.
 * */
void save(char * X){
	FILE * fp = NULL;
	int i,j,n,m,cellval,isSolvable=1;
	int boardsize=board->size;
	char * val=(char*)malloc(sizeof(char)*12);
	if (val==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	if (mode==2){ /*Edit mode. Erroneous boards and boards without a solution are not saved*/
		isSolvable=validate(0);
		if(isSolvable==0){
			printBoardErroneous();
		}
		else if(isSolvable==2){
			printf("Error: The board in it's current state is unsolvable and cannot be saved.\n");
		}
	}
	if (isSolvable==1){/*The board is valid for being saved.*/
		fp = fopen(X, "w+");
		if (fp == NULL){
			printf("Error: the command can not be executed because of the following issue with the provided file path: %s\n",strerror(errno));
		}
		else{
			m=board->rowsinblock;
			val=intToString(m,val);
			fputs(val,fp);
			fputs(" ",fp);
			n=board->colsinblock;
			val=intToString(n,val);
			fputs(val,fp);
			fputs("\n",fp);
			for (i=0;i<boardsize;i++){
				for (j=0;j<boardsize;j++){
					cellval=board->cells[i][j].val;
					val=intToString(cellval,val);
					fputs(val,fp);
					if (board->cells[i][j].fixed==1 || (mode==2 && cellval!=0)){
						fputs(".",fp);
					}
					if (j!=boardsize-1)
						fputs(" ",fp);
				}
				fputs("\n",fp);
			}
			if(ferror(fp)!=0){
				printf("Error: failed to write to file.\n");
			}
			fclose(fp);
			printf("The board was saved successfully.\n");
		}
	}
	free(val);
}

/*this function gives the user a hint by showing the solution of a single cell X,Y
 * first we check if the board is erroneous or unsolvable after filling obvious cells values
 * if it is unsolvable we print the appropriate message else we are running the Gurobi ILP optimizer
 * if the Gurobi ILP found a solution the board is solvable and we print the user the value of the cell he asked from the solution
 * else we didn't find even one solution to the current board and we print the appropriate message*/
void hint(int X,int Y){
	char unsolvable;
	Board backup;
	int row=Y-1;
	int col=X-1;
	if (isBoardErroneous(board)){
			printBoardErroneous();
		}
	else if(board->cells[row][col].fixed==1){
		printf("Error: the command can not be executed because the cell is fixed\n");
	}
	else if(board->cells[row][col].val!=0){
		printf("Error: the command can not be executed because the cell is already has a value\n");
	}
	else{
		backupBoardGame(&backup);
		unsolvable=fillObviCells(board);/*fills empty cells that have only one legal value. returns 1 if an empty cell with no legal values is found, 0 o\w*/
		if(unsolvable){
			printf("Error: the command can not be executed because the board is unsolvable.\n");
		}
		else{
			markErroneousCells(board);
			if (isBoardErroneous(board)){
				printf("Error: the command can not be executed because the board is unsolvable.\n");
			}
			else if(board->cells[row][col].val!=0){
				printf("the value for cell <%d,%d> should be: %d\n",Y,X,board->cells[row][col].val);
			}
			else{
				if(gurobiILP(0)){
					printf("the value for cell <%d,%d> should be: %d\n",Y,X,board->cells[row][col].val);
				}
				else{
					printf("Error: the command can not be executed because the board is unsolvable.\n");
				}
			}
	}
		restoreBackup(&backup);
	}
}
/*this function gives the user a hint by showing guess to a single cell X,Y
 * the function gives the user valid values that he can put in the cell and their probabilities
 * first we check if the board is erroneous or unsolvable after filling obvious cells values
 * if it is unsolvable we print the appropriate message else we are running the Gurobi LP optimizer
 * if the Gurobi LP found a solution the board is solvable and we print the user the values of the cell that got a positive probability and their probabilities
 * else we didn't find even one solution to the current board and we print the appropriate message*/
void guess_hint(int X,int Y){
	char unsolvable;
	Board backup;
	int row=Y-1;
	int col=X-1;
	if (isBoardErroneous(board)){
			printBoardErroneous();
		}
	else if(board->cells[row][col].fixed==1){
		printf("Error: the command can not be executed because the cell is fixed.\n");
	}
	else if(board->cells[row][col].val!=0){
		printf("Error: the command can not be executed because the cell already has a value.\n");
	}

	else{
		backupBoardGame(&backup);
		unsolvable=fillObviCells(board);/*fills empty cells that have only one legal value. returns 1 if an empty cell with no legal values is found, 0 o\w*/
		if(unsolvable){
			printf("Error: the command can not be executed because the board is unsolvable.\n");
		}
		else{
			markErroneousCells(board);
			if (isBoardErroneous(board)){
				printf("Error: the command can not be executed because the board is unsolvable.\n");
			}
			else if(board->cells[row][col].val!=0){
				printf("the legal values and their scores for cell <%d,%d> are:\n",Y,X);
				printf("value: %d score: %f\n",board->cells[row][col].val,1.0);
			}
			else{
				gurobiLP(row,col,0.0,'h');
			}
	}
		restoreBackup(&backup);
}
}
/*this command prints the number of solutions that the current game board has using exhaustive backtracking algorithm. */
void num_solutions(){
	if (isBoardErroneous(board)){
		printBoardErroneous();
	}
	else{
		numOfSol();
	}
}
/*this function automatically fill "obvious" values – cells which contain a single legal value.
 * first we check that the board is not erroneous
 * then we go over the board and creating a list of the cells that contain a single legal value
 * afterward we go over the list and fill the board correspondingly to the single legal values of each cell
 * in the end we notify the user the changes that has been done */
void autofill(){
	int j,k,firstCell=1;
	int* ValidValues;
	struct cellToFill{
		int row;
		int col;
		int valueToFill;
		struct cellToFill * next;
	};
	struct cellsList{
		struct cellToFill *head;
		struct cellToFill *ptr;
	}autoFillCells;
	struct cellToFill *autofill;
	struct cellToFill *tmp;
	autoFillCells.head=NULL;
	autoFillCells.ptr=autoFillCells.head;
	if (isBoardErroneous(board)){
			printBoardErroneous();
		}
	else{
	for(j=0;j<board->size;j++){
		for(k=0;k<board->size;k++){
			if(board->cells[j][k].val==0){
			ValidValues=examineValidValue(j,k);
			if(ValidValues[board->size]==1){
				autofill=(struct cellToFill*)malloc(sizeof(struct cellToFill));
				if (autofill==NULL){
					printf("Error: failed to allocate memory.\n");
					exit(1);
				}
				autofill->row=j;
				autofill->col=k;
				autofill->valueToFill=ValidValues[0];
				autofill->next=NULL;
				if(firstCell){
					firstCell=0;
					autoFillCells.head=autofill;
					autoFillCells.ptr=autofill;
				}
				else{
					autoFillCells.ptr->next=autofill;
					autoFillCells.ptr=autofill;
				}
			}
			free(ValidValues);
			}

		}
	}
	if(autoFillCells.head==NULL){
		printf("There are no obvious values to fill.\n");

	}
	else{
	while(autoFillCells.head!=NULL){
		board->cells[autoFillCells.head->row][autoFillCells.head->col].val=autoFillCells.head->valueToFill;
		printf("the cell in row: %d, column: %d has been auto filled with the value: %d\n",
				autoFillCells.head->row+1,autoFillCells.head->col+1,autoFillCells.head->valueToFill);
		tmp=autoFillCells.head->next;
		free(autoFillCells.head);
		autoFillCells.head=tmp;
	}
	markErroneousCells(board);
	printBoard();
	/*free all the memory we allocated for commands to use afterwards if we will choose to use redo function*/
	while((list->tail) != (list->currPtr)){
		deleteNode();
	}
	createNewNode("autofill");
	if(mode==3){
					checkBoardCompleted();
				}
	}
	}
}
/*the functions reset the game board to it's initial state*/
void reset(){
	int i,j,size=board->size;
	if((list->currPtr)==(list->head)){
		printf("There are no moves to reset.\n");
	}
	else{
		list->currPtr=list->head;
		/*change game board to the list's head board*/
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				board->cells[i][j].val=list->head->board->cells[i][j].val;
				board->cells[i][j].error=list->head->board->cells[i][j].error;
				board->cells[i][j].fixed=list->head->board->cells[i][j].fixed;
			}
		}
		printBoard();
	}
}

/*	Called when user enters the "exit" command or when EOF is reached.
 * 	Frees all dynamically allocated memory.
 * 	After it is executed, the getCommand function in Parser.c returns 1 to the main function, and the program ends.
 * */
void exitGame(){
	int i;
	if ((board->cells)!=NULL){
		for(i=0;i<board->size;i++){
			free(board->cells[i]);
		}
		free(board->cells);
	}
	if ((list->tail)!=NULL){
		while((list->tail->prev) != NULL){
			deleteNode();
		}
		deleteNode();
	}
	free(board);
	free(list);
	printf("Exiting...\n");
}

