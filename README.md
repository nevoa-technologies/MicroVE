# MicroVE (Work in Progress)
MicroVE IT'S STILL IN EARLY DEVEOPMENT! IT SHOULD NOT BE USED AS IT IS!

<br>
<br>

![icon](https://raw.githubusercontent.com/nevoa-dev/micro-ve/master/icon.png)

[![CMake](https://github.com/nevoa-dev/micro-ve/actions/workflows/cmake.yml/badge.svg?branch=master)](https://github.com/nevoa-dev/micro-ve/actions/workflows/cmake.yml)

# A Virtual Environment for Embedded Systems.
MicroVE aims a lightweight solution for a dynamic code execution on limited devices. All the memory usage is limited at compile-time, so there is no runtime memory allocation. This allows you to run a program under **500 bytes** of RAM (excluding the program itself). The main purpose is to be added to a project and run under a microcontroller, but it can also be embeddable into any other kind of project such as a game or a webserver (Although it may lose kind of its purpose in that environments).

A program file contains the entire bytecode to be interpreted by the Virtual Machine. If you plan to use this in an embedded environment and your compiled bytecode is over your remainded size, you may consider saving it into an external storage like an SPI EEPROM. MicroVE does not load the entire program into the memory, it loads as it needs, so you can load always 64 bytes at a time, if you want.

MicroVE has its own bytecode, and this repository contains a base Virtual Machine to execute it. Later, a compiler will be developed to compile programmer-friendly code into executable bytecode.

<br>

# Progress for the first release
At the moment, MicroVE is not usable. The version 1.0 will be released as soon as the points are all completed.
- [x] Read program blocks when needed
- [x] Interpret the program header
- [x] Link C functions into the virtual machine
- Execution
    - [ ] Declare variables
    - [ ] Assign values to variables
    - [ ] Call external linked functions
    - [ ] Basic arithmetic operations (sum, subtraction, multiplication and division)
    - [ ] Comparsions (if, else)
    - [ ] Loops
    - [ ] Heap management (allocate memory in the VM's heap to store data such as strings)


<br>

# Usage
You just need to add the `mve.c` and `mve.h` files into your project and it should work. There is not external libraries apart from the standard ones.

## Defines
You can declare a few defines in your C project to indicate the processor specs and to limit the memory usage.
| Name	| Default Value | Description	|
| - | - | -	|
| `MVE_EXTERNAL_FUNCTIONS_LIMIT` | 8 | The maximum amount of external functions. (External functions are functions from your C project that will be called through your MicroVE program) |
| `MVE_BUFFER_SIZE` | 128 | The amount of memory used by the program buffer. This is used to store the program. |
| `MVE_STACK_SIZE` | 128 | The amount of memory used by the stack. This is used to store variables and other stuff. |
| `MVE_HEAP_SIZE` | 128 | The amount of memory used by the heap. This is used to store name of external functions and other runtime values. The external function names are cleared once the VM starts. |
| `MVE_USE_64BIT_TYPES` | `undefined` | Indicate if you want to use 64 bit types such as `int64` and `double`. Leave it undefined if you don't. |
| `MVE_BIG_ENDIAN` | `undefined` | Indicate if the architecture you're building for is big endian. Leave it undefined if it is little endian. |

## Basic Example
```c
#include "mve.h"
#include <stdio.h>

#define MVE_EXTERNAL_FUNCTIONS_LIMIT 8
#define MVE_BUFFER_SIZE 128
#define MVE_STACK_SIZE 128
#define MVE_HEAP_SIZE 128

void load_next(MVE_VM *vm, uint8_t *buffer, uint32_t read_index, uint32_t read_length) {
    // Read the next bytecode block of your program.
    // Simple example, loading from a file:  
    FILE *fileptr;
    long filelen;

    fileptr = fopen("test.bin", "rb");
    fseek(fileptr, 0, SEEK_END);
    filelen = ftell(fileptr);                 // Get the length of the file.

    fseek(fileptr, read_index, SEEK_SET);     // Go to the given index, to read.

    long length = filelen - read_index;       // Calculate the amount of bytes to read.

    if (read_length < length)                 // If the given length to read is smaller than the file length, 
        length = read_length;                 // we use the given length.

    fread(buffer, length, 1, fileptr);        // Read the bytes from the given index to the smaller length, into the buffer.
    fclose(fileptr);  
}

void hello() {
    printf("Hello!");
}

int main() {
    MVE_VM vm;
    mve_init(&vm, &load_next);
    
    mve_link_function(&vm, "hello", (void *)&hello);

    mve_start(&vm);

    while (mve_is_running(&vm)) {
        mve_run(&vm);
    }

    return 0;
}

```
