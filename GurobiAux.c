/*
 * GurobiAux.c
 *	Contains auxiliary functions used by the Gurobi_ILP and Gurobi_LP modules.
 */
#include "Board.h"
#include "MainAux.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
extern Board * board;

/*
 * to use this function, board must have at least one empty cell (it can't be entirely full).
 * validValsBoard has the same dimensions as the game board.
 * Each cell in validValsBoard is "related" to the respective cell in the game board - it hold's the cell's valid values.
 * Each cell in validValsBoard holds a pointer to an array of integers that is set as follows:
 * - Index 0 holds the number of valid values that the cell has.
 * - Odd indices contain valid values of the cell (in rising order).
 * - Even indices (except for 0) contain the variable number for the given valid value. This is used in the constraints.
 * For cells that already have values, the pointer is NULL
 * The function returns the number of variables needed for the Gurobi model
 */
int validValuesBoard(int *** validValsBoard){
	int i,j,k,numOfValidVals,variableNum=0;
	int size=board->size;
	int * VV;
	for(i=0;i<size;i++){
		validValsBoard[i]=(int**)malloc(size*sizeof(int*));
		if (validValsBoard[i]==NULL){
			printf("Error: failed to allocate memory.\n");
			exit(1);
		}
	}
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			/*cell has value*/
			if (board->cells[i][j].val!=0){
				validValsBoard[i][j]=NULL;
			}
			/*cell is empty*/
			else{
				VV=examineValidValue(i,j);
				numOfValidVals=VV[size];
				validValsBoard[i][j]=(int*)malloc((1+numOfValidVals*2)*sizeof(int));
				if (validValsBoard[i][j]==NULL){
					printf("Error: failed to allocate memory.\n");
					exit(1);
				}
				validValsBoard[i][j][0]=numOfValidVals;
				for (k=0;k<numOfValidVals;k++){
					validValsBoard[i][j][2*k+1]=VV[k];
					validValsBoard[i][j][2*k+2]=variableNum;
					variableNum+=1;
				}
				free(VV);
			}
		}
	}
	return (variableNum);
}

/*frees all the memory we allocated in the valid values board function*/
void freeValidValuesBoard(int *** validValsBoard){
	int i,j;
	int size=board->size;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			if (validValsBoard[i][j]!=NULL){/*there was a valid values array created that we need to free*/
				free(validValsBoard[i][j]);
			}
		}
	}
	for(i=0;i<size;i++){
		free(validValsBoard[i]);
	}
	free(validValsBoard);
}
