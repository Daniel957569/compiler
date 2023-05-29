# Compiler

My toy compiler.

## Prerequisites

- CMake (version 3.5.2 or higher)
- Make
- C Compiler 

## Getting Started

1. Clone the repository:

```shell
git clone https://github.com/Daniel957569/compiler.git
```

2. Create a build directory:

```shell
cd compiler
cd build
```

3. Generate the build files using CMake:

```shell
cmake ..
```

4. Compile the project:

```shell
make
```


## Running the Compiler

To execute code written in the custom language, follow these steps:

1. Place your code in a text file, for example, `ex.txt`.

2. Run the executable with the text file as input:

```shell
./compiler ex.txt
```


## Executable Location

The compiled executable will be saved in the directory in which cmake resides,
in this case the Build directory


## Features
1. while loops
2. if statement
2. function declaration && parameters
3. function calling && arguments
4. local && global variables
5. global && local scopes
6. types: string, int, bool, TODO: float, TODO: struct, TODO: arrays
7. staticly typed
8. TODO: structs
9. TODO: arrays

i'll finish the todos whenever :)
