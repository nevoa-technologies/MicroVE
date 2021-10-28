#ifndef MVE_H
#define MVE_H


#include <stdint.h>
#include <stdlib.h>

#include "defines.h"


#define MVE_VERSION_MAJOR ((uint16_t)1) // Bytecode major version. The program must have the same version.
#define MVE_VERSION_MINOR ((uint16_t)0) // Bytecode minor version. The program must have a lower or same version.


#ifndef MVE_EXTERNAL_FUNCTIONS_LIMIT
#define MVE_EXTERNAL_FUNCTIONS_LIMIT 16
#endif


#ifdef MVE_LOCAL_PROGRAM
#define MVE_BUFFER_SIZE UINT32_MAX
#endif

#ifndef MVE_BUFFER_SIZE
#define MVE_BUFFER_SIZE 128
#endif

#if MVE_BUFFER_SIZE < 32
#error MVE_BUFFER_SIZE must be greater than 32.
#endif


#ifndef MVE_STACK_SIZE
#define MVE_STACK_SIZE 128
#endif


#ifndef MVE_HEAP_SIZE
#define MVE_HEAP_SIZE 128
#endif


#ifndef MVE_SCOPE_LIMIT
#define MVE_SCOPE_LIMIT 8
#endif

#if MVE_SCOPE_LIMIT < 4
#error MVE_SCOPE_LIMIT must be greater than 4.
#endif


#ifdef MVE_ERROR_LOG
#define STR(x) #x
#define MVE_ASSERT(x, vm, error_id, msg) if (!(x)) { MVE_ERROR_LOG(vm, vm->program_index + vm->buffer_index, error_id, "Error " STR(error_id) ": "  msg); abort(); }
#else
#define MVE_ASSERT(x, vm, error_id, msg) (0)
#endif


#define MVE_ERROR_INCOMPATIBLE_VERSION                  0       // Happens when the program is not not compatible.
#define MVE_ERROR_STACK_OUT_OF_RANGE                    1       // Happens when trying to access an index bigger than the size of the stack.
#define MVE_ERROR_EXTERNAL_FUNCTION_OUT_OF_RANGE        2       // Happens when calling an external functions, which index is invalid.
#define MVE_ERROR_REGISTER_OUT_OF_RANGE                 3       // Happens when accessing an invalid register, that is smaller than 0 or bigger than MVE_REGISTERS_LIMIT.
#define MVE_ERROR_SCOPE_OUT_OF_RANGE                    4       // Happens when creating or deleting a scope that is out of range.
#define MVE_ERROR_UNRECOGNIZED_CMP_OPERATION            5       // Happens when a compare instruction has an unrecognized operation that is not between 0 and 5.
#define MVE_ERROR_UNDEFINED_OP                          57      // Happens when the OP of the next instruction is not recognized.


#define MVE_OP_EOP                      ((uint8_t) 0)           // Indicates the end of the program. Stops the virtual machine.

#define MVE_OP_LDR                      ((uint8_t) 1)           // Load bytes from the stack into a register, using an address and length from registers.
#define MVE_OP_STR                      ((uint8_t) 2)           // Set bytes of the stack from a register, using an address and length from registers.
#define MVE_OP_LDS                      ((uint8_t) 3)           // Load bytes from the stack into a register, using a stack address.
#define MVE_OP_STS                      ((uint8_t) 4)           // Set bytes of the stack from a register, using a stack address.
#define MVE_OP_LDI                      ((uint8_t) 5)           // Load an immediate constant value into a register.
#define MVE_OP_MOV                      ((uint8_t) 6)           // Copies the value from a register into another.
#define MVE_OP_NEG                      ((uint8_t) 7)           // Negates a register.
#define MVE_OP_INVOKE                   ((uint8_t) 8)           // Call a linked external function.
#define MVE_OP_ADD                      ((uint8_t) 9)           // Adds 2 registers.
#define MVE_OP_SUB                      ((uint8_t) 10)          // Subtracts 2 registers.
#define MVE_OP_MUL                      ((uint8_t) 11)          // Multiplies 2 registers.
#define MVE_OP_DIV                      ((uint8_t) 12)          // Divides 2 registers.
#define MVE_OP_SCOPE                    ((uint8_t) 13)          // Creates a new scope.
#define MVE_OP_END                      ((uint8_t) 14)          // Finishes the previous scope.
#define MVE_OP_CMP                      ((uint8_t) 15)          // Compares 2 registers.
#define MVE_OP_JMP                      ((uint8_t) 16)          // Jumps to a location if the value of the given register is not 0.
#define MVE_OP_CALL                     ((uint8_t) 17)          // Jumps to a location. Creating and ending a scope will make it return to where it was called.
#define MVE_OP_AND                      ((uint8_t) 18)          // Logical And. Performs a bitwise AND on 2 registers.
#define MVE_OP_ORR                      ((uint8_t) 19)          // Logical Or. Performs a bitwise OR on 2 registers.
#define MVE_OP_NOT                      ((uint8_t) 20)          // Logical Not. Performs a bitwise NOT on 2 registers.
#define MVE_OP_LSL                      ((uint8_t) 21)          // Logical Shift Left. Performs a bitwise shift left on 2 registers.
#define MVE_OP_LSR                      ((uint8_t) 22)          // Logical Shift Right. Performs a bitwise shift right on 2 registers.
#define MVE_OP_XOR                      ((uint8_t) 23)          // Logical Exclusive Or. Performs a bitwise XOR on 2 registers.


#define MVE_OP_ITOF                     ((uint8_t) 32)
#define MVE_OP_FTOI                     ((uint8_t) 33)
#define MVE_OP_FADD                     ((uint8_t) 34)
#define MVE_OP_FSUB                     ((uint8_t) 35)
#define MVE_OP_FMUL                     ((uint8_t) 36)
#define MVE_OP_FDIV                     ((uint8_t) 37)
#define MVE_OP_FCMP                     ((uint8_t) 38)
#define MVE_OP_FNEG                     ((uint8_t) 39)

#define MVE_OP_ALLOC                    ((uint8_t) 64)
#define MVE_OP_FREE                     ((uint8_t) 65)


#define MVE_R0                          ((uint8_t) 0)
#define MVE_R1                          ((uint8_t) 1)
#define MVE_R2                          ((uint8_t) 2)
#define MVE_R3                          ((uint8_t) 3)
#define MVE_R4                          ((uint8_t) 4)
#define MVE_RR                          ((uint8_t) 5)


#define MVE_CMP_EQUAL                   ((uint8_t) 0)
#define MVE_CMP_NOTEQUAL                ((uint8_t) 1)
#define MVE_CMP_GREATER                 ((uint8_t) 2)
#define MVE_CMP_LESS                    ((uint8_t) 3)
#define MVE_CMP_GREATEREQUAL            ((uint8_t) 4)
#define MVE_CMP_LESSEQUAL               ((uint8_t) 5)



#define MVE_REGISTERS_LIMIT 6


#define MVE_ASSERT_REGISTER(reg, msg, vm) MVE_ASSERT(reg >= 0 && reg < MVE_REGISTERS_LIMIT, vm, MVE_ERROR_REGISTER_OUT_OF_RANGE, msg " Invalid register. The register cannot be negative or bigger than MVE_REGISTERS_LIMIT.");
#define MVE_ASSERT_STACK_ADDRESS(address, msg, vm) MVE_ASSERT(address >= 0 && address < MVE_STACK_SIZE, vm, MVE_ERROR_STACK_OUT_OF_RANGE, msg " Stack address out of range. The address cannot be negative or bigger than MVE_STACK_SIZE.");

#ifdef MVE_BIG_ENDIAN

#define MVE_BYTES_TO_UINT8(buffer, offset) (uint8_t)(buffer[offset + 1])
#define MVE_BYTES_TO_UINT16(buffer, offset) (uint16_t)((buffer[offset] << 8) + buffer[offset + 1])
#define MVE_BYTES_TO_UINT32(buffer, offset) (uint32_t)((buffer[offset] << 24) + (buffer[offset + 1] << 16) + (buffer[offset + 2] << 8) + buffer[offset + 3])
#define MVE_BYTES_TO_UINT64(buffer, offset) (uint64_t)((buffer[offset] << 56) + (buffer[offset + 1] << 48) + (buffer[offset + 2] << 40) + (buffer[offset + 3] << 32) + (buffer[offset + 4] << 24) + (buffer[offset + 5] << 16) + (buffer[offset + 6 << 8) + buffer[offset + 7])

#define MVE_BYTES_TO_INT8(buffer, offset) (int8_t)(buffer[offset + 1])
#define MVE_BYTES_TO_INT16(buffer, offset) (int16_t)((buffer[offset] << 8) + buffer[offset + 1])
#define MVE_BYTES_TO_INT32(buffer, offset) (int32_t)((buffer[offset] << 24) + (buffer[offset + 1] << 16) + (buffer[offset + 2] << 8) + buffer[offset + 3])
#define MVE_BYTES_TO_INT64(buffer, offset) (int64_t)((buffer[offset] << 56) + (buffer[offset + 1] << 48) + (buffer[offset + 2] << 40) + (buffer[offset + 3] << 32) + (buffer[offset + 4] << 24) + (buffer[offset + 5] << 16) + (buffer[offset + 6 << 8) + buffer[offset + 7])

#else

#define MVE_BYTES_TO_UINT8(buffer, offset) (uint8_t)(buffer[offset])
#define MVE_BYTES_TO_UINT16(buffer, offset) (uint16_t)(buffer[offset] + (buffer[offset + 1] << 8))
#define MVE_BYTES_TO_UINT32(buffer, offset) (uint32_t)(buffer[offset] + (buffer[offset + 1] << 8) + (buffer[offset + 2] << 16) + (buffer[offset + 3] << 24))
#define MVE_BYTES_TO_UINT64(buffer, offset) (uint64_t)(buffer[offset] + (buffer[offset + 1] << 8) + (buffer[offset + 2] << 16) + (buffer[offset + 3] << 24) + (buffer[offset + 4] << 32) + (buffer[offset + 5] << 40) + (buffer[offset + 6] << 48) + (buffer[offset + 7] << 56))

#define MVE_BYTES_TO_INT8(buffer, offset) (int8_t)(buffer[offset])
#define MVE_BYTES_TO_INT16(buffer, offset) (int16_t)(buffer[offset] + (buffer[offset + 1] << 8))
#define MVE_BYTES_TO_INT32(buffer, offset) (int32_t)(buffer[offset] + (buffer[offset + 1] << 8) + (buffer[offset + 2] << 16) + (buffer[offset + 3] << 24))
#define MVE_BYTES_TO_INT64(buffer, offset) (int64_t)(buffer[offset] + (buffer[offset + 1] << 8) + (buffer[offset + 2] << 16) + (buffer[offset + 3] << 24) + (buffer[offset + 4] << 32) + (buffer[offset + 5] << 40) + (buffer[offset + 6] << 48) + (buffer[offset + 7] << 56))


#endif


#define MVE_GET_STACK(vm, address) (vm->stack + vm->stack_pointer - address)
#define MVE_GET_STACK_UINT8(vm, address) MVE_BYTES_TO_UINT8(vm->stack, vm->stack_pointer - address)
#define MVE_GET_STACK_UINT16(vm, address) MVE_BYTES_TO_UINT16(vm->stack, vm->stack_pointer - address)
#define MVE_GET_STACK_UINT32(vm, address) MVE_BYTES_TO_UINT32(vm->stack, vm->stack_pointer - address)
#define MVE_GET_STACK_UINT64(vm, address) MVE_BYTES_TO_UINT64(vm->stack, vm->stack_pointer - address)

#define MVE_GET_STACK_INT8(vm, address) MVE_BYTES_TO_INT8(vm->stack, vm->stack_pointer - address)
#define MVE_GET_STACK_INT16(vm, address) MVE_BYTES_TO_INT16(vm->stack, vm->stack_pointer - address)
#define MVE_GET_STACK_INT32(vm, address) MVE_BYTES_TO_INT32(vm->stack, vm->stack_pointer - address)
#define MVE_GET_STACK_INT64(vm, address) MVE_BYTES_TO_INT64(vm->stack, vm->stack_pointer - address)


typedef uint8_t bool;

 
#ifdef MVE_USE_64BIT_TYPES
typedef union {
    uint64_t i;
    double f;
    uint8_t b[8];
} MVE_Value;
#else
typedef union  {
    uint32_t i;
    float f;
    uint8_t b[4];
} MVE_Value;
#endif


typedef struct {
    uint32_t program_index;
    uint32_t stack_base;
} MVE_Scope_Info;


struct MVE_VM;
typedef struct MVE_VM MVE_VM;


typedef union
{
    struct
    {
        MVE_Value r0;
        MVE_Value r1;
        MVE_Value r2;
        MVE_Value r3;
        MVE_Value r4;
        MVE_Value rr;
    };
    
    MVE_Value all[6];
} MVE_Registers;
    

struct MVE_VM {

    MVE_Registers registers;        // Contains the registers of the virtual machine.
                                    // The first one is used to store the result from operations and also the returned value from functions.
                                    // The others can be used to general purpose.

    void (*fun_load_next_block)(MVE_VM *, uint8_t *, uint32_t, uint32_t);

    void *external_functions[MVE_EXTERNAL_FUNCTIONS_LIMIT];

    uint32_t buffer_index;                      // The current position in the program buffer.

    uint32_t scope_index;                                  // The current scope index.
    MVE_Scope_Info scopes[MVE_SCOPE_LIMIT];       // Used to know where it was when calling contexts.

#ifdef MVE_LOCAL_PROGRAM
    uint8_t *program_buffer;    // Buffer to store the next instructions of the program to be processed.
#else
    uint8_t program_buffer[MVE_BUFFER_SIZE];    // Buffer to store the next instructions of the program to be processed.
#endif

    uint32_t program_index;         // The position in the program that is executing. This is only updated when loading the next bytes of the program.

    uint32_t stack_pointer;
    uint8_t stack[MVE_STACK_SIZE];              // Stores fixed size data, such as int variables.
    uint8_t heap[MVE_HEAP_SIZE];                // Stores dynamic data such as function names at the start, and strings during execution.

    uint16_t external_functions_count;
    bool is_running;
};


#ifdef MVE_LOCAL_PROGRAM
/**
 * @brief Prepares the VM to run. Loads the header of the program and sets up all the required data.
 * 
 * @param vm VM to be loaded.
 * @param program Program byte array to execute.
 * @return Returns true if the VM was initiated successfully. False if an error ocurred, such as incompatible byte code.
 */
bool mve_init(MVE_VM *vm, uint8_t *program);
#else
/**
 * @brief Prepares the VM to run. Loads the header of the program and sets up all the required data.
 * 
 * @param vm VM to be loaded.
 * @param fun_load_next Function that is going to be called whenever the VM needs to load the next bytes (VM, buffer to load into, index in the program, amount to read).
 * @return Returns true if the VM was initiated successfully. False if an error ocurred, such as incompatible byte code.
 */
bool mve_init(MVE_VM *vm, void (*fun_load_next_block)(MVE_VM *, uint8_t *, uint32_t, uint32_t));
#endif

/**
 * @brief Links a C function into the VM. Use this if you call functions from the program in the VM.
 * 
 * @param vm VM to link the function.
 * @param name Name of the function that is declared in the program.
 * @param function Function to be linked into the VM.
 */
void mve_link_function(MVE_VM *vm, const char *name, void (* function)(MVE_VM *));


/**
 * @brief Start the VM. After calling this, you can no longer link external functions.
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


/**
 * @brief Stop the VM. To continue, 'start' must be called. To restart,  'init' must be called. 
 * 
 * @param vm VM to start.
 */
void mve_stop(MVE_VM *vm);

#endif