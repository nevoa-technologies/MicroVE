#include "mve.h"


#define true    1
#define false   0


static inline bool string_equals(const char *str1, const char *str2) {
    uint16_t i = 0;

    while (str1[i] != 0 && str1[i] == str2[i])
    {
        if (str2[i] == 0)
            return false;

        i++;
    }

    return str2[i] == 0;
}


/**
 * @brief Loads the next bytes of the program file.
 * If the buffer index is not at the end of the buffer,
 * it will drag the remain bytes to the start of the buffer.
 * Then it will read the next program bytes.   
 * 
 * @param vm VM to load the bytes into.
 */
static void mve_load_next_block(MVE_VM *vm) {

#ifdef MVE_LOCAL_PROGRAM
#else
    // TODO: This may cause infinite looping. Add verifications in the future.
    //if (vm->buffer_index == 0)
    //    return;

    int index = 0;

    // Move the last bytes to the start of the buffer.
    for (uint32_t i = vm->buffer_index; i < MVE_BUFFER_SIZE; i++)
    {
        vm->program_buffer[index] = vm->program_buffer[i];
        index++;
    }

    // The bytes to load, are going to be placed in the buffer after the bytes moved to the start.
    uint8_t *buffer = vm->program_buffer + index;
    uint32_t length = vm->buffer_index;

    if (vm->buffer_index == 0) {
        length = MVE_BUFFER_SIZE;
        buffer = vm->program_buffer;
    }


    vm->buffer_index = 0;

    vm->fun_load_next_block(vm, buffer, vm->program_index, length);

    vm->program_index += length;
#endif
}


/**
 * @brief Ensures that the buffer have the program within a specific index and length.
 * This is used to read a specific amount of bytes,
 * without having to check the size of the buffer.
 * If the size passes the buffer limit, then it will read the next bytes.
 * 
 * @param vm VM to ensure the buffer size.
 * @param index Index at the buffer.
 * @param size Size to ensure the buffer has after the index.
 */
inline static void mve_ensure_buffer_size_at(MVE_VM *vm, uint16_t index, uint8_t length) {
    if (index + length > MVE_BUFFER_SIZE)
        mve_load_next_block(vm);
}


/**
 * @brief Ensures that the buffer have the program within the buffer index and length.
 * This is used to read a specific amount of bytes,
 * without having to check the size of the buffer.
 * If the size passes the buffer limit, then it will read the next bytes.
 * 
 * @param vm VM to ensure the buffer size.
 * @param size Size to ensure the buffer has after the buffer index.
 */
inline static void mve_ensure_buffer_size(MVE_VM *vm, uint8_t length) {
    mve_ensure_buffer_size_at(vm, vm->buffer_index, length);
}


/**
 * @brief Returns the next uint32 from the program buffer and increases the buffer index.
 * 
 * @param vm VM to read the next byte.
 * @return Returns the value readed.
 */
inline static uint32_t mve_request_uint32(MVE_VM *vm) {
    mve_ensure_buffer_size(vm, 4);
    uint32_t value = MVE_BYTES_TO_UINT32(vm->program_buffer, vm->buffer_index);
    vm->buffer_index += 4;

    return value;   
}


/**
 * @brief Returns the next uint16 from the program buffer and increases the buffer index.
 * 
 * @param vm VM to read the next byte.
 * @return Returns the value readed.
 */
inline static uint32_t mve_request_uint16(MVE_VM *vm) {
    mve_ensure_buffer_size(vm, 2);
    uint32_t value = MVE_BYTES_TO_UINT16(vm->program_buffer, vm->buffer_index);
    vm->buffer_index += 2;

    return value;   
}


/**
 * @brief Returns the next byte from the program buffer and increases the buffer index.
 * 
 * @param vm VM to read the next byte.
 * @return Returns the byte readed.
 */
inline static uint8_t mve_request_uint8(MVE_VM *vm) {
    mve_ensure_buffer_size(vm, 1);
    uint8_t byte = vm->program_buffer[vm->buffer_index];
    vm->buffer_index++;

    return byte;   
}


/**
 * @brief Loads and processes the header of the program.
 * It the bytecode version of the program is not compatible, it will abort.
 * 
 * @param vm VM to load the header.
 */
static bool mve_load_header(MVE_VM *vm) {

    uint16_t major_version = MVE_BYTES_TO_UINT16(vm->program_buffer, 0);
    uint16_t minor_version = MVE_BYTES_TO_UINT16(vm->program_buffer, 2);

    if (major_version != MVE_VERSION_MAJOR)
        return false;

    if (minor_version > MVE_VERSION_MINOR)
        return false;

    vm->buffer_index = 4;
    uint16_t external_functions_length = mve_request_uint32(vm);

    uint8_t strings_counter = 0;

    // Load the function names.
    for (uint32_t i = 0; i < MVE_HEAP_SIZE && strings_counter < external_functions_length; i++) {
        vm->heap[i] = mve_request_uint8(vm);
        
        if (vm->heap[i] == '\0')
            strings_counter++;
    }

    vm->external_functions_count = strings_counter;

    return true;
}


#ifdef MVE_LOCAL_PROGRAM
bool mve_init(MVE_VM *vm, uint8_t *program) {
    vm->program_buffer = program;
#else
bool mve_init(MVE_VM *vm, void (*fun_load_next_block)(MVE_VM *, uint8_t *, uint32_t, uint32_t)) {
    vm->fun_load_next_block = fun_load_next_block;
#endif
    vm->program_index = 0;
    vm->is_running = false;
    vm->buffer_index = 0;
    vm->stack_index = 0;
    vm->branch_index = 0;

    for (uint16_t i = 0; i < MVE_EXTERNAL_FUNCTIONS_LIMIT; i++) {
        vm->external_functions[i] = NULL;
    }

    mve_load_next_block(vm);

    return mve_load_header(vm);
}


void mve_link_function(MVE_VM *vm, const char *name, void (*function) (MVE_VM *)) {
    uint32_t heap_index = 0;
    uint16_t function_index = 0;

    for (uint16_t i = 0; i < vm->external_functions_count; i++) {

        uint32_t start = heap_index;

        while (vm->heap[heap_index] != '\0') {
            heap_index++;
        } 

        heap_index++;

        if (string_equals((const char *) vm->heap + start, name))
        {
            vm->external_functions[function_index] = function;
            return;
        }

        function_index++;
    }
}


void mve_start(MVE_VM *vm) {
    vm->is_running = true;
}


void mve_op_push(MVE_VM *vm) {

    // The size of the value to push.
    uint8_t size = mve_request_uint8(vm);

    MVE_ASSERT(MVE_STACK_SIZE > vm->stack_index + size, vm, MVE_ERROR_STACK_OUT_OF_RANGE, "PUSH failed! Stack out of range. The index cannot be bigger than the stack.");

    // Write the value into the stack.
    for (uint16_t i = 0; i < size; i++)
        vm->stack[vm->stack_index + i] = mve_request_uint8(vm);

    vm->stack_index += size;
}


void mve_op_pop(MVE_VM *vm) {

    // The size of the value to pop.
    uint8_t size = mve_request_uint8(vm);

    MVE_ASSERT(vm->stack_index - size > 0, vm, MVE_ERROR_STACK_OUT_OF_RANGE, "POP failed! Stack out of range. The index cannot be negative.");

    vm->stack_index -= size;
}


void mve_op_ldr(MVE_VM *vm) {

    // The register to load the value into.
    uint8_t reg = mve_request_uint8(vm);

    uint32_t stack_end_index = mve_request_uint32(vm);
    uint8_t length = mve_request_uint8(vm);

    MVE_Value value;
    value.i = 0;

    for (uint8_t i = 0; i < length; i++)
    {
        #ifdef MVE_BIG_ENDIAN
            value.b[length - i - 1] = vm->stack[vm->stack_index - stack_end_index + i];
        #else
            value.b[i] = vm->stack[vm->stack_index - stack_end_index + i];
        #endif
    }

    vm->registers.all[reg] = value;
}


void mve_op_add(MVE_VM *vm) {

    uint8_t reg_result = mve_request_uint8(vm);
    uint8_t reg_op1 = mve_request_uint8(vm);
    uint8_t reg_op2 = mve_request_uint8(vm);

    vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i + vm->registers.all[reg_op2].i;
}


void mve_op_callex(MVE_VM *vm) {

    uint16_t function_index = mve_request_uint16(vm);

    MVE_ASSERT(vm->external_functions_count > function_index, vm, MVE_ERROR_EXTERNAL_FUNCTION_OUT_OF_RANGE, "CALLEX failed! Invalid function index.");

    uint8_t params_count = mve_request_uint8(vm);
    int total_length = 0;

    for (int i = 0; i < params_count; i++)
        total_length += mve_request_uint8(vm);

    void (*func) () = vm->external_functions[0];
    func(vm);
}


void mve_run(MVE_VM *vm) {

    uint8_t next_operation = mve_request_uint8(vm);

    switch (next_operation)
    {
    case MVE_OP_PUSH:
        mve_op_push(vm);
        break;
    case MVE_OP_POP:
        mve_op_pop(vm);
        break;
    case MVE_OP_LDR:
        mve_op_ldr(vm);
        break;
    case MVE_OP_CALLEX:
        mve_op_callex(vm);
        break;
    case MVE_OP_ADD:
        mve_op_add(vm);
        break;
    case MVE_OP_EOP:
        mve_stop(vm);
        break;
    default:
        MVE_ASSERT(false, vm, MVE_ERROR_UNDEFINED_OP, "Undefined Instruction! Code does not exist.");
        mve_stop(vm);
        break;
    }
}


bool mve_is_running(MVE_VM *vm) {
    return vm->is_running;
}


void mve_stop(MVE_VM *vm) {
    vm->is_running = false;
}
