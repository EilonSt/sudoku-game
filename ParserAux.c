/*
 * ParserAux.c
 * Contains auxiliary functions used by the Parser module.
 */
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "Board.h"

extern Board * board;

/*returns the number of empty cells in the board game*/
int numberOfEmptyCells(){
	int i,j,count=0;
	int size=board->size;
	for(i=0;i<size;i++){
		for(j=0;j<size;j++){
			if(board->cells[i][j].val==0){
				count++;
			}
		}
	}
	return count;
}

/**returns -1 if not integer, 1 if integer */
short isInteger(char * st){
	int i;
	int stlen=strlen(st);
	for (i=0;i<stlen;i++){
		if (isdigit(st[i])==0)
			return -1;
	}
	return 1;
}

/**returns -1 if not integer, 0 if integer not in range, 1 if integer in range */
short isValidIntegerValue(char * st, int max, int min){
	int num;
	if (isInteger(st)==-1)
		return -1;
	num=atoi(st);
	if (num>max || num<min)
		return 0;
	return 1;
}

/*returns 1 if float, 0 otherwise*/
short isFloat(char * st){
	int i;
	short countdots=0;
	char c;
	int stlen=strlen(st);
	for (i=0;i<stlen;i++){
		c=st[i];
		if (isdigit(c)==0){
			if((c-'.')==0){
				if (countdots>0)
					return 0;
				countdots=1;
			}
			else
				return 0;
		}
	}
	return 1;
}

/*receives a string and returns the float if it is between 0 and 1, -1.0 o/w*/
float isFloatBetween0and1(char * st){
    char *token;
	float f;
	if (isFloat(st)==0)
		return -1.0;
	token = strtok(st, "-");
	sscanf(token,"%f",&f);
	if (f>1.0 || f<0.0)
		return -1.0;
	return f;
}

/*Print's an error when a command that is not valid for the current mode is entered by the user.*/
void printModeError(int modeflag){
	char * mode;
	if (modeflag==2)
		mode="Edit mode";
	else if (modeflag==3)
		mode="Solve mode";
	else if (modeflag==23)
		mode="Edit and Solve modes";
	printf("Error: the command is not available in current mode. It is available in %s only.\n",mode);
}

/*Prints an error when the number of parameters entered after the command name does not match the command's syntax*/
void printParamNumError(short paramflag,char * syntax){
	if (paramflag==-1)
		printf("Error: not enough parameters were entered.\n");
	else
		printf("Error: too many parameters were entered.\n");
	printf("The correct syntax for this command is: %s\n",syntax);
}
