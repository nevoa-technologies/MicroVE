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

 
#ifdef MVE_USE_64BIT_TYPES
#define MVE_VALUE(name)  uint8_t name[8]
#else
#define MVE_VALUE(name)  uint8_t name[4]
#endif


struct MVE_VM;
typedef struct MVE_VM MVE_VM;


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


/**
 * @brief Indicates whether the VM is running or not.
 * 
 * @param vm The VM to check if is running.
 */
bool mve_is_running(MVE_VM *vm);

#endif