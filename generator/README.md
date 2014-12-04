Input generator
===============

This program generates inputs for both our own metaheuristic programs and IBM ILOG CPLEX.

Files:
* InputGenerator.cpp
	`Usage: InputGenerator numProcs numInstructions inputNum numDigits debug`
	* numProcs: number of processors
	* numInstructions: number of instructions to generate
	* inputNum: numer of this input sample
	* numDigits: number of digits needed to represent all input sample numbers
	* debug: print debug information
* Makefile
	* `make all` to compile
	* `make clean` to remove the executable
