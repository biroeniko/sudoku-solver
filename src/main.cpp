/* Copyright (C) 2016 Biro Eniko - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the license.
 * This file is subject to the terms and conditions defined in
 * file 'LICENSE.txt', which is part of this source code package.
 */

#include <iostream>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

static const int nMax = 3;
static const int howManyMax = nMax*nMax;
static int n;
static int howMany;
static int howManyMinus1;

int found = 0;
int sudoku[howManyMax][howManyMax] = {{0}};
bool maybe[howManyMax][howManyMax][howManyMax] = {{{false}}};            // save how many solutions there could be
int howManyPossible[howManyMax][howManyMax] = {{0}};
bool findMore = false;

bool generateSudoku = false;
bool canPrint = true;
bool singleSolution = false;
int difficulty = 46;

FILE* out;

// for solving the sudoku
struct Filler
{
    bool numbers[howManyMax];

    Filler()
    {
        reset();
    }

    Filler(const Filler& f)
    {
        for (int i = 0; i < howMany; i++)
        {
            numbers[i] = f.numbers[i];
        }
    }

    void operator = (const Filler& f)
    {
        for (int i = 0; i < howMany; i++)
        {
            numbers[i] = f.numbers[i];
        }
    }

    void reset()
    {
        for (int i = 0; i < howMany; i++)
            numbers[i] = false;
    }

    bool add(int x)
    {
        if (numbers[x-1])
            return false;
        numbers[x-1] = true;
        return true;
    }

    void del(int x)
    {
        numbers[x-1] = false;
    }

    // if this number wasn't so far, it is ok
    bool isOK(int x)
    {
        return !numbers[x-1];
    }
};

Filler row[howManyMax];
Filler column[howManyMax];
Filler box[howManyMax];

// initialize to zero
void setToZero()
{
    found = 0;

    for (int i = 0; i < howMany; i++)
        row[i].reset();
    for (int i = 0; i < howMany; i++)
        column[i].reset();
    for (int i = 0; i < howMany; i++)
        box[i].reset();

}


// print matrix
void printMatrix()
{
    for (int i = 0; i < howMany; i++)
    {
        if (i % n == 0 && i != 0)
        {
            for (int j = 0; j < howMany+n-1; j++)
            {
                if ((j+1) % (n+1) == 0 && j != 0)
                    fprintf(out, "+ ");
                else
                    fprintf(out, "— ");
            }
            fprintf(out, "\n");
        }

        for (int j = 0; j < howMany; j++)
        {
            if (j % n == 0 && j != 0)
                fprintf(out, "| %d ", sudoku[i][j]);
            else
                fprintf(out, "%d ", sudoku[i][j]);
        }
        fprintf(out, "\n");
    }
    fprintf(out, "\n");
}


/*
void printMatrix()
{
    for (int i = 0; i < howMany; i++)
    {
        if (i % n == 0 && i != 0)
        {
            for (int j = 0; j < howMany+n-1; j++)
            {
                if ((j+1) % (n+1) == 0 && j != 0)
                    printf("+ ");
                else
                    printf("— ");
            }
            printf("\n");
        }

        for (int j = 0; j < howMany; j++)
        {
            if (j % n == 0 && j != 0)
                printf("| %d ", sudoku[i][j]);
            else
                printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}
*/

// for generating the 2x2 sudokus
struct Checker
{
    bool numbers[howManyMax];

    Checker()
    {
        reset();
    }

    void reset()
    {
        for (int i = 0; i < howMany; i++)
            numbers[i] = false;
    }

    void add(int x)
    {
        numbers[x-1] = true;
    }

    bool check()
    {
        for (int i = 0; i < howMany; i++)
        {
            if (!numbers[i])
                return false;
        }
        return true;
    }
};

// checks rows/columns/boxes
// calls for the proper rows, columns and boxes the appropriate checker methods
bool checkFunction(int i, int j)
{
    Checker c;

    if (i == howManyMinus1)             // we are done with a column
    {
        for (int k = 0; k < howMany; k++)
        {
            c.add(sudoku[k][j]);
        }
        return c.check();
    }
    if (j == howManyMinus1)             // we are done with a row
    {
        for (int k = 0; k < howMany; k++)
        {
            c.add(sudoku[i][k]);
        }
        return c.check();
    }
    if ( !((i+1)%n) && !((j+1)%n) )     // we are done with a box
    {
        for (int k = i - (n-1); k <= i; k++)
        {
            for (int l = j - (n-1); l <= j; l++)
            {
                c.add(sudoku[k][l]);
            }
        }
        return c.check();
    }
    return true;
}

// generate the 2x2 sudoku - backtrack
void generate(int i, int j)
{
    bool ok = false;
    if (i == howMany && j == 0)             // if the last row is done and is ok
    {
        found++;
        printMatrix();
    }
    else if (j == howMany)                  // a whole row is done and is ok
    {
        generate(i+1, 0);
    }
    else
    {
        for (int k = 1; k <= howMany; k++)
        {
            sudoku[i][j] = k;
            ok = checkFunction(i, j);
            if (ok)
                generate(i,j+1);
        }
    }
}

// add number to filler
bool addToFillers(int i, int j, int number)
{
    if (!row[i].add(number) || !column[j].add(number) || !box[(i/3)*3 + (j/3)].add(number))
        return false; // error occurred
    return true;
}

// remove number from the filler
void removeFromFillers(int i, int j, int number)
{
    row[i].del(number);
    column[j].del(number);
    box[(i/3)*3 + (j/3)].del(number);
}

// initialize filler
bool initializeFillers()
{
    int number;

    for (int i = 0; i < howMany; i++)
    {
        for (int j = 0; j < howMany; j++)
        {
            if ((number = sudoku[i][j]) != 0)         // if this field is already filled
            {
                if (!addToFillers(i, j, number))
                    return false;
            }
        }
    }

    return true;
}

// initialize the characteristic 3 dimensional array
bool initializeCharacteristic()
{
    for (int i = 0; i < howMany; i++)
    {
        for (int j = 0; j < howMany; j++)
        {
            // set the howManyPossible and maybe matrices to zero
            howManyPossible[i][j] = 0;
            for (int k = 0; k < howMany; k++)
                maybe[i][j][k] = false;

            // if the field is already filled, we skip it
            if (sudoku[i][j] != 0)
                continue;

            // we check and save the possible values
            for (int k = 0; k < howMany; k++)
            {
                const int number = k+1;
                if (row[i].isOK(number) && column[j].isOK(number) && box[(i/3)*3 + (j/3)].isOK(number))
                {
                    maybe[i][j][k] = true;
                    howManyPossible[i][j]++;
                }
            }
        }
    }
    return true;
}

void backTrack(int i, int j);

// logical solution: is there are fields where only one number is possible, we fill this field immediately
void solveLogical()
{
    while (true)
    {
        bool wasUpdated = false;

        for (int i = 0; i < howMany; i++)
        {
            for (int j = 0; j < howMany; j++)
            {
                if (howManyPossible[i][j] == 1)
                {
                    for (int k = 0; k < howMany; k++)
                    {
                        if (maybe[i][j][k])
                        {
                            int number = k+1;
                            if (row[i].isOK(number) && column[j].isOK(number) && box[(i/3)*3 + (j/3)].isOK(number))
                            {
                                sudoku[i][j] = number;
                                addToFillers(i, j, number);
                                wasUpdated = true;
                                break;
                            }
                        }
                    }
                }
            }
        }

        // if there are no more fields which can be solved logically, we continue with backtracking
        if (!wasUpdated)
        {
            break;
        }
        else
        {
            // if there was, then we count again the possibilities
            initializeCharacteristic();
        }
    }

    // check if we are in a dead-end: if there are a field which is not yet filled, but there are 0 possibilities
    for (int i = 0; i < howMany; i++)
    {
        for (int j = 0; j < howMany; j++)
        {
            if (sudoku[i][j] == 0 && howManyPossible[i][j] == 0)
            {
                return;
            }
        }
    }

    // the logical solver is over, we search for the place where the fewest possibilities are -> we try with backtracking here
    int min = howManyPossible[0][0];

    if (min == 0)
    {
        for (int i = 0; i < howMany; i++)
        {
            for (int j = 1; j < howMany; j++)
            {
                if (howManyPossible[i][j] != 0)
                {
                    min = howManyPossible[i][j];
                    break;
                }
            }
            if (min != 0)
                break;
        }
    }

    // everything is solved - minimum is 0
    if (min == 0)
    {
        found++;
        if (found == 1)
        {
            if (canPrint)
            {
                printMatrix();
            }
        }
    }
    else
    {
        // we find the field with the fewest possibilities
        int mini = 0;
        int minj = 0;
        for (int k = 0; k < howMany; k++)
        {
            for (int l = 0; l < howMany; l++)
            {
                // if we find a lesser number, we change the minimum
                if (min >= howManyPossible[k][l] && howManyPossible[k][l] != 0)
                {
                    mini = k;
                    minj = l;
                    min = howManyPossible[k][l];
                }
            }
        }

        backTrack(mini, minj);
    }
}

void backTrack(int i, int j)
{
    if (found > 1 || (!findMore && found == 1))            // if we know that there are more solutions, or we already have one and we only want one, then we stop
        return;

    int saved[howMany][howMany];

    for (int i = 0; i < howMany; i++)
    {
        for (int j = 0; j < howMany; j++)
        {
            saved[i][j] = sudoku[i][j];
        }
    }

    Filler savedRow[howManyMax];
    Filler savedColum[howManyMax];
    Filler savedBox[howManyMax];

    for (int k = 0; k < howMany; k++)
    {
        savedRow[k] = row[k];
        savedColum[k] = column[k];
        savedBox[k] = box[k];
    }

    bool theSudokuHasChanged = false;

    // we try the numbers for this field
    for (int k = 0; k < howMany; k++)
    {
        if (theSudokuHasChanged)
        {
            for (int i = 0; i < howMany; i++)
            {
                for (int j = 0; j < howMany; j++)
                {
                    sudoku[i][j] = saved[i][j];
                }
            }

            for (int k = 0; k < howMany; k++)
            {
                row[k] = savedRow[k];
                column[k] = savedColum[k];
                box[k] = savedBox[k];
            }

            theSudokuHasChanged = false;
        }
        int number = k+1;
        if (row[i].isOK(number) && column[j].isOK(number) && box[(i/3)*3 + (j/3)].isOK(number))
        {
            theSudokuHasChanged = true;
            sudoku[i][j] = number;
            addToFillers(i, j, number);
            initializeCharacteristic();
                solveLogical();
        }
    }
}

// sudoku solver: first logically, then with backtracking
void solve()
{
    solveLogical();
}

// simply remove a number of fields and solve the sudoku
bool simplyremove()
{
    int x, y;
    // easy: < 35
    // medium: < 55
    // hard: >= 55

    // remove a number of filled fields
    for (int i = 0; i < difficulty; i++)
    {
        x = rand()% howMany;
        y = rand()% howMany;
        if (sudoku[x][y] == 0)
        {
            i--;
            continue;
        }
        sudoku[x][y] = 0;
    }

    // save the sudoku
    int savedSudoku[howMany][howMany];
    for (int i = 0; i < howMany; i++)
    {
        for (int j = 0; j < howMany; j++)
        {
            savedSudoku[i][j] = sudoku[i][j];
        }
    }

    // solve the sudoku
    struct timespec start, finish;
    double elapsed;
    clock_gettime(CLOCK_MONOTONIC, &start);

    setToZero();
    findMore = true;
    canPrint = false;
    bool sudokuIsOK = initializeFillers();
    initializeCharacteristic();

    if (!sudokuIsOK)
    {
        return false;
    }
    else
    {
        solve();
    }
    if (findMore)
    {
        if (found > 1)
        {
            return false;
        }
        else if (found == 1)
        {
            printf("The sudoku has a single solution\n");
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    elapsed = (finish.tv_sec - start.tv_sec);
    elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("The sudoku was solved in %f seconds\n\n", elapsed);

    for (int i = 0; i < howMany; i++)
    {
        for (int j = 0; j < howMany; j++)
        {
            sudoku[i][j] = savedSudoku[i][j];
        }
    }

    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("usage: sudoku n [other options]\n");
        exit(1);
    }

    n = atoi(argv[1]);
    howMany = n*n;
    howManyMinus1 = n*n-1;

    out = fopen("output.txt", "wt");
    if (out == NULL)
    {
        printf("Failed to open output file\n");
        return 1;
    }

    if (n == 2)
    {
        // we generate all possible 2x2 solutions
        generate(0,0);
        printf("%d\n", found);
    }
    else if (n == 3)                // solve/generate the sudoku
    {
        if (argc < 3)
        {
            printf("usage: sudoku 3 filenameToSolve|generate [single_sol|multi_sol]\n");
            exit(1);
        }

        generateSudoku = strcmp(argv[2], "generate") == 0;

        // SOLVE THE SUDOKU
        if (!generateSudoku)
        {
            if (argc < 4)
            {
               printf("usage: sudoku 3 filenameToSolve single_sol|multi_sol\n");
               exit(1);
            }

            if (strcmp(argv[3], "single_sol") == 0)
                findMore = false;
            else if (strcmp(argv[3], "multi_sol") == 0)
                findMore = true;
            else
            {
                printf("invalid argument: %s\n", argv[3]);
                exit(1);
            }

            int index = 1;
            double totalTime = 0;

            FILE* file;
            file = fopen(argv[2], "rt");
            if (file == NULL)
            {
                printf("Failed to open file\n");
                return 1;
            }
            char c;

            while (!feof(file))
            {
                for (int i = 0; i < howMany; i++)
                {
                    for (int j = 0; j < howMany; j++)
                    {
                        c = getc(file);
                        if (c == '\n' || c == '\r')
                        {
                            j--;
                            continue;
                        }
                        sudoku[i][j] = (c == '.') ? 0 : c-'0';
                    }
                }

                if (feof(file))
                {
                    break;
                }
                c = getc(file);

                struct timespec start, finish;
                double elapsed;
                clock_gettime(CLOCK_MONOTONIC, &start);

                setToZero();
                bool sudokuIsOK = initializeFillers();
                initializeCharacteristic();

                if (!sudokuIsOK)
                {
                    printf("The %d. sudoku has no solution\n", index);
                }
                else
                {
                    solve();
                }
                if (findMore && sudokuIsOK)
                {
                    if (found > 1)
                        printf("The %d. sudoku has multiple solutions\n", index);
                    else if (found == 1)
                        printf("The %d. sudoku has a single solution\n", index);
                }
                clock_gettime(CLOCK_MONOTONIC, &finish);
                elapsed = (finish.tv_sec - start.tv_sec);
                elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
                printf("The %d. sudoku was solved in %f seconds\n\n", index++, elapsed);
                totalTime += elapsed;
            }
            printf("Total time: %f s \n", totalTime);
            fclose(file);
        }
        else
        {
            // GENERATE SUDOKU
            if (argc == 4)
            {
                if (strcmp(argv[3], "easy") == 0)
                    difficulty = 35;
                else if (strcmp(argv[3], "medium") == 0)
                    difficulty = 50;
                else if (strcmp(argv[3], "hard") == 0)
                    difficulty = 55;
            }

            for (int i = 0; i < n*n; i++)
            {
                for (int j = 0; j < n*n; j++)
                {
                    sudoku[i][j] = (i*n + i/n + j) % (n*n) + 1;
                }
            }

            int timesRandomized = 5000;
            srand(time(NULL));
            int x, y, temp;
            // randomize those rows and columns which are in the same box
            for (int i = 0; i < timesRandomized; i++)
            {
                for (int k = 0; k < 3; k++)
                {
                    if (i%4 == 1)                                    // randomize rows
                    {
                        x = (int)(k*3 + (rand()%n));
                        y = (int)(k*3 + (rand()%n));
                        for (int j = 0; j < howMany; j++)
                        {
                            temp = sudoku[x][j];
                            sudoku[x][j] = sudoku[y][j];
                            sudoku[y][j] = temp;
                        }
                    }
                    else if (i%4 == 2)                               // randomize columns
                    {
                        x = (int)(k*3 + (rand()%n));
                        y = (int)(k*3 + (rand()%n));
                        for (int j = 0; j < howMany; j++)
                        {
                            temp = sudoku[j][x];
                            sudoku[j][x] = sudoku[j][y];
                            sudoku[j][y] = temp;
                        }
                    }
                    else if (i%4 == 3)                               // randomize rows by three
                    {
                        x = (int)(rand()%n);
                        y = (int)(rand()%n);
                        for (int k = 0; k < 3; k++)
                        {
                            for (int j = 0; j < howMany; j++)
                            {
                                temp = sudoku[x*3+k][j];
                                sudoku[x*3+k][j] = sudoku[y*3+k][j];
                                sudoku[y*3+k][j] = temp;
                            }
                        }
                    }
                    else
                    {
                        x = (int)(rand()%n);
                        y = (int)(rand()%n);
                        for (int k = 0; k < 3; k++)
                        {
                            for (int j = 0; j < howMany; j++)
                            {
                                temp = sudoku[j][x*3+k];
                                sudoku[j][x*3+k] = sudoku[j][y*3+k];
                                sudoku[j][y*3+k] = temp;
                            }
                        }
                    }
                }
            }

            // remove numbers while the sudoku is single-solutioned
            struct timespec start, finish;
            double elapsed;
            clock_gettime(CLOCK_MONOTONIC, &start);

            while (!singleSolution)
            {
                // save the sudoku
                int savedSudoku[howMany][howMany];
                for (int i = 0; i < howMany; i++)
                {
                    for (int j = 0; j < howMany; j++)
                    {
                        savedSudoku[i][j] = sudoku[i][j];
                    }
                }

                singleSolution = simplyremove();

                if (!singleSolution)
                {
                    for (int i = 0; i < howMany; i++)
                    {
                        for (int j = 0; j < howMany; j++)
                        {
                            sudoku[i][j] = savedSudoku[i][j];
                        }
                    }
                }
            }

            if (difficulty <= 35)
                printf("The sudoku is easy\n");
            else if (difficulty < 55)
                printf("The sudoku is medium\n");
            else
                printf("The sudoku is hard\n");

            clock_gettime(CLOCK_MONOTONIC, &finish);
            elapsed = (finish.tv_sec - start.tv_sec);
            elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
            printf("The sudoku was generated in %f seconds\n\n", elapsed);

            printMatrix();
        }
    }
    else
    {
        printf("Dimension is not supported\n");
    }

    return 0;
}
