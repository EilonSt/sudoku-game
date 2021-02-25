/*
 * Parser.c
 * In charge of parsing the user's input.
 * Parses the input and checks if it is a valid command.
 * If it is, calls the command's function (Game.c).
 * If it isn't, prints an error accordingly.
 */

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "Game.h"
#include "MainAux.h"
#include "ParserAux.h"
#include "Board.h"
#include "SPBufferset.h"
extern char mark_errors;
extern char mode;
extern Board * board;

/*returns 1 if string is blank, 0 otherwise.*/
int stringIsEmpty(const char *s) {
  while (*s != '\0') {
    if (!isspace((unsigned char)*s))
      return 0;
    s++;
  }
  return 1;
}


/**
 * mode: Init=1,Edit=2,Solve=3
 * get's user's input. based on the user's input decides which command to execute in Game.c.
 * Returns 1 if exit command, 0 o\w
 * All "format" validations are done here (correct mode, correct num of params, correct range of params)
 * All "logical" validations are done in Game.c. (checking if the board is erroneous, etc.)
 * */
char getCommand(){
	int numOfEmptyCells;
	char ret=0;
	FILE * fp;
	float f=0;
	char numparams=0;
	short paramflag=0;/*used when there are not enough parameters or too many parameters. -1 if not enough, 1 if too many*/
	char modeflag=0;/*used when trying to call a command that is not valid for the current mode*/
	char rangeflag=0;/*used when one of the parameters is not in the correct range*/
	char filepathflag=0;/*used when there is an error when trying to access the filepath*/
	char * syntax;
	char * paramnum="";
	char * type="integer";
	char rangemin=0;
	int errnum;
	int rangemax;
	char str[258];
	char * command;
	char * X;
	char * Y;
	char * Z;
	char * delims=" \t\r\n";
	int boardsize=board->size;
	SP_BUFF_SET();
	do
	{
		printf("Please enter a command\n");
		if (fgets(str,258,stdin)==NULL){
			if (feof(stdin)){/*EOF*/
				exitGame();
				ret=1;
				return ret;
			}
			else{/*ferror(stdin)*/
				printf("Error: stdin has failed\n");
				exit(1);
			}
		}
	} while (stringIsEmpty(str)==1); /*if blank line, scan again*/
	if (strlen(str)>256){
		printf("Error: too many characters were entered in a single line\n");
		/*clean stdin until reaching line break*/
		while(!strchr( str, '\n' )){
		    if(!fgets(str,258,stdin)){
				if(feof(stdin)){/*EOF*/
					exitGame();
					ret=1;
					return ret;
				}
				else{/*ferror(stdin)*/
					printf("Error: stdin has failed\n");
					exit(1);
				}
		    }
		}
	}
	else{
		/*Splits string into command name and parameters. Counts how many parameters there are.*/
		command = strtok(str,delims);
		X=strtok(NULL,delims);
		if (X==NULL){
			numparams=0;
		}
		else {
			Y=strtok(NULL,delims);
			if (Y==NULL){
				numparams=1;
			}
			else{
				Z=strtok(NULL,delims);
				if (Z==NULL){
					numparams=2;
				}
				else if (strtok(NULL,delims)==NULL){
					numparams=3;
				}
				else
					numparams=4;
			}
		}
		/*Checks which command:*/
		/*SOLVE*/
		if (strcmp(command,"solve")==0){
			if(numparams!=1){
				syntax="solve <file_path> (path can be full or relative)";
				if (numparams<1)
					paramflag=-1;
				else
					paramflag=1;
			}
			else{
				fp = fopen(X, "r+");
				if (fp == NULL){
					filepathflag=1;
					errnum = errno;
				}
				else{
					solve(fp);
					fclose(fp);
				}
			}
		}
		/*EDIT [X]*/
		else if(strcmp(command,"edit")==0){
			if(numparams>1){
				syntax="edit or edit <file_path> (path can be full or relative)";
				paramflag=1;
			}
			/*EDIT X*/
			else if(numparams==1){
				fp = fopen(X, "r+");
				if (fp == NULL){
					filepathflag=1;
					errnum = errno;
				}
				else{
					edit_file(fp);
					fclose(fp);
				}
			}
			/*EDIT*/
			else /*here numparams==0*/
				edit();
		}
		/*MARK_ERRORS*/
		else if(strcmp(command,"mark_errors")==0){
			if (mode!=3)
				modeflag=3;
			else{
				if (numparams!=1){/*wrong number of params*/
					syntax="mark_errors X (where X is 0 or 1)";
					if (numparams==0)
						paramflag=-1;
					else
						paramflag=1;
				}
				else if (strcmp(X,"0")!=0 && strcmp(X,"1")!=0){
					rangeflag=1;
					rangemin=0;
					rangemax=1;
				}
				else
					mark_errors=atoi(X);
			}
		}
		/*PRINT_BOARD*/
		else if(strcmp(command,"print_board")==0){
			if (mode==1)
				modeflag=23;
			else if (numparams!=0){
				syntax="print_board";
				paramflag=1;
			}
			else
				printBoard();
		}
		/*SET*/
		else if(strcmp(command,"set")==0){
			if (mode==1)
				modeflag=23;
			else if (numparams!=3){
				syntax="set <column> <row> <value>";
				if (numparams==4)
					paramflag=1;
				else
					paramflag=-1;
			}
			else{
				if ((isValidIntegerValue(X,boardsize,1)!=1)||(isValidIntegerValue(Y,boardsize,1)!=1)||(isValidIntegerValue(Z,boardsize,0)!=1)){
					rangeflag=1;
					rangemin=1;
					rangemax=board->size;
					if (isValidIntegerValue(X,boardsize,1)!=1)
						paramnum="first ";
					else if (isValidIntegerValue(Y,boardsize,1)!=1)
						paramnum="second ";
					else{
						paramnum="third ";
						rangemin=0;
					}
				}
				else
					set(atoi(X),atoi(Y),atoi(Z));
			}
		}
		/*VALIDATE*/
		else if(strcmp(command,"validate")==0){
			if (mode==1)
				modeflag=23;
			else if (numparams!=0){
				syntax="validate";
				paramflag=1;
			}
			else{
				validate(1);
			}
		}
		/*GUESS*/
		else if(strcmp(command,"guess")==0){
			if (mode!=3)
				modeflag=3;
			else if (numparams!=1){
				syntax="guess <threshold>";
				if (numparams==0)
					paramflag=-1;
				else
					paramflag=1;
			}
			else{
				f=isFloatBetween0and1(X);
				if (f<0){
					rangeflag=1;
					rangemax=1;
				}
				else
					guess(f);
			}
		}
		/*GENERATE*/
		else if(strcmp(command,"generate")==0){
			if (mode!=2)
				modeflag=2;
			else if (numparams!=2){
				syntax="generate <num_cells_to_randomly_fill> <num_cells_to_keep>";
				if (numparams<2)
					paramflag=-1;
				else
					paramflag=1;
			}
			else{
				numOfEmptyCells=numberOfEmptyCells();
				if ((isValidIntegerValue(X,numOfEmptyCells,0)!=1)||(isValidIntegerValue(Y,(boardsize*boardsize),1)!=1)){
					rangeflag=1;
					if (isValidIntegerValue(X,numOfEmptyCells,0)!=1){
						rangemax=numOfEmptyCells;
						paramnum="first ";
					}
					else{
						rangemin=1;
						rangemax=boardsize*boardsize;
						paramnum="second ";
					}

				}
				else
					generate(atoi(X),atoi(Y),numOfEmptyCells);
			}

		}
		/*UNDO*/
		else if(strcmp(command,"undo")==0){
			if (mode==1)
				modeflag=23;
			else if(numparams>0){
				syntax="undo";
				paramflag=1;
			}
			else
				undo();
		}
		/*REDO*/
		else if(strcmp(command,"redo")==0){
			if (mode==1)
				modeflag=23;
			else if(numparams>0){
				syntax="redo";
				paramflag=1;
			}
			else
				redo();
		}
		/*SAVE*/
		else if(strcmp(command,"save")==0){
			if (mode==1)
				modeflag=23;
			else if(numparams!=1){
				syntax="save <file_path> (path can be full or relative)";
				if (numparams<1)
					paramflag=-1;
				else
					paramflag=1;
			}
			else{
				save(X);
			}
		}
		/*HINT*/
		else if(strcmp(command,"hint")==0){
			if (mode!=3)
				modeflag=3;
			else if (numparams!=2){
				syntax="hint <column> <row>";
				if (numparams<2)
					paramflag=-1;
				else
					paramflag=1;
			}
			else if ((isValidIntegerValue(X,boardsize,1)!=1)||(isValidIntegerValue(Y,boardsize,1)!=1)){
				rangeflag=1;
				type="integer";
				rangemin=1;
				rangemax=boardsize;
				if (isValidIntegerValue(X,boardsize,1)!=1)
					paramnum="first ";
				else
					paramnum="second ";
			}
			else
				hint(atoi(X),atoi(Y));
		}
		/*GUESS_HINT*/
		else if(strcmp(command,"guess_hint")==0){
			if (mode!=3)
				modeflag=3;
			else if (numparams!=2){
				syntax="guess_hint <column> <row>";
				if (numparams<2)
					paramflag=-1;
				else
					paramflag=1;
			}
			else if ((isValidIntegerValue(X,boardsize,1)!=1)||(isValidIntegerValue(Y,boardsize,1)!=1)){
				rangeflag=1;
				type="integer";
				rangemin=1;
				rangemax=boardsize;
				if (isValidIntegerValue(X,boardsize,1)!=1)
					paramnum="first ";
				else
					paramnum="second ";
			}
			else
				guess_hint(atoi(X),atoi(Y));
		}
		/*NUM_SOLUTIONS*/
		else if(strcmp(command,"num_solutions")==0){
			if (mode==1)
				modeflag=23;
			else if (numparams>0){
				syntax="num_solutions";
				paramflag=1;
			}
			else
				num_solutions(board);
		}
		/*AUTOFILL*/
		else if(strcmp(command,"autofill")==0){
			if (mode!=3)
				modeflag=3;
			else if (numparams>0){
				syntax="autofill";
				paramflag=1;
			}
			else
				autofill(board);
		}
		/*RESET*/
		else if(strcmp(command,"reset")==0){
			if (mode==1)
				modeflag=23;
			else if (numparams!=0){
				syntax="reset";
				paramflag=1;
			}
			else
				reset();
		}
		/*EXIT*/
		else if(strcmp(command,"exit")==0){
			exitGame();/*terminates the program, all memory resources should be freed, all open files must be closed.*/
			ret=1;
		}
		else
			printf("Error: invalid command\n");
		}
		if (modeflag)
			printModeError(modeflag);
		else if (paramflag!=0)
			printParamNumError(paramflag,syntax);
		else if (rangeflag)
			printf("Error: the %sparameter is not in the correct range.\n It should be a %s between %d and %d.\n",paramnum,type,rangemin,rangemax);
		else if (filepathflag){
			printf("Error: the command can not be executed because of the following issue with the provided file path: %s\n",strerror(errnum));
		}
	return ret;
}

