/*
 * Game.h
 *
 *  Created on: Feb 25, 2020
 *      Author: inbal
 */

#ifndef GAME_H_
#define GAME_H_
#include "Board.h"

void solve(FILE * fp);
void edit_file(FILE * fp);
void edit();
void set(int X,int Y,int Z);
char validate(char print);
void guess(float X);
void generate(int X,int Y,int numOfEmptyCells);
void undo();
void redo();
void save(char * X);
void hint(int X,int Y);
void guess_hint(int X,int Y);
void num_solutions();
void autofill();
void reset();
void exitGame();

#endif /* GAME_H_ */
