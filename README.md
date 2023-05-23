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
mkdir build
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

