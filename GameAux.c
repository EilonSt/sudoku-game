/*
Contains auxiliary functions used by the Game module.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "Board.h"
#include "MainAux.h"
#include "GameAux.h"
#include "LinkedList.h"
extern LinkedList * list;
extern Board * board;
extern char mode;

/* Fills empty cells that have only one legal value and checks if there's an empty cell with no legal values.
 * Stops when all empty cells have at least 2 legal values, or when there is an empty cell with no legal values.
 * if an empty cell with no legal values is found, returns 1, o\w returns 0
 * */
char fillObviCells(Board * board){
	char ret=0;
	char fill=1;
	int i,j,size=board->size;
	int * VV;
	while(fill){
		fill=0;
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				if(board->cells[i][j].val==0){
					VV=examineValidValue(i,j);
					if (VV[size]==1){/*cell has only one legal value*/
						board->cells[i][j].val=VV[0];
						fill=1;
					}
					else if(VV[size]==0){/*cell has no legal value*/
						ret=1;
						fill=0;
						i=size;
						j=size;
					}
					free(VV);
				}
			}
		}
	}
	return ret;
}


/*marks erroneous cells. returns 1 if at least one cell was marked, 0 o\w*/
char markErroneousCells(struct Board * board){
	char flag=0;
	int i,j,cellval,k,p;
	int blockrowmin,blockcolmin,blockrowmax,blockcolmax;
	int bsize=board->size;
	int rowsinblock=board->rowsinblock;
	int colsinblock=board->colsinblock;
	Cell ** cells=board->cells;
	for (i=0;i<bsize;i++){
		for (j=0;j<bsize;j++){
			cells[i][j].error=0;
			cellval=cells[i][j].val;
			if (cellval==0)
				continue;
			for (k=0;k<bsize;k++){
				/*check column*/
				if (cells[k][j].val==cellval && k!=i){
					cells[i][j].error=1;
					flag=1;
					break;
				}
			}
			if (cells[i][j].error==0){
				/*check row*/
				for (k=0;k<bsize;k++){
					if (cells[i][k].val==cellval && k!=j){
						cells[i][j].error=1;
						flag=1;
						break;
					}
				}
			}
			if (cells[i][j].error==0){
				/*check block*/
				blockrowmin=(i / rowsinblock)*rowsinblock;
				blockcolmin=(j / colsinblock)*colsinblock;
				blockrowmax=blockrowmin+rowsinblock;
				blockcolmax=blockcolmin+colsinblock;
				for (k=blockrowmin;k<blockrowmax;k++){
					for (p=blockcolmin;p<blockcolmax;p++){
						if (cells[k][p].val==cellval && (k!=i || p!=j)){
							cells[i][j].error=1;
							flag=1;
							break;
						}
					}
				}
			}
		}
	}
	return flag;
}

/*returns 1 if board is full, 0 o\w*/
char isBoardFull(){
	int i,j;
	int boardsize=board->size;
	for (i=0;i<boardsize;i++){
		for (j=0;j<boardsize;j++){
			if (board->cells[i][j].val==0){
				return 0;
			}
		}
	}
	return 1;
}



/*checks if there is a cell marked as erroneous (meaning two or more neighbor cells have the same value)*/
char isBoardErroneous(struct Board * board){
	int i,j;
	int boardsize=board->size;
	for (i=0;i<boardsize;i++){
		for (j=0;j<boardsize;j++){
			if (board->cells[i][j].error==1){
				return 1;
			}
		}
	}
	return 0;
}


void printBoardErroneous(){
	printf("Error: the command can not be executed because the board is erroneous (two or more neighbor cells have the same value).\n"
	"Please fix the board and try again.\n");
}

/*When in solve mode, this function is called after each change in the board that may have filled the board entirely.
 * If the board is full and correct, a message is printed and the game moves to Init mode.
 * */
void checkBoardCompleted(){
	if(isBoardFull()){
		if(isBoardErroneous(board)){
			printf("The current solution is erroneous.\n");
		}
		else{
			mode=1;
			printf("Good job! The puzzle was successfully solved!\n");
		}
	}
}

/*converts int to string*/
char * intToString(int value, char * result) {
	int base=10;
    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;
    do {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    } while ( value );
    *ptr-- = '\0';
    while(ptr1 < ptr) {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

/* Called by Solve and Edit commands when loading a file.
 * Checks if the file contains a legal board. If it does, saves the board in the file to newBoard.
 * fixedBoard is used if the function is called by Solve - it's used to store only the board's fixed cells. The fixedBoard is later validated in the solve function.
 * returns 1 if the file contains a valid board, 0 otherwise.
 * */
char isFileFormatValid(FILE * fp,struct Board * newBoard,struct Board * fixedBoard,char isSolveCom){
	char c,notEnoughVals=0,incorrectForm=0,notInRange=0,ret=1,fileerror=0;
	int n,m,max,i,j,count=0,cellval;
	/*read block dimensions from file*/
	count=fscanf(fp,"%d %d",&m,&n);
	if (count==EOF){
	    if (ferror(fp)) {
	        fileerror=1;
	    }
	    else {/*EOF*/
	        notEnoughVals=1;
	    }
	}
	else if (count!=2){
		c=fgetc(fp);
		while(isspace(c)!=0){
			c=fgetc(fp);
		}
		if (c==EOF){
			if (ferror(fp)){
				fileerror=1;
			}
			else
				notEnoughVals=1;
		}
		else
			incorrectForm=1;
	}
	else {/*count==2*/
		c=fgetc(fp);
		if (n==0 || m==0){
			printf("The block dimensions provided in the file are invalid. Both must be positive integers.\n");
			ret=0;
		}
		else if (isspace(c)==0){
			if (c==EOF){
				if (ferror(fp)){
					fileerror=1;
				}
				else
					notEnoughVals=1;
			}
			else
				incorrectForm=1;
		}
		else{/*read cell values from file*/
			max=n*m;
			newBoard->size=n*m;
			newBoard->rowsinblock=m;
			newBoard->colsinblock=n;
			newBoard->cells=malloc(max*sizeof(Cell*));
			if (newBoard->cells==NULL){
				printf("Error: failed to allocate memory.\n");
				exit(1);
			}
			for(i=0;i<max;i++){
				newBoard->cells[i]=malloc(max*sizeof(Cell));
				if (newBoard->cells[i]==NULL){
					printf("Error: failed to allocate memory.\n");
					exit(1);
				}
			}
			if (isSolveCom){
				fixedBoard->size=n*m;
				fixedBoard->rowsinblock=m;
				fixedBoard->colsinblock=n;
				fixedBoard->cells=malloc(max*sizeof(Cell*));
				if (fixedBoard->cells==NULL){
					printf("Error: failed to allocate memory.\n");
					exit(1);
				}
				for(i=0;i<max;i++){
					fixedBoard->cells[i]=malloc(max*sizeof(Cell));
					if (fixedBoard->cells[i]==NULL){
						printf("Error: failed to allocate memory.\n");
						exit(1);
					}
				}
			}
			for (i=0;i<max;i++){
				for (j=0;j<max;j++){
					count=fscanf(fp,"%d",&cellval);
					if (count==EOF){
						if (ferror(fp)) {
							fileerror=1;
							i=max;
							break;
						}
						else {/*EOF*/
							notEnoughVals=1;
							i=max;
							break;
						}
					}
					else if (count==0){
						incorrectForm=1;
						i=max;
						break;
					}
					else {/*count==1*/
						c=fgetc(fp);
						if (isspace(c)==0){
							if (c==EOF){
								if (ferror(fp)){
									fileerror=1;
									i=max;
									break;
								}
								else if ((i!=max-1)||(j!=max-1)){
									notEnoughVals=1;
									i=max;
									break;
								}
								else{
									/*value in board's bottom right corner*/
									if (cellval>max || cellval<0){
										notInRange=1;
										i=max;
										break;
									}
									if (isSolveCom){
										fixedBoard->cells[i][j].val=0;
									}
									newBoard->cells[i][j].val=cellval;
									newBoard->cells[i][j].fixed=0;
								}
							}
							else if(c-'.'==0){
								if (cellval==0){
									incorrectForm=1;
									i=max;
									break;
								}
								else{
									c=fgetc(fp);
									if (isspace(c)==0){
										if (c==EOF){
											if (ferror(fp)){
												fileerror=1;
											}
											else if ((i!=max-1)||(j!=max-1)){
												notEnoughVals=1;
											}
											else{
												/*cell in bottom right corner*/
												if (cellval>max || cellval<0){
													notInRange=1;
													i=max;
													break;
												}
												if (isSolveCom)
													fixedBoard->cells[i][j].val=cellval;
												newBoard->cells[i][j].val=cellval;
												newBoard->cells[i][j].fixed=1;
											}
										}
										else
											incorrectForm=1;
										i=max;
										break;
									}
									if (cellval>max || cellval<0){
										notInRange=1;
										i=max;
										break;
									}
									if (isSolveCom)
										fixedBoard->cells[i][j].val=cellval;
									newBoard->cells[i][j].val=cellval;
									newBoard->cells[i][j].fixed=1;
								}
							}
							else{
								incorrectForm=1;
								i=max;
								break;
							}

						}
						else{/*integer value,non fixed*/
							if (cellval>max || cellval<0){
								notInRange=1;
								i=max;
								break;
							}
							if (isSolveCom){
								fixedBoard->cells[i][j].val=0;
							}
							newBoard->cells[i][j].val=cellval;
							newBoard->cells[i][j].fixed=0;
						}
					}
				}
			}
		}
	}
	if (incorrectForm||notEnoughVals||notInRange||fileerror){
		ret=0;
		if (notEnoughVals){
			printf("Error: there aren't enough values provided in the file.\n");
		}
		else if (incorrectForm){
			printf("Error: one or more values in the file is not in the correct form.\n");
		}
		else if (notInRange){
			printf("Error: one or more values in the file are not in the correct range.\n");
		}
		else if(fileerror){
			printf("Error: error while reading from file.\n");
		}
		printf("The file must include 2 positive integers that indicate the board's block dimensions, and integer values for all cells.\n"
		"The cell values must be between 0 and the board size "
		"(0 for empty cells, and '.' at the end of fixed cells).\n");
	}
	else {
		while(c!=EOF && isspace(c)!=0)
			c=fgetc(fp);
		if (c!=EOF){
			printf("Error: there are too many values provided in the file.\n");
			ret=0;
		}
		else if (ferror(fp)){
		        ret=0;
		        printf("Error: error while reading from file.\n");
				printf("The file must include 2 positive integers that indicate the board's block dimensions, and integer values for all cells.\n"
				"The cell values must be between 0 and the board size "
				"(0 for empty cells, and '.' at the end of fixed cells).\n");
		}
	}
	return ret;
}

/*makes a copy of the current board game, which will be stored in a node of the undo-redo list*/
void saveCurrentBoard(){
	int i,j,k;
	Board *current=(Board*)malloc(sizeof(Board));
	if(current==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	current->size=board->size;
	current->rowsinblock=board->rowsinblock;
	current->colsinblock=board->colsinblock;
	current->cells=malloc(current->size*sizeof(Cell*));
	if(current->cells==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	for(i=0;i<current->size;i++){
	current->cells[i]=malloc(current->size*sizeof(Cell));
	if(current->cells[i]==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	}
	for(j=0;j<board->size;j++){
		for(k=0;k<board->size;k++){
			current->cells[j][k].val=board->cells[j][k].val;
			current->cells[j][k].fixed=board->cells[j][k].fixed;
			current->cells[j][k].error=board->cells[j][k].error;
		}
	}
	list->currPtr->board=current;
}

/*delete's the undo-redo list's last node*/
void deleteNode(){
	int i,size;
	size=list->tail->board->size;
	for(i=0;i<size;i++){
		free(list->tail->board->cells[i]);
	}
	free(list->tail->board->cells);
	if(list->tail->prev!=NULL){/*deleted node is not the list's head*/
		list->tail=list->tail->prev;
		free(list->tail->next->board);
		free(list->tail->next);
	}
	else{/*deleted node is the list's head*/
		free(list->tail->board);
		free(list->tail);
	}
}

/*create's a head for the undo-redo list when starting a new board (through the solve or edit commands). The initial board is stored in the head*/
void createHead(){
	Node * newHead=malloc(sizeof(Node));
	if (newHead==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	newHead->next=NULL;
	newHead->prev=NULL;
	newHead->command="initial board";
	list->head=newHead;
	list->tail=newHead;
	list->currPtr=newHead;
	saveCurrentBoard();
}

/*create's new node in the undo-redo list*/
void createNewNode(char * commandName){
	Node * com=(Node*)malloc(sizeof(Node));
	if(com==NULL){
		printf("Error: failed to allocate memory.\n");
				exit(1);
	}
			com->command=commandName;
			list->currPtr->next=com;
			com->prev=list->currPtr;
			com->next=NULL;
			list->currPtr=com;
			saveCurrentBoard();/* saves the current board after we execute the command*/
			list->tail=com;/*updates the list tail*/
}

/*returns an array of the indices of the board's empty cells*/
int * emptyCellIndices(int numOfEmptyCells){
	int i,j,count=0;
	int size=board->size;
	int * emptyCells=(int *)malloc(numOfEmptyCells*sizeof(int));
	if(emptyCells==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			if(board->cells[i][j].val==0){
				emptyCells[count]=(i*size)+j;
				count++;
			}
		}
	}
	return emptyCells;
}

/*returns 1 if the X cells were filled correctly, 0 if one of the cells could not be filled because there were no legal values for it*/
char fillXRandomEmptyCells(int X,int numOfEmptyCells){
	char ret=1;
	int index1,index2;
	int * ValidValues;
	int x,y,i,numOfVVs;
	int size = board->size;
	int * emptyCells=emptyCellIndices(numOfEmptyCells);
	for(i=0;i<X;i++){
		index1=rand()%numOfEmptyCells;
		index2=emptyCells[index1];
		x=index2/size;
		y=index2%size;
		ValidValues=examineValidValue(x,y);
		numOfVVs=ValidValues[size];
		if (numOfVVs == 0){
			ret = 0;
			free(ValidValues);
			break;
		}
		else{
			index2=rand()%numOfVVs;/*randomly choose a valid value by randomly choosing an index in the valid values array*/
			board->cells[x][y].val=ValidValues[index2];
		}
		free(ValidValues);
		emptyCells[index1]=emptyCells[numOfEmptyCells-1];
		numOfEmptyCells--;
	}
	free(emptyCells);
	return ret;
}

/*randomly chooses Y cells to keep, empties all other cells*/
void emptyAllButYCells(int Y){
	int i,j,k,index1,index2;
	int size=board->size;
	int modulu=size*size;
	int numOfCellsToClear=(size*size)-Y;
	int * cells=(int *)malloc(size*size*sizeof(int));
	if(cells==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	for(i=0;i<size*size;i++){
		cells[i]=i;
	}
	for(k=0;k<numOfCellsToClear;k++){
		/*empties cell*/
		index1=rand()%modulu;
		index2=cells[index1];
		i=index2/size;
		j=index2%size;
		board->cells[i][j].val=0;
		board->cells[i][j].fixed=0;
		cells[index1]=cells[modulu-1];
		modulu--;
	}
	free(cells);
}


