#!/bin/bash

if [ $1 ];
then
    if [ $2 ];
    then
        if [ $1 == "gcc" ];
        then
            if [ -f /usr/bin/gcc-$2 ] && [ -f /usr/bin/g++-$2 ];
            then
                export C=/usr/bin/gcc-$2
                export CXX=/usr/bin/g++-$2
                echo "Kit GCC selected:" $(gcc-$2 --version | head -n 1)
                echo "C="${C} "CXX="${CXX}
            else
                echo "Error GCC version $2 not found !"
            fi
        fi

        if [ $1 == "clang" ];
        then
            if [ -f /usr/bin/clang-$2 ] && [ -f /usr/bin/clang++-$2 ];
            then
                export C=/usr/bin/clang-$2
                export CXX=/usr/bin/clang++-$2
                echo "Kit Clang selected:" $(clang-$2 --version | head -n 1)
                echo "C="${C} "CXX="${CXX}
            else
                echo "Error Clang version $2 not found !"
            fi
        fi
    else
        if [ $1 == "gcc" ];
        then
            export C=/usr/bin/gcc
            export CXX=/usr/bin/g++
            echo "Kit GCC selected:" $(gcc --version | head -n 1)
        fi

        if [ $1 == "clang" ];
        then
            export C=/usr/bin/clang
            export CXX=/usr/bin/clang++
            echo "Kit Clang selected:" $(clang --version | head -n 1)
        fi
        echo "C="${C} "CXX="${CXX}
    fi
else
    echo "Usage:"
    echo "# source $0 <gcc/clang> <version>"
fi