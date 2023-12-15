cmake_minimum_required(VERSION 3.26)
project(MINI_SI C)

set(CMAKE_C_STANDARD 11)

add_executable(MINI_SI
        lexer.c
        tokens.h
        tokens.c)
