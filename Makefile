sudoku: main.cpp
	g++ -std=c++11 -O3 -DNDEBUG -fopenmp src/main.cpp -o sudoku

clean: rm sudoku

.PHONY: all clean
