# MicroVE (Work in Progress)
MicroVE IT'S STILL IN EARLY DEVEOPMENT! IT SHOULD NOT BE USED AS IT IS!

<br>
<br>

# A Virtual Environment for Embedded Systems.
MicroVE aims a lightweight solution for a dynamic code execution on limited devices. All the memory usage is limited at compile-time, so there is no runtime memory allocation. This allows you to run a program under **500 bytes** of RAM (excluding the program itself).

MicroVE has its own bytecode, and this repository contains a base Virtual Machine to execute it. Later, a compiler will be developed to compile programmer-friendly code into executable bytecode.

A program file contains the entire bytecode to be interpreted by the Virtual Machine. If you plan to use this in an embedded environment and your compiled bytecode is over your remainded size, you may consider saving it into an external storage like an SPI EEPROM. MicroVE does not load the entire program into the memory, it loads as it needs, so you can load always 64 bytes at a time, if you want.

<br>

# Add to your project
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
