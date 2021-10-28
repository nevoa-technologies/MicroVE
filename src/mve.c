#include "mve.h"


#define true    1
#define false   0


static inline bool string_equals(const char *str1, const char *str2) 
{
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
static void mve_load_next_block(MVE_VM *vm) 
{
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

    if (vm->buffer_index == 0) 
    {
        length = MVE_BUFFER_SIZE;
        buffer = vm->program_buffer;
    }


    vm->buffer_index = 0;

    vm->fun_load_next_block(vm, buffer, vm->program_index, length);

    vm->program_index += length;
#endif
}


/**
 * @brief Jumps a given location in the program.
 * If it's not loaded, it will load that location first.
 * 
 * @param vm The VM executing the program.
 * @param index Index in the program to go.
 */
static void mve_jump_to_program_index(MVE_VM *vm, uint32_t index) 
{
    #ifdef MVE_LOCAL_PROGRAM
        vm->buffer_index = index;
    #else
        if (vm->program_index - MVE_BUFFER_SIZE <= index && index <= vm->program_index) 
        {
            vm->buffer_index = index - (vm->program_index - MVE_BUFFER_SIZE);
            return;
        }

        vm->buffer_index = 0;
        vm->program_index = index;
        mve_load_next_block(vm);
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
inline static void mve_ensure_buffer_size_at(MVE_VM *vm, uint16_t index, uint8_t length) 
{
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
inline static void mve_ensure_buffer_size(MVE_VM *vm, uint8_t length) 
{
    mve_ensure_buffer_size_at(vm, vm->buffer_index, length);
}


/**
 * @brief Returns the next uint32 from the program buffer and increases the buffer index.
 * 
 * @param vm VM to read the next byte.
 * @return Returns the value readed.
 */
static inline uint32_t mve_request_int32(MVE_VM *vm) 
{
    #ifndef MVE_LOCAL_PROGRAM
        mve_ensure_buffer_size(vm, 4);
    #endif

    uint32_t value = MVE_BYTES_TO_INT32(vm->program_buffer, vm->buffer_index);
    vm->buffer_index += 4;

    return value;   
}


/**
 * @brief Returns the next uint32 from the program buffer and increases the buffer index.
 * 
 * @param vm VM to read the next byte.
 * @return Returns the value readed.
 */
static inline uint32_t mve_request_uint32(MVE_VM *vm) 
{
    #ifndef MVE_LOCAL_PROGRAM
        mve_ensure_buffer_size(vm, 4);
    #endif

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
static inline uint32_t mve_request_uint16(MVE_VM *vm) 
{
    #ifndef MVE_LOCAL_PROGRAM
        mve_ensure_buffer_size(vm, 2);
    #endif

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
static inline uint8_t mve_request_uint8(MVE_VM *vm) 
{
    #ifndef MVE_LOCAL_PROGRAM
        mve_ensure_buffer_size(vm, 1);
    #endif

    uint8_t byte = vm->program_buffer[vm->buffer_index];
    vm->buffer_index++;

    return byte;   
}


static void mve_load_scope_memory(MVE_VM *vm) 
{
    uint32_t length = mve_request_uint32(vm);

    MVE_ASSERT_STACK_ADDRESS(length + vm->stack_pointer < MVE_STACK_SIZE, "Error loading scope memory.", vm);

    for (uint32_t i = 0; i < length; i++) {
        vm->stack[vm->stack_pointer] = mve_request_uint8(vm);
        vm->stack_pointer++;
    }
}


/**
 * @brief Loads and processes the header of the program.
 * It the bytecode version of the program is not compatible, it will abort.
 * 
 * @param vm VM to load the header.
 */
static bool mve_load_header(MVE_VM *vm) 
{
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

    mve_load_scope_memory(vm);

    return true;
}


static void mve_op_ldr(MVE_VM *vm)
{
    // The register to receive the value.
    uint8_t reg = mve_request_uint8(vm);

    // The register that contains the index to load.
    uint8_t reg_index = mve_request_uint8(vm);

    // The register that contains the amount o bytes to load.
    uint8_t reg_length = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg, "LDR failed!", vm);
    MVE_ASSERT_REGISTER(reg_index, "LDR failed!", vm);
    MVE_ASSERT_REGISTER(reg_length, "LDR failed!", vm);

    MVE_Value value;
    value.i = 0;

    uint32_t stack_address = vm->registers.all[reg_index].i;
    uint32_t length = vm->registers.all[reg_length].i;

    // Copy the bytes from the stack into the value.
    for (uint8_t i = 0; i < length; i++)
    {
        #ifdef MVE_BIG_ENDIAN
            value.b[length - i - 1] = vm->stack[stack_address + i];
        #else
            value.b[i] = vm->stack[stack_address + i];
        #endif
    }

    vm->registers.all[reg] = value;
}


static void mve_op_str(MVE_VM *vm)
{
    // The register to load the value from.
    uint8_t reg = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg, "STR failed!", vm);

    // The register that contains the index to load.
    uint8_t reg_index = mve_request_uint8(vm);

    // The register that contains the amount o bytes to load.
    uint8_t reg_length = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg, "LDR failed!", vm);
    MVE_ASSERT_REGISTER(reg_index, "LDR failed!", vm);
    MVE_ASSERT_REGISTER(reg_length, "LDR failed!", vm);

    MVE_Value value;
    value.i = 0;

    uint32_t stack_address = vm->registers.all[reg_index].i;
    uint32_t length = vm->registers.all[reg_length].i;

    // Copy the bytes from the register into the stack.
    for (uint8_t i = 0; i < length; i++)
    {
        #ifdef MVE_BIG_ENDIAN
            vm->stack[stack_address + i] = vm->registers.all[reg].b[length - i - 1];
        #else
            vm->stack[stack_address + i] = vm->registers.all[reg].b[i];
        #endif
    }
}


static void mve_op_lds(MVE_VM *vm) 
{
    // The register to receive the value.
    uint8_t reg = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg, "LDS failed!", vm);

    // Address of the stack and length of the bytes.
    int32_t stack_address = mve_request_uint32(vm);
    uint8_t length = mve_request_uint8(vm);

    MVE_Value value;
    value.i = 0;

    uint32_t address = 0;

    // If the stack_address is negative then the end of the stack is used.
    // This is used when accessing scope memory.
    if (stack_address < 0)
        address = vm->stack_pointer - (-stack_address);
    else
        address = stack_address;

    MVE_ASSERT_STACK_ADDRESS(address, "LDS failed!", vm);
    MVE_ASSERT_STACK_ADDRESS(address + length, "LDS failed!", vm);

    // Copy the bytes from the stack into the value.
    for (uint8_t i = 0; i < length; i++)
    {
        #ifdef MVE_BIG_ENDIAN
            value.b[length - i - 1] = vm->stack[address + i];
        #else
            value.b[i] = vm->stack[address + i];
        #endif
    }

    vm->registers.all[reg] = value;
}


static void mve_op_sts(MVE_VM *vm)
{
// The register to load the value from.
    uint8_t reg = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg, "STS failed!", vm);

    // Address of the stack and length of the bytes.
    int32_t stack_address = mve_request_int32(vm);
    uint8_t length = mve_request_uint8(vm);

    uint32_t address = 0;

    // If the stack_address is negative then the end of the stack is used.
    // This is used when accessing scope memory.
    if (stack_address < 0)
        address = vm->stack_pointer - (-stack_address);
    else
        address = stack_address;

    MVE_ASSERT_STACK_ADDRESS(address, "STS failed!", vm);
    MVE_ASSERT_STACK_ADDRESS(address + length, "STS failed!", vm);

    // Copy the bytes from the register into the stack.
    for (uint8_t i = 0; i < length; i++)
    {
        #ifdef MVE_BIG_ENDIAN
            vm->stack[address + i] = vm->registers.all[reg].b[length - i - 1];
        #else
            vm->stack[address + i] = vm->registers.all[reg].b[i];
        #endif
    }
}


static void mve_op_ldi(MVE_VM *vm) 
{
    // The register to receive the value.
    uint8_t reg = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg, "LDI failed!", vm);
    
    // The amount of bytes to write.
    uint8_t length = mve_request_uint8(vm);

    MVE_Value value;
    value.i = 0;

    // Get the bytes to write into the register.
    for (uint8_t i = 0; i < length; i++)
    {
        #ifdef MVE_BIG_ENDIAN
            value.b[sizeof(MVE_Value) - i - 1] = mve_request_uint8(vm);
        #else
            value.b[i] = mve_request_uint8(vm);
        #endif
    }

    vm->registers.all[reg].i = value.i;
}


static void mve_op_mov(MVE_VM *vm) 
{
    uint8_t reg_to = mve_request_uint8(vm);
    uint8_t reg_from = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg_to, "MOV failed!", vm);
    MVE_ASSERT_REGISTER(reg_from, "MOV failed!", vm);

    vm->registers.all[reg_to].i = vm->registers.all[reg_from].i;
}


static void mve_op_neg(MVE_VM *vm) 
{
    uint8_t reg = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg, "NEG failed!", vm);

    vm->registers.all[reg].i = -vm->registers.all[reg].i;
}


static void mve_op_add(MVE_VM *vm) 
{
    uint8_t reg_result = mve_request_uint8(vm);
    uint8_t reg_op1 = mve_request_uint8(vm);
    uint8_t reg_op2 = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg_result, "ADD failed!", vm);
    MVE_ASSERT_REGISTER(reg_op1, "ADD failed!", vm);
    MVE_ASSERT_REGISTER(reg_op2, "ADD failed!", vm);

    vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i + vm->registers.all[reg_op2].i;
}


static void mve_op_sub(MVE_VM *vm) 
{
    uint8_t reg_result = mve_request_uint8(vm);
    uint8_t reg_op1 = mve_request_uint8(vm);
    uint8_t reg_op2 = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg_result, "SUB failed!", vm);
    MVE_ASSERT_REGISTER(reg_op1, "SUB failed!", vm);
    MVE_ASSERT_REGISTER(reg_op2, "SUB failed!", vm);

    vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i - vm->registers.all[reg_op2].i;
}


static void mve_op_mul(MVE_VM *vm) 
{
    uint8_t reg_result = mve_request_uint8(vm);
    uint8_t reg_op1 = mve_request_uint8(vm);
    uint8_t reg_op2 = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg_result, "MUL failed!", vm);
    MVE_ASSERT_REGISTER(reg_op1, "MUL failed!", vm);
    MVE_ASSERT_REGISTER(reg_op2, "MUL failed!", vm);

    vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i * vm->registers.all[reg_op2].i;
}


static void mve_op_div(MVE_VM *vm) 
{
    uint8_t reg_result = mve_request_uint8(vm);
    uint8_t reg_op1 = mve_request_uint8(vm);
    uint8_t reg_op2 = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg_result, "DIV failed!", vm);
    MVE_ASSERT_REGISTER(reg_op1, "DIV failed!", vm);
    MVE_ASSERT_REGISTER(reg_op2, "DIV failed!", vm);

    vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i / vm->registers.all[reg_op2].i;
}


static void mve_op_invoke(MVE_VM *vm) 
{
    // The function index according to the header declaration order.
    uint16_t function_index = mve_request_uint16(vm);

    MVE_ASSERT(vm->external_functions_count > function_index, vm, MVE_ERROR_EXTERNAL_FUNCTION_OUT_OF_RANGE, "INVOKE failed! Invalid function index.");

    void (*func) (MVE_VM *) = vm->external_functions[function_index];

    MVE_ASSERT(func != NULL, vm, MVE_ERROR_EXTERNAL_FUNCTION_OUT_OF_RANGE, "INVOKE failed! Function was not linked into the VM."); 
    
    func(vm);
}


static void mve_op_scope(MVE_VM *vm) 
{
    MVE_ASSERT(vm->scope_index + 1 < MVE_SCOPE_LIMIT, vm, MVE_ERROR_SCOPE_OUT_OF_RANGE, "SCOPE failed! There cannot be no more scopes than MVE_SCOPE_LIMIT.");

    vm->scope_index++;
    vm->scopes[vm->scope_index].stack_base = vm->stack_pointer;

    mve_load_scope_memory(vm);
}


static void mve_op_end(MVE_VM *vm) 
{
    MVE_ASSERT(vm->scope_index - 1 >= 0, vm, MVE_ERROR_SCOPE_OUT_OF_RANGE, "END failed! There is no scope to end.");

    vm->stack_pointer = vm->scopes[vm->scope_index].stack_base;

    uint32_t program_index = vm->scopes[vm->scope_index].program_index;

    if (program_index != 0) 
    {
        mve_jump_to_program_index(vm, program_index);
    }

    // Reset the program index of the scope, otherwise JMPs would also bring back to this location.
    vm->scopes[vm->scope_index].program_index = 0;
    vm->scope_index--;
}


static void mve_op_cmp(MVE_VM *vm) 
{
    uint8_t operation = mve_request_uint8(vm);

    MVE_ASSERT(operation <= MVE_CMP_LESSEQUAL, vm, MVE_ERROR_UNRECOGNIZED_CMP_OPERATION, "CMP failed! Unrecognized compare operation.");

    uint8_t reg_result = mve_request_uint8(vm);
    uint8_t reg_op1 = mve_request_uint8(vm);
    uint8_t reg_op2 = mve_request_uint8(vm); 

    MVE_ASSERT_REGISTER(reg_result, "CMP failed!", vm);
    MVE_ASSERT_REGISTER(reg_op1, "CMP failed!", vm);
    MVE_ASSERT_REGISTER(reg_op2, "CMP failed!", vm);

    switch (operation)
    {
    case MVE_CMP_EQUAL:
        vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i == vm->registers.all[reg_op2].i;
        break;
    case MVE_CMP_NOTEQUAL:
        vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i != vm->registers.all[reg_op2].i;
        break;
    case MVE_CMP_GREATER:
        vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i > vm->registers.all[reg_op2].i;
        break;
    case MVE_CMP_LESS:
        vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i < vm->registers.all[reg_op2].i;
        break;
     case MVE_CMP_GREATEREQUAL:
        vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i >= vm->registers.all[reg_op2].i;
        break;
    case MVE_CMP_LESSEQUAL:
        vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i <= vm->registers.all[reg_op2].i;
        break;
    default:
        break;
    }
}


static void mve_op_jmp(MVE_VM *vm) 
{
    uint8_t reg = mve_request_uint8(vm);
    MVE_ASSERT_REGISTER(reg, "JMP failed!", vm);

    uint32_t index = mve_request_uint32(vm);

    if (vm->registers.all[reg].i)
        mve_jump_to_program_index(vm, index);
}


static void mve_op_call(MVE_VM *vm) {

    uint32_t index = mve_request_uint32(vm);

    if (vm->scope_index + 1 >= MVE_SCOPE_LIMIT)
        return;
    
    // Set the program index of the next scope, so after ending the next scope, the VM will go back to this location.
    #ifdef MVE_LOCAL_PROGRAM
        vm->scopes[vm->scope_index + 1].program_index = vm->buffer_index;
    #else
        vm->scopes[vm->scope_index + 1].program_index = vm->program_index - MVE_BUFFER_SIZE + vm->buffer_index;
    #endif
    
    mve_jump_to_program_index(vm, index);
}


static void mve_op_and(MVE_VM *vm) 
{
    uint8_t reg_result = mve_request_uint8(vm);
    uint8_t reg_op1 = mve_request_uint8(vm);
    uint8_t reg_op2 = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg_result, "AND failed!", vm);
    MVE_ASSERT_REGISTER(reg_op1, "AND failed!", vm);
    MVE_ASSERT_REGISTER(reg_op2, "AND failed!", vm);

    vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i & vm->registers.all[reg_op2].i;
}


static void mve_op_orr(MVE_VM *vm) 
{
    uint8_t reg_result = mve_request_uint8(vm);
    uint8_t reg_op1 = mve_request_uint8(vm);
    uint8_t reg_op2 = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg_result, "ORR failed!", vm);
    MVE_ASSERT_REGISTER(reg_op1, "ORR failed!", vm);
    MVE_ASSERT_REGISTER(reg_op2, "ORR failed!", vm);

    vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i | vm->registers.all[reg_op2].i;
}


static void mve_op_not(MVE_VM *vm) 
{
    uint8_t reg_result = mve_request_uint8(vm);
    uint8_t reg_op1 = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg_result, "NOT failed!", vm);
    MVE_ASSERT_REGISTER(reg_op1, "NOT failed!", vm);

    vm->registers.all[reg_result].i = ~vm->registers.all[reg_op1].i;
}


static void mve_op_lsl(MVE_VM *vm) 
{
    uint8_t reg_result = mve_request_uint8(vm);
    uint8_t reg_op1 = mve_request_uint8(vm);
    uint8_t reg_op2 = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg_result, "LSL failed!", vm);
    MVE_ASSERT_REGISTER(reg_op1, "LSL failed!", vm);
    MVE_ASSERT_REGISTER(reg_op2, "LSL failed!", vm);

    vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i << vm->registers.all[reg_op2].i;
}


static void mve_op_xor(MVE_VM *vm) 
{
    uint8_t reg_result = mve_request_uint8(vm);
    uint8_t reg_op1 = mve_request_uint8(vm);
    uint8_t reg_op2 = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg_result, "XOR failed!", vm);
    MVE_ASSERT_REGISTER(reg_op1, "XOR failed!", vm);
    MVE_ASSERT_REGISTER(reg_op2, "XOR failed!", vm);

    vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i ^ vm->registers.all[reg_op2].i;
}


static void mve_op_lsr(MVE_VM *vm) 
{
    uint8_t reg_result = mve_request_uint8(vm);
    uint8_t reg_op1 = mve_request_uint8(vm);
    uint8_t reg_op2 = mve_request_uint8(vm);

    MVE_ASSERT_REGISTER(reg_result, "LSR failed!", vm);
    MVE_ASSERT_REGISTER(reg_op1, "LSR failed!", vm);
    MVE_ASSERT_REGISTER(reg_op2, "LSR failed!", vm);

    vm->registers.all[reg_result].i = vm->registers.all[reg_op1].i >> vm->registers.all[reg_op2].i;
}


#ifdef MVE_LOCAL_PROGRAM
bool mve_init(MVE_VM *vm, uint8_t *program) 
{
    vm->program_buffer = program;
#else
bool mve_init(MVE_VM *vm, void (*fun_load_next_block)(MVE_VM *, uint8_t *, uint32_t, uint32_t)) {
    vm->fun_load_next_block = fun_load_next_block;
#endif
    vm->program_index = 0;
    vm->is_running = false;
    vm->buffer_index = 0;
    vm->stack_pointer = 0;
    vm->scope_index = 0;

    for (uint16_t i = 0; i < MVE_EXTERNAL_FUNCTIONS_LIMIT; i++) {
        vm->external_functions[i] = NULL;
    }

    mve_load_next_block(vm);

    bool result = mve_load_header(vm);

    if (!result)
        MVE_ASSERT(result, vm, MVE_ERROR_INCOMPATIBLE_VERSION, "Incompatible program. Please upgrade your MicroVE into a newer version or compile your program to an old one.");

    return result;
}


void mve_link_function(MVE_VM *vm, const char *name, void (*function) (MVE_VM *)) 
{
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


void mve_start(MVE_VM *vm) 
{
    vm->is_running = true;

    // Reset the scopes because they are not set at runtime, unless on CALL instructions.
    // Without this, JMP instructions will misbehave.
    for (uint32_t i = 0; i < MVE_SCOPE_LIMIT; i++) {
        vm->scopes[i].program_index = 0;
        vm->scopes[i].stack_base = 0;
    }
}


void mve_run(MVE_VM *vm) 
{
    uint8_t next_operation = mve_request_uint8(vm);

    switch (next_operation)
    {
    case MVE_OP_LDR:
        mve_op_ldr(vm);
        break;
    case MVE_OP_STR:
        mve_op_str(vm);
        break;
    case MVE_OP_LDS:
        mve_op_lds(vm);
        break;
    case MVE_OP_STS:
        mve_op_sts(vm);
        break;
    case MVE_OP_LDI:
        mve_op_ldi(vm);
        break;
    case MVE_OP_MOV:
        mve_op_mov(vm);
        break;
    case MVE_OP_NEG:
        mve_op_neg(vm);
        break;
    case MVE_OP_INVOKE:
        mve_op_invoke(vm);
        break;
    case MVE_OP_ADD:
        mve_op_add(vm);
        break;
    case MVE_OP_SUB:
        mve_op_sub(vm);
        break;
    case MVE_OP_MUL:
        mve_op_mul(vm);
        break;
    case MVE_OP_DIV:
        mve_op_div(vm);
        break;
    case MVE_OP_SCOPE:
        mve_op_scope(vm);
        break;
    case MVE_OP_END:
        mve_op_end(vm);
        break;
    case MVE_OP_CMP:
        mve_op_cmp(vm);
        break;
    case MVE_OP_JMP:
        mve_op_jmp(vm);
        break;
    case MVE_OP_CALL:
        mve_op_call(vm);
        break;
    case MVE_OP_AND:
        mve_op_and(vm);
        break;
    case MVE_OP_ORR:
        mve_op_orr(vm);
        break;
    case MVE_OP_NOT:
        mve_op_not(vm);
        break;
    case MVE_OP_LSL:
        mve_op_lsl(vm);
        break;
    case MVE_OP_LSR:
        mve_op_lsr(vm);
        break;
    case MVE_OP_XOR:
        mve_op_xor(vm);
        break;
    case MVE_OP_EOP:
        mve_stop(vm);
        break;
    default:
        MVE_ASSERT(false, vm, MVE_ERROR_UNDEFINED_OP, "Undefined instruction! Code does not exist.");
        mve_stop(vm);
        break;
    }
}


bool mve_is_running(MVE_VM *vm) 
{
    return vm->is_running;
}


void mve_stop(MVE_VM *vm) 
{
    vm->is_running = false;
}
