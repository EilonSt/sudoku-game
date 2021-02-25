/*
 * Parser.h
 * In charge of parsing the user's input.
 * Parses the input and checks if it is a valid command.
 * If it is, calls the command's function (Game.c).
 * If it isn't, prints an error accordingly.
 */

#ifndef PARSER_H_
#define PARSER_H_
#include "Board.h"

int getCommand();

#endif /* PARSER_H_ */
