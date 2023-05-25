cmake_minimum_required(VERSION 3.5.1)
project(lol C)

set(CMAKE_C_STANDARD 99)

add_executable(
    compiler main.c scanner.c scanner.h memory.h memory.c
    parser.c parser.h ast.c ast.h common.h
    semantic_check.c semantic_check.h
    codegen.c codegen.h
    utils/file.c utils/file.h 
    utils/table.c utils/table.h
    utils/array.c utils/array.h
    tests/test_scanner.h tests/test_scanner.c
    tests/test_parser.h tests/test_parser.c
    tests/test_codegen.h tests/test_codegen.c
)

