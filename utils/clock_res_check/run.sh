#!/bin/bash
echo "Check A"
gcc -O2 -march=native -lrt clockres_a.c
./a.out

echo -e "\nCheck B"
g++ -O2 -march=native -lrt clockres_b.cpp
./a.out

echo -e "\nCleanup"
rm a.out