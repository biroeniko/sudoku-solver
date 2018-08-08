# Sudoku-solver
A sudoku solver program, written in C++, using OpenMP.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

The basic requirements for building the executable are:

* CMake 2.8+
* a c++ compiler
* make

#### Installation on Ubuntu

```
sudo apt-get install build-essentials cmake
```

### Installing

This program have been tested on Ubuntu 16.04 but should work under any systems that fulfills the aforementioned requirements.

#### Installation on Ubuntu

If you succesfully cloned the source files and you are currently in the project directory, you can generate the Makefile using the following command:

```
mkdir build
cd build/
cmake ..
```
Now you can build the binary by running:

```
make
```
Now, you should see the executable in the build file.

## Usage instructions
The program has three main functionalities:
* generate all possible 2x2 sudokus and solve them
```
./sudoku 2
```
* generate a 3x3 sudoku with custom difficulty
```
./sudoku 3 generate easy
```
or
```
./sudoku 3 generate medium
```
or
```
./sudoku 3 generate hard
```
## Built With

* [OpenMP](https://www.openmp.org/) - used for creating multiple threads for the tasks (pixel color calculation)

## Authors

* **Biró Enikő** - [BiroEniko](https://github.com/biroeniko)
