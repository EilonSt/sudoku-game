/*
 * Implements the board game.
 */

#ifndef CELL_H_
#define CELL_H_
/*every cell in the game board has 3 attributes his value, if he is a fixed cell and if he is an erroneous cell*/
typedef struct Cell{
	int val;
	int fixed;
	int error;
} Cell;
/* struct of sudoku board, every board has a size that represents the number of rows and columns in the board
 * rows in block - represents the number of the rows in each block
 * cols in block - represents the number of the columns in each block
 * cells is a 2 dimensional array of type Cell that stores all the cells of the board */
typedef	struct Board{
	int size;
	int rowsinblock;
	int colsinblock;
	struct Cell ** cells;
} Board;

#endif /* CELL_H_ */
