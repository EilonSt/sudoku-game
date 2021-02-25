/*
 * ParserAux.h
 * Contains auxiliary functions used by the Parser module.
 */

#ifndef PARSERAUX_H_
#define PARSERAUX_H_

int numberOfEmptyCells();
float isFloatBetween0and1(char * st);
short isValidIntegerValue(char * st, int max, int min);
void printModeError(int modeflag);
void printParamNumError(short paramflag,char * syntax);

#endif /* PARSERAUX_H_ */
