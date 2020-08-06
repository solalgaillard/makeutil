cmake_minimum_required(VERSION 3.15)
project(make_utility C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(make_utility
        expr.c
        lex.yy.c
        ppcm.h
        ppcm.tab.c cmake-build-debug/hashTable.c)
