/*
* Contains all auxiliary functions that are not for a specific module.
*/
#include <stdio.h>
#include <stdlib.h>
#include "Board.h"

extern Board * board;
extern char mark_errors;
extern char mode;
/*prints the current state of the game board*/
void printBoard(){
	int i;
	int j;
	int k;
	int m;
	int z;
	int dash=board->size*4+board->rowsinblock+1; /*number of dashes in each separator row*/
	int rowsinblock=board->rowsinblock;
	int colsinblock=board->colsinblock;
	for(i=0;i<colsinblock;i++){
		for(z=0;z<dash;z++){
		printf("-");
		}
		printf("\n");
		for(m=0;m<rowsinblock;m++){
		for(j=0;j<rowsinblock;j++){
			printf("|");
			for(k=0;k<colsinblock;k++){
				if(board->cells[m+(i*rowsinblock)][k+(colsinblock*j)].val!=0){
				if(board->cells[m+(i*rowsinblock)][k+(colsinblock*j)].fixed!=0){
					printf(" %2d.",board->cells[m+(i*rowsinblock)][k+(colsinblock*j)].val); /*special . for fixed cell*/
				}
				else if(board->cells[m+(i*rowsinblock)][k+(colsinblock*j)].error!=0 && (mode==2||(mark_errors==1))){
					printf(" %2d*",board->cells[m+(i*rowsinblock)][k+(colsinblock*j)].val); /*special asterisk for erroneous cells*/
				}
				else{
					printf(" %2d ",board->cells[m+(i*rowsinblock)][k+(colsinblock*j)].val);
				}
			}
				else{
					printf("    ");
				}
			}
		}
		printf("|\n");
		}
	}
	for(z=0;z<dash;z++){
			printf("-");
			}
			printf("\n");
}


/*
 * determines how to compare between 2 integers a and b. returns 1 if a>b, 0 if a=b, -1 if a<b).
 * used by the qsort function.
 */
int compare(const void* a,const void* b)
{
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}

/*
 * parameters: row and column of a cell.
 * returns an array of the valid digits for the given cell (according to it's neighbour's current digits).
 * the returned array has the number of valid values (x) stored in it's last cell.
 * the first x values in the returned array are the valid digits, sorted.
 */

int * examineValidValue(int row,int column){
	int N=board->size;
	int* validValues;
	int * allValues;
	int i,j,k,l,m,prevrow,prevcol,count=N;
	int rowsinblock=board->rowsinblock;
	int colsinblock=board->colsinblock;
	validValues=(int*)malloc((N+1)*sizeof(int));
	if (validValues==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	allValues=(int*)malloc((N)*sizeof(int));
	if (allValues==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	for(m=0;m<N;m++){
		allValues[m]=m+1;
	}

		for(l=0;l<N;l++){
			validValues[l]=allValues[l];
		}
		validValues[N]=N+2;
		for(i=0;i<N;i++){
			/*checking the cell's row*/
			if(board->cells[row][i].val!=0 && validValues[board->cells[row][i].val-1]!=N+1  &&column!=i){
				count--;
				validValues[board->cells[row][i].val-1]=N+1;
			}
			/*checking the cell's column*/
			if(board->cells[i][column].val!=0 && validValues[board->cells[i][column].val-1]!=N+1 && row!=i){
				count--;
				validValues[board->cells[i][column].val-1]=N+1;
			}
		}
		prevrow=row;
		prevcol=column;
		row=row-(row%rowsinblock);
		column=column-(column%colsinblock);
		/*checking the cell's block*/
		for(j=row;j<row+rowsinblock;j++){
			for(k=column;k<column+colsinblock;k++){
				if(board->cells[j][k].val!=0 && validValues[board->cells[j][k].val-1]!=N+1 &&(j!=prevrow||k!=prevcol)){
					count--;
					validValues[board->cells[j][k].val-1]=N+1;
				}
			}
		}
		qsort(validValues,N,sizeof(int),compare);
		validValues[N]=count;

	free(allValues);
	return validValues;
}
/*Backs up the extern board's cell values in backup board*/
void backupBoardGame(Board * backup){
	int i,j,size=board->size;
	backup->cells=malloc(size*sizeof(Cell*));
	if (backup->cells==NULL){
		printf("Error: failed to allocate memory.\n");
		exit(1);
	}
	for(i=0;i<size;i++){
		backup->cells[i]=malloc(size*sizeof(Cell));
		if (backup->cells[i]==NULL){
			printf("Error: failed to allocate memory.\n");
			exit(1);
		}
	}
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			backup->cells[i][j].val=board->cells[i][j].val;
			backup->cells[i][j].error=board->cells[i][j].error;
			backup->cells[i][j].fixed=board->cells[i][j].fixed;
		}
	}
}
/*Copies the backup board's cell values to the extern game board and frees memory of backup board*/
void restoreBackup(Board * backup){
	int i,j,size=board->size;
	/*restore backup*/
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			board->cells[i][j].val=backup->cells[i][j].val;
			board->cells[i][j].error=backup->cells[i][j].error;
			board->cells[i][j].fixed=backup->cells[i][j].fixed;
		}
	}
	for(i=0;i<size;i++){
		free(backup->cells[i]);
	}
	free(backup->cells);
}


