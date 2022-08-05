#!/bin/bash
shopt -s extglob
cd ../build
rm -rfv !("." || "..")
shopt -u extglob

cmake ../ -DCMAKE_BUILD_TYPE=debug
cmake --build .