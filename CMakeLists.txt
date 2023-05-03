cmake_minimum_required(VERSION 3.5.1)
project(lol C)

set(CMAKE_C_STANDARD 99)

add_executable(compiler main.c scanner.c scanner.h memory.h memory.c array.c
    array.h parser.c parser.h utils/file.c utils/file.h tests/test_scanner.h
    tests/test_scanner.c)
