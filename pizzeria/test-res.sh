#!/bin/bash

# Compile the program
gcc main.c -o main -pthread 

# Run the program
./main 100 1000
