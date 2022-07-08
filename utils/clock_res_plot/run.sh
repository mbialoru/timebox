#!/bin/bash
if [ ! -f ./a.out ]; then
    echo "Compiling ..."
    g++ -O2 -lrt -march=native src/main.cpp
fi
echo "Running clock resolution test, mode 0 ..."
./a.out -l 1001 -m 0 -k | python3 plot.py mode0

echo "Running clock resolution test, mode 1 ..."
./a.out -l 1001 -m 1 -k | python3 plot.py mode1

echo "Running clock resolution test, mode 2 ..."
./a.out -l 1001 -m 2 -k | python3 plot.py mode2

echo "Running clock resolution test, mode 3 ..."
./a.out -l 1001 -m 3 -k | python3 plot.py mode3
