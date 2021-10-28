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
    - [x] Reserve and populate stack memory
    - [x] Load values from the stack into registers and from registers into the stack
    - [x] Call external linked functions
    - [x] Basic arithmetic operations (sum, subtraction, multiplication and division)
    - [x] Scopes
    - [x] Comparsions and logical operators
    - [x] Jumps and scope branching
    - [ ] Floating point values
    - [ ] Heap management (allocate memory in the VM's heap to store data such as strings)


<br>

# Usage
You just need to add the files: `mve.c`, `mve.h` and `defines.h` (this one you can use to define some macros that are presented below) into your project and it should work. There is no external libraries apart from the standard ones.

Otherwise, if you use CMake, you can just drag this repository into your project. By default it compiles the examples and a shared and a static library. 

In your CMakeLists file, disable the examples using `set(MICROVE_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)`. If you want a static library, disable the shared using `set(MICROVE_BUILD_SHARED_LIB OFF CACHE BOOL "" FORCE)`. But if you want a shared library, disable the static library using `set(MICROVE_BUILD_STATIC_LIB OFF CACHE BOOL "" FORCE)`.

NOTE: If you are using CMake, you need to set the defines (presented below) on the file `defines.h` or `mve.h`. Otherwise they will not be applied unless you include `mve.c` directly from your code and don't include it in the CMakeLists.

## Defines
You can declare a few defines in your C project to indicate the processor specs and to limit the memory usage.
| Name	| Default Value | Description	|
| - | - | -	|
| `MVE_EXTERNAL_FUNCTIONS_LIMIT` | 8 | The maximum amount of external functions. (External functions are functions from your C project that will be called through your MicroVE program) |
| `MVE_SCOPE_LIMIT` | 8 | The maximum amount of branches. |
| `MVE_BUFFER_SIZE` | 128 | The amount of memory used by the program buffer. This is used to store the program. |
| `MVE_STACK_SIZE` | 128 | The amount of memory used by the stack. This is used to store variables and other stuff. |
| `MVE_HEAP_SIZE` | 128 | The amount of memory used by the heap. This is used to store name of external functions and other runtime values. The external function names are cleared once the VM starts. |
| `MVE_USE_64BIT_TYPES` | `undefined` | Indicate if you want to use 64 bit types such as `int64` and `double`. Leave it undefined if you don't. |
| `MVE_BIG_ENDIAN` | `undefined` | Indicate if the architecture you're building for is big endian. Leave it undefined if it is little endian. |
| `MVE_LOCAL_PROGRAM` | `undefined` | Indicate if the program is in the memory. If this is undefined, then the program will be loaded at runtime. |
| `MVE_ERROR_LOG` | `undefined` | Use to define a function to be called whenever an error is thrown. Example: `#define MVE_ERROR_LOG(vm, program_index, error_id, msg) printf("%s Program index: %u.", msg, program_index);` |

## Basic Example executing an embedded program
```c
void hello(MVE_VM *vm) 
{
    char c1 = MVE_GET_STACK_INT8(vm, 1);
    char c2 = MVE_GET_STACK_INT8(vm, 2);

    printf("Hello: %d %d.", (int) c1, (int) c2);
}


int main() 
{
    uint8_t program[] = {   0x01, 0x00, // Verion Major
                            0x00, 0x00, // Version Minor
                            0x02, 0x00, 0x00, 0x00, // External functions count
                            'h', 'e', 'l', 'l', 'o', 0x00, // Function 1
                            'f', 'u', 'n', 'c', '2', 0x00,  // Function 2,
                            0x02, 0, 0, 0, 4, 5, // The main scope has 2 bytes of memory, 1 byte with the value 4, and other with 5.
                            MVE_OP_INVOKE, 0, 0,
                            MVE_OP_LDI, MVE_R3, 1, 0,
                            MVE_OP_LDI, MVE_R4, 1, 1,
                            MVE_OP_LDR, MVE_R0, MVE_R3, MVE_R4,
                            MVE_OP_LDR, MVE_R1, MVE_R4, MVE_R4,
                            MVE_OP_ADD, MVE_R0, MVE_R0, MVE_R1,
                            MVE_OP_MOV, MVE_R1, MVE_R0,
                            MVE_OP_NEG, MVE_R0,
                            MVE_OP_STR, MVE_R0, MVE_R3, MVE_R4,
                            MVE_OP_CALL, 63, 0, 0, 0,
                            MVE_OP_SCOPE, 0, 0, 0, 0,
                            MVE_OP_ADD, MVE_R0, MVE_R0, MVE_R1,
                            MVE_OP_END,
                            MVE_OP_LDI, MVE_R0, 1, 4,
                            MVE_OP_LDI, MVE_R1, 1, 5,
                            MVE_OP_CMP, MVE_CMP_GREATER, MVE_R2, MVE_R0, MVE_R1,
                            MVE_OP_JMP, MVE_R2, 95, 0, 0, 0,
                            MVE_OP_INVOKE, 0, 0,
                            MVE_OP_NOT, MVE_R0, MVE_R0,
                            MVE_OP_EOP
    };

    MVE_VM vm;
    mve_init(&vm, program);

    mve_link_function(&vm, "hello", &hello);

    mve_start(&vm);

    printf("Size of the VM (bytes): %ld\n", sizeof(vm));

    while (mve_is_running(&vm)) {
        mve_run(&vm);
    }

    return 0;
}

```


## Basic Example executing a program file
```c
void load_next_block(MVE_VM *vm, uint8_t *buffer, uint32_t read_index, uint32_t read_length) 
{
    FILE *fileptr;
    long filelen;

    fileptr = fopen("test.bin", "rb");
    fseek(fileptr, 0, SEEK_END);
    filelen = ftell(fileptr);                   // Get the length of the file.

    fseek(fileptr, read_index, SEEK_SET);       // Go to the given index, to read.

    long length = filelen - read_index;         // Calculate the amount of bytes to read.

    if (read_length < length)                   // If the given length to read is smaller than the file length,
        length = read_length;                   // we use the given length.

    if (fread(buffer, length, 1, fileptr) !=  1)          // Read the bytes from the given index to the smaller length, into the buffer.
        printf("Error reading program file, index: %u, length: %u\n", read_index, read_length);

    fclose(fileptr);             
}


void hello(MVE_VM *vm) 
{
    char c1 = MVE_GET_STACK_INT8(vm, 1);
    char c2 = MVE_GET_STACK_INT8(vm, 2);

    printf("Hello: %d %d.", (int) c1, (int) c2);
}


int main() 
{
    MVE_VM vm;
    mve_init(&vm, &load_next_block);

    mve_link_function(&vm, "hello", &hello);

    mve_start(&vm);

    printf("Size of the VM (bytes): %ld\n", sizeof(vm));

    while (mve_is_running(&vm)) {
        mve_run(&vm);
    }

    return 0;
}

```
