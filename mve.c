#include "mve.h"


#ifdef MVE_BIG_ENDIAN

#define BYTES_TO_UINT8(buffer, offset) (uint8_t)(buffer[offset + 1])
#define BYTES_TO_UINT16(buffer, offset) (uint16_t)((buffer[offset] << 8) + buffer[offset + 1])
#define BYTES_TO_UINT32(buffer, offset) (uint32_t)((buffer[offset] << 24) + (buffer[offset + 1] << 16) + (buffer[offset + 2] << 8) + buffer[offset + 3])
#define BYTES_TO_UINT64(buffer, offset) (uint64_t)((buffer[offset] << 56) + (buffer[offset + 1] << 48) + (buffer[offset + 2] << 40) + (buffer[offset + 3] << 32) + (buffer[offset + 4] << 24) + (buffer[offset + 5] << 16) + (buffer[offset + 6 << 8) + buffer[offset + 7])

#define BYTES_TO_INT8(buffer, offset) (int8_t)(buffer[offset + 1])
#define BYTES_TO_INT16(buffer, offset) (int16_t)((buffer[offset] << 8) + buffer[offset + 1])
#define BYTES_TO_INT32(buffer, offset) (int32_t)((buffer[offset] << 24) + (buffer[offset + 1] << 16) + (buffer[offset + 2] << 8) + buffer[offset + 3])
#define BYTES_TO_INT64(buffer, offset) (int64_t)((buffer[offset] << 56) + (buffer[offset + 1] << 48) + (buffer[offset + 2] << 40) + (buffer[offset + 3] << 32) + (buffer[offset + 4] << 24) + (buffer[offset + 5] << 16) + (buffer[offset + 6 << 8) + buffer[offset + 7])

#else

#define BYTES_TO_UINT8(buffer, offset) (uint8_t)(buffer[offset])
#define BYTES_TO_UINT16(buffer, offset) (uint16_t)(buffer[offset] + (buffer[offset + 1] << 8))
#define BYTES_TO_UINT32(buffer, offset) (uint32_t)(buffer[offset] + (buffer[offset + 1] << 8) + (buffer[offset + 2] << 16) + (buffer[offset + 3] << 24))
#define BYTES_TO_UINT64(buffer, offset) (uint64_t)(buffer[offset] + (buffer[offset + 1] << 8) + (buffer[offset + 2] << 16) + (buffer[offset + 3] << 24) + (buffer[offset + 4] << 32) + (buffer[offset + 5] << 40) + (buffer[offset + 6] << 48) + (buffer[offset + 7] << 56))

#define BYTES_TO_INT8(buffer, offset) (int8_t)(buffer[offset])
#define BYTES_TO_INT16(buffer, offset) (int16_t)(buffer[offset] + (buffer[offset + 1] << 8))
#define BYTES_TO_INT32(buffer, offset) (int32_t)(buffer[offset] + (buffer[offset + 1] << 8) + (buffer[offset + 2] << 16) + (buffer[offset + 3] << 24))
#define BYTES_TO_INT64(buffer, offset) (int64_t)(buffer[offset] + (buffer[offset + 1] << 8) + (buffer[offset + 2] << 16) + (buffer[offset + 3] << 24) + (buffer[offset + 4] << 32) + (buffer[offset + 5] << 40) + (buffer[offset + 6] << 48) + (buffer[offset + 7] << 56))

#endif

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
    for (int i = vm->buffer_index; i < MVE_BUFFER_SIZE; i++)
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
    uint32_t value = BYTES_TO_UINT32(vm->program_buffer, vm->buffer_index);
    vm->buffer_index += 4;

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

    uint16_t major_version = BYTES_TO_UINT16(vm->program_buffer, 0);
    uint16_t minor_version = BYTES_TO_UINT16(vm->program_buffer, 2);

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
    vm->program_index = 0;
#endif
    vm->is_running = false;
    vm->buffer_index = 0;

    for (uint16_t i = 0; i < MVE_EXTERNAL_FUNCTIONS_LIMIT; i++) {
        vm->external_functions[i] = NULL;
    }

    mve_load_next_block(vm);

    return mve_load_header(vm);
}


void mve_link_function(MVE_VM *vm, const char *name, void *function) {
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

    // TODO: Implementation
}


void mve_run(MVE_VM *vm) {
    vm->is_running = false;

    // TODO: Implementation
}


bool mve_is_running(MVE_VM *vm) {
    return vm->is_running;
}