/*
 * GameAux.h
 * Contains auxiliary functions used by the Game module.
 */

#ifndef GAMEAUX_H_
#define GAMEAUX_H_
#include "Board.h"

void emptyAllButYCells(int Y);
char fillObviCells(Board * board);
char isFileFormatValid(FILE * fp,struct Board * newBoard,struct Board * fixedBoard,char isSolveCom);
char isBoardErroneous(struct Board * board);
void printBoardErroneous();
char markErroneousCells(struct Board * board);
char * intToString(int value, char * result);
char isBoardFull();
void saveCurrentBoard();
void deleteNode();
void createHead();
void createNewNode(char * commandName);
void checkBoardCompleted();
char fillXRandomEmptyCells(int X,int numOfEmptyCells);
#endif /* GAMEAUX_H_ */
