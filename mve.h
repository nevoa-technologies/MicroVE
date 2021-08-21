#ifndef MVE_H
#define MVE_H


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

#ifndef MVE_BRANCH_LIMIT
#define MVE_BRANCH_LIMIT 8
#endif

#if MVE_BRANCH_LIMIT < 4
#error MVE_BRANCH_LIMIT must be greater than 4.
#endif

#ifdef MVE_ERROR_LOG
#define STR(x) #x
#define MVE_ASSERT(x, vm, error_id, msg) if (!(x)) { MVE_ERROR_LOG(vm, vm->program_index + vm->buffer_index, error_id, "Error " STR(error_id) ": "  msg); abort(); }
#else
#define MVE_ASSERT(x, vm, error_id, msg) (0)
#endif


#define MVE_ERROR_STACK_OUT_OF_RANGE                    1       // Happens when trying to access an index bigger than the size of the stack.
#define MVE_ERROR_EXTERNAL_FUNCTION_OUT_OF_RANGE        2       // Happens when calling an external functions, which index is invalid.
#define MVE_ERROR_UNDEFINED_OP                          57      // Happens when the OP of the next instruction is not recognized.


#define MVE_OP_PUSH                     (uint8_t) 1
#define MVE_OP_POP                      (uint8_t) 2
#define MVE_OP_LDR                      (uint8_t) 3
#define MVE_OP_STR                      (uint8_t) 4
#define MVE_OP_MOV                      (uint8_t) 5
#define MVE_OP_MVN                      (uint8_t) 6
#define MVE_OP_NEG                      (uint8_t) 7
#define MVE_OP_BX                       (uint8_t) 8
#define MVE_OP_CALLEX                   (uint8_t) 9
#define MVE_OP_SUM                      (uint8_t) 10
#define MVE_OP_SUB                      (uint8_t) 11
#define MVE_OP_MUL                      (uint8_t) 12
#define MVE_OP_DIV                      (uint8_t) 13
#define MVE_OP_CMP                      (uint8_t) 14
#define MVE_OP_AND                      (uint8_t) 15
#define MVE_OP_ORR                      (uint8_t) 16
#define MVE_OP_JIF                      (uint8_t) 17

#define MVE_OP_ITOF                     (uint8_t) 64
#define MVE_OP_FTOI                     (uint8_t) 65
#define MVE_OP_FSUM                     (uint8_t) 66
#define MVE_OP_FSUB                     (uint8_t) 67
#define MVE_OP_FMUL                     (uint8_t) 68
#define MVE_OP_FDIV                     (uint8_t) 69
#define MVE_OP_FCMP                     (uint8_t) 70
#define MVE_OP_FNEG                     (uint8_t) 71

#define MVE_OP_ALLOC                    (uint8_t) 128
#define MVE_OP_FREE                     (uint8_t) 129

#define MVE_OP_EOP                      (uint8_t) 255


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


#define MVE_GET_STACK(vm, index) (vm->stack + vm->stack_index - index)
#define MVE_GET_STACK_UINT8(vm, index) MVE_BYTES_TO_UINT8(vm->stack, vm->stack_index - index)
#define MVE_GET_STACK_UINT16(vm, index) MVE_BYTES_TO_UINT16(vm->stack, vm->stack_index - index)
#define MVE_GET_STACK_UINT32(vm, index) MVE_BYTES_TO_UINT32(vm->stack, vm->stack_index - index)
#define MVE_GET_STACK_UINT64(vm, index) MVE_BYTES_TO_UINT64(vm->stack, vm->stack_index - index)

#define MVE_GET_STACK_INT8(vm, index) MVE_BYTES_TO_INT8(vm->stack, vm->stack_index - index)
#define MVE_GET_STACK_INT16(vm, index) MVE_BYTES_TO_INT16(vm->stack, vm->stack_index - index)
#define MVE_GET_STACK_INT32(vm, index) MVE_BYTES_TO_INT32(vm->stack, vm->stack_index - index)
#define MVE_GET_STACK_INT64(vm, index) MVE_BYTES_TO_INT64(vm->stack, vm->stack_index - index)


typedef uint8_t bool;

 
#ifdef MVE_USE_64BIT_TYPES
typedef struct {
    uint64_t i;
    double   f;
} MVE_Value;
#else
typedef struct  {
    uint32_t i;
    float   f;
} MVE_Value;
#endif


struct MVE_VM;
typedef struct MVE_VM MVE_VM;

typedef struct {
    uint32_t program_index;
    uint32_t stack_base;
} MVE_Branch_Info;


struct MVE_VM {

    MVE_Value registers[6];       // Contains the registers of the virtual machine.
                                  // The first one is used to store the result from operations and also the returned value from functions.
                                  // The others can be used to general purpose.

    void (*fun_load_next_block)(MVE_VM *, uint8_t *, uint32_t, uint32_t);

    void *external_functions[MVE_EXTERNAL_FUNCTIONS_LIMIT];

    uint32_t buffer_index;                      // The current position in the program buffer.

    uint32_t branch_index;                                  // The current branch index.
    MVE_Branch_Info branches[MVE_BRANCH_LIMIT];       // Used to know where it was when calling contexts.

#ifdef MVE_LOCAL_PROGRAM
    uint8_t *program_buffer;    // Buffer to store the next instructions of the program to be processed.
#else
    uint8_t program_buffer[MVE_BUFFER_SIZE];    // Buffer to store the next instructions of the program to be processed.
#endif

    uint32_t program_index;         // The position in the program that is executing. This is only updated when loading the next bytes of the program.

    uint32_t stack_index;
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