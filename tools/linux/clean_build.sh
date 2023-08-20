#!/bin/bash

shopt -s extglob
if [ "${C}" ] && [ "${CXX}" ];
then
    cd ../../build
    rm -rfv !("." || "..")
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    cmake --build . --parallel 2 --config Debug
else
    echo "Kit not selected !"
fi
shopt -u extglob