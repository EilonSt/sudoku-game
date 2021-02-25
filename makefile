CC = gcc
OBJS = main.o MainAux.o Game.o GameAux.o Gurobi_ILP.o Gurobi_LP.o GurobiAux.o Parser.o ParserAux.o Solver.o Stack.o
EXEC = sudoku-console
COMP_FLAGS = -ansi -O3 -Wall -Wextra -Werror -pedantic-errors
GUROBI_COMP = -I/usr/local/lib/gurobi563/include
GUROBI_LIB = -L/usr/local/lib/gurobi563/lib -lgurobi56

all: $(EXEC)
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(GUROBI_LIB) -o $@ -lm
main.o: main.c MainAux.h Parser.h Board.h LinkedList.h SPBufferset.h
	$(CC) $(COMP_FLAGS) -c $*.c
MainAux.o: MainAux.c MainAux.h Board.h
	$(CC) $(COMP_FLAGS) -c $*.c
Game.o: Game.c Game.h Gurobi_ILP.h Gurobi_LP.h GameAux.h Board.h MainAux.h LinkedList.h Solver.h
	$(CC) $(COMP_FLAGS) -c $*.c
GameAux.o: GameAux.c GameAux.h Board.h GameAux.h LinkedList.h MainAux.h
	$(CC) $(COMP_FLAGS) -c $*.c
Gurobi_ILP.o: Gurobi_ILP.c Gurobi_ILP.h GurobiAux.h Board.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
Gurobi_LP.o: Gurobi_LP.c Gurobi_LP.h Board.h GurobiAux.h
	$(CC) $(COMP_FLAGS) $(GUROBI_COMP) -c $*.c
GurobiAux.o: GurobiAux.c GurobiAux.h Board.h MainAux.h
	$(CC) $(COMP_FLAGS) -c $*.c
Parser.o: Parser.c Parser.h Game.h MainAux.h ParserAux.h Board.h SPBufferset.h
	$(CC) $(COMP_FLAGS) -c $*.c
ParserAux.o: ParserAux.c ParserAux.h Board.h
	$(CC) $(COMP_FLAGS) -c $*.c
Solver.o: Solver.c Solver.h MainAux.h Stack.h Board.h
	$(CC) $(COMP_FLAGS) -c $*.c
Stack.o: Stack.c Stack.h
	$(CC) $(COMP_FLAGS) -c $*.c
clean:
	rm -f *.o $(EXEC)
