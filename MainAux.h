/*
 * MainAux.h
 * Contains all auxiliary functions that are not for a specific module.
 */
#ifndef MAINAUX_H_
#define MAINAUX_H_
#include "Board.h"

void printBoard();
int * examineValidValue(int row,int column);
void backupBoardGame(Board * backup);
void restoreBackup(Board * backup);

#endif /* MAINAUX_H_ */
