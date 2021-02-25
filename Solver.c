/*
implementing the exhaustive backtracking algorithm with stack, using deterministic back track algorithm.
the target of this module is to find all the possible solutions to the current game board.
 */
#include <stdio.h>
#include <stdlib.h>
#include "Stack.h"
#include "MainAux.h"
#include "Board.h"

extern Board * board;
/*this function implements the backtracking algorithm with stack*/
int *BackTrack(int row,int col){
	int *cell=(int*)malloc(2*sizeof(int));	/*cell [0]= row   cell [1] = column*/
	int k,i,j,flag=0,flag2=1;
	int * BVV;
	int f;
	struct element* stack;

	if(cell!=NULL){
		cell[0]=row;
		cell[1]=col;
		stack = (struct element*)malloc(sizeof(struct element));
		if (stack==NULL){
			printf("Error: failed to allocate memory.\n");
			exit(1);
		}
		push(cell,&stack);
		while(stack->next!=NULL){
			flag=0;
			flag2=1;
			cell=top(stack);
			row=cell[0];
			col=cell[1];
			pop(&stack);
		do{
			if(col!=0){
				col--;
			}
			else{
				row--;
				col=board->size-1;
			}
			if(row<0){ /*if we try to backtrack from the (0,0) cell*/
					cell[0]=board->size+1;
					cell[1]=board->size+1;
					free(stack);
					return cell;
				}
		} while((board->cells[row][col].fixed==1) && (row!=0 || col!=0));

				BVV=examineValidValue(row,col);
				if(row==0 && col==0 && (board->cells[0][0].val==board->size||board->cells[0][0].fixed==1)){
					board->cells[0][0].val=0;
					cell[0]=board->size+1;
					cell[1]=board->size+1;
					free(BVV);
					free(stack);
					return cell;
				}

				else if(BVV[board->size]!=0){  /* we found a valid cell to change*/
					k=board->cells[row][col].val;
					for(i=0;i<board->size-k;i++){
					board->cells[row][col].val++;
					for(j=0;j<BVV[board->size];j++){
						if(BVV[j]==board->cells[row][col].val){
							flag=1;
							break;
						}
					}
					if(flag){
						break;
					}
					}

			}

				 if(row==0 && col==0 && (board->cells[0][0].val==board->size||board->cells[0][0].fixed==1)){
					 	 	 	 for(f=0;f<BVV[board->size];f++){
					 	 	 		 if(board->size==BVV[f]){
					 	 	 			 flag2=0;
					 	 	 			 break;
					 	 	 		 }
					 	 	 	 }
					 	 	 	 if(flag2){
					 	 	 	 board->cells[0][0].val=0;
					 	 	 	 cell[0]=board->size+1;
					 	 	 	 cell[1]=board->size+1;
					 	 	 	 free(BVV);
					 	 	 	 free(stack);
					 	 	 	 return cell;
					 	 	 	 }
							}
				 if(!flag){
					board->cells[row][col].val=0;
					cell[0]=row;
					cell[1]=col;
					push(cell,&stack);
					free(BVV);
					continue;
				}


			if(col==board->size-1){
						col=0;
						row++;
						}
						else{     /* the cell is not end of row */
							col++;
						}
						cell[0]=row;
						cell[1]=col;
						stack->next=NULL;
						free(BVV);
	}
		free(stack);
	}
	else{
		printf("Error: malloc has failed\n");
		exit(1);
	}
	return cell;
}




/*parameters: row ,column. the function tries to fill the board correctly from the cell that is in (row,column)
 * the function return a pointer to array in size 3 that called last modified cell
 * the last modified cell array saves the last cell that we changed his value during the fill function
 * the last item in the last modified cell array is 1 if we found a legal solution to the board and 0 o/w.*/

int* Fill(int row,int col){
	struct element* stack;
	int*VV;
	int numOfValidNums;
	int BTflag=0;
	int* NextCell;
	int *lastModifiedCell=(int*)malloc(3*sizeof(int));
	if (lastModifiedCell==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	lastModifiedCell[2]=1;
	lastModifiedCell[0]=row;
	lastModifiedCell[1]=col;
	stack = (struct element*)malloc(sizeof(struct element));
	if (stack==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	push(lastModifiedCell,&stack);
	while(stack->next!=NULL){
		lastModifiedCell=top(stack);
		row=lastModifiedCell[0];
		col=lastModifiedCell[1];
		pop(&stack);

	 while((board->cells[row][col].fixed==1)&&((row!=board->size-1 || col!=board->size-1))){
			if(col<board->size-1){
								col++;
					     	}
					else{
								row++;
								col=0;
						}
	 }
	 if((row==board->size-1 && col==board->size-1)&& board->cells[row][col].fixed==1){
		 if(lastModifiedCell[1]==0){
			 lastModifiedCell[0]=lastModifiedCell[0]-1;
		 }
		 else{
			 lastModifiedCell[1]=lastModifiedCell[1]-1;
		 }
		 stack->next=NULL;
		 continue;
	 }

	VV =examineValidValue(row,col);
	numOfValidNums=VV[board->size];

		if(numOfValidNums>0 && board->cells [row][col].val < board->size+1){
			BTflag=0;
			board->cells[row][col].val=VV[0];
			free(VV);
			lastModifiedCell[0]=row;
			lastModifiedCell[1]=col;
			if(row==(board->size-1) && col==(board->size-1)){
							stack->next=NULL;
							continue;

						}
		}
		else{
			free(VV);
			board->cells [row][col].val=0;
			BTflag=1;
			NextCell=BackTrack(row,col);
		}


	 if(BTflag==0){
		 	 	if(col==board->size-1){
		 	 				row++;
							col=0;
				     	}
				else{
							col++;
					}

}
	 else if(BTflag==1){/* BTflag=1 we used backtracking so we will proceed from the cell we fixed*/
		 row=NextCell[0];
		 col=NextCell[1];
		 free(NextCell);
		 if(row==board->size+1 && col==board->size+1){
			 lastModifiedCell[2]=0;
			 stack->next=NULL;
			 continue;
		 }

	 }
	 if(row<board->size-1 || col<board->size-1){
		 if(row<board->size){
			 lastModifiedCell[0]=row;
			 lastModifiedCell[1]=col;
			 push(lastModifiedCell,&stack);
			 continue;
	 }
	 }
	 if(row==(board->size-1) && col==(board->size-1)){
		if((board->cells[board->size-1][board->size-1].fixed==0)){
			VV= examineValidValue(row,col);
			board->cells[row][col].val=VV[0];
			lastModifiedCell[0]=row;
			lastModifiedCell[1]=col;
			free(VV);
		}
		stack->next=NULL;
	 }
	}
	 free(stack);
	 return (lastModifiedCell);

}
/*this function returns the number of solutions that the current game board has. */
int numOfSol(){
	int count=0;
	int sol,row,col,i,j;
	int *lastModifiedCell;
	Board backup;

	backupBoardGame(&backup);
	for(i=0;i<board->size;i++){
		for(j=0;j<board->size;j++){
			if(board->cells[i][j].val!=0){
				board->cells[i][j].fixed=1;
			}
		}
	}
	lastModifiedCell=Fill(0,0);
	sol=lastModifiedCell[2];
	row=lastModifiedCell[0];
	col=lastModifiedCell[1];
	free(lastModifiedCell);
	while(sol){
		count++;
		if(row<0){
				sol=0;
			}
		else{
			board->cells[row][col].val=board->size+1;
			lastModifiedCell=Fill(row,col);
			sol=lastModifiedCell[2];
			row=lastModifiedCell[0];
			col=lastModifiedCell[1];
			free(lastModifiedCell);
		}
	}
	restoreBackup(&backup);
	printf("%d\n",count);
	return count;
}



