#ifndef MVE_H
#define MVE_H


#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


#define MVE_VERSION_MAJOR ((uint16_t)1) // Bytecode major version. The program must have the same version.
#define MVE_VERSION_MINOR ((uint16_t)5) // Bytecode minor version. The program must have a lower or same version.


#ifndef MVE_EXTERNAL_FUNCTIONS_LIMIT
#define MVE_EXTERNAL_FUNCTIONS_LIMIT 16
#endif

#ifndef MVE_BUFFER_SIZE
#define MVE_BUFFER_SIZE 128
#endif

#ifndef MVE_STACK_SIZE
#define MVE_STACK_SIZE 128
#endif

#ifndef MVE_HEAP_SIZE
#define MVE_HEAP_SIZE 128
#endif

#if MVE_BUFFER_SIZE < 32
#error MVE_BUFFER_SIZE must be greater than 32.
#endif

typedef enum {
    false, true
} bool;

typedef union
{
    uint8_t as_uint8;
    uint16_t as_uint16;
    uint32_t as_uint32;
    int8_t as_int8;
    int16_t as_int16;
    int32_t as_int32;
    float as_float;
    
#ifdef MVE_USE_64BIT_TYPES
    uint64_t as_uint64;
    int64_t as_int64;
    double as_double;
#else
    uint32_t as_uint64;
    int32_t as_int64;
    float as_double;
#endif
} MVE_Value;


struct MVE_VM;
typedef struct MVE_VM MVE_VM;


struct MVE_VM
{
    uint32_t program_index;         // The position in the program that is executing. This is only updated when loading the next bytes of the program.
    uint16_t variables_count;
    uint16_t external_functions_count;

    MVE_Value reg_result;           // A register to store the result from operations and also the returned value from funnctions.

    void (*fun_load_next)(MVE_VM *, uint8_t *, uint32_t, uint32_t);

    void *external_functions[MVE_EXTERNAL_FUNCTIONS_LIMIT];

    uint32_t buffer_index;                      // The current position in the program buffer.
    uint8_t program_buffer[MVE_BUFFER_SIZE];    // Buffer to store the next instructions of the program to be processed.

    uint8_t stack[MVE_STACK_SIZE];              // Stores fixed size data, such as int variables.
    uint8_t heap[MVE_HEAP_SIZE];                // Stores dynamic data such as function names at the start, and strings during execution.
};


/**
 * @brief Prepares the VM to run. Loads the header of the program and sets up all the required data.
 * 
 * @param vm VM to be loaded.
 * @param fun_load_next Function that is going to be called whenever the VM needs to load the next bytes (VM, buffer to load into, index in the program, amount to read).
 * @return Returns true if the VM was initiated successfully. False if an error ocurred, such as incompatible byte code.
 */
bool mve_init(MVE_VM *vm, void (*fun_load_next)(MVE_VM *, uint8_t *, uint32_t, uint32_t));


/**
 * @brief Links a C function into the VM. Use this if you call functions from the program in the VM.
 * 
 * @param vm VM to link the function.
 * @param name Name of the function that is declared in the program.
 * @param function Function to be linked into the VM.
 */
void mve_link_function(MVE_VM *vm, const char *name, void *function);


/**
 * @brief Starts the VM. After calling this, you can no longer link external functions.
 * 
 * @param vm VM to start.
 */
void mve_start(MVE_VM *vm);


/**
 * @brief Runs the next instruction in the VM. This is tipically called inside a while true after start the VM.
 * 
 * @param vm VM to execute the next instruction.
 */
void mve_run(MVE_VM *vm);

#endif