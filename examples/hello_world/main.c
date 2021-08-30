#include "stdio.h"

#define MVE_EXTERNAL_FUNCTIONS_LIMIT 8

//#define MVE_BUFFER_SIZE 128

#define MVE_STACK_SIZE 128
#define MVE_HEAP_SIZE 128

#define MVE_SCOPE_LIMIT 8

#define MVE_LOCAL_PROGRAM

#define MVE_ERROR_LOG(vm, program_index, error_id, msg) printf("%s Program index: %u.", msg, program_index);

//#define MVE_USE_64BIT_TYPES
//#define MVE_BIG_ENDIAN

#include "../../src/mve.c"
#include <stdio.h>

void load_next_block(MVE_VM *vm, uint8_t *buffer, uint32_t read_index, uint32_t read_length) {
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


void hello(MVE_VM *vm) {

    char c1 = MVE_GET_STACK_INT8(vm, 1);
    char c2 = MVE_GET_STACK_INT8(vm, 2);

    printf("Hello: %d %d.", (int) c1, (int) c2);
}


int main() {
    
    uint8_t program[] = {   0x01, 0x00, // Verion Major
                            0x00, 0x00, // Version Minor
                            0x02, 0x00, 0x00, 0x00, // External functions count
                            'h', 'e', 'l', 'l', 'o', 0x00, // Function 1
                            'f', 'u', 'n', 'c', '2', 0x00,  // Function 2,
                            MVE_OP_PUSH, 2, 'a', 'b',
                            MVE_OP_POP, 2,
                            MVE_OP_PUSH, 2, 'c', 'd',
                            MVE_OP_POP, 1,
                            MVE_OP_PUSH, 2, 4, 5,
                            MVE_OP_INVOKE, 0, 0,
                            MVE_OP_LDR, MVE_R0, 1, 0, 0, 0, 1,
                            MVE_OP_LDR, MVE_R1, 2, 0, 0, 0, 1,
                            MVE_OP_ADD, MVE_R0, MVE_R0, MVE_R1,
                            MVE_OP_MOV, MVE_R1, MVE_R0,
                            MVE_OP_NEG, MVE_R0,
                            MVE_OP_STR, MVE_R0, 1, 0, 0, 0, 1,
                            MVE_OP_LDI, MVE_R0, 1, 0,
                            MVE_OP_CALL, 78, 0, 0, 0,
                            MVE_OP_SCOPE,
                            MVE_OP_ADD, MVE_R0, MVE_R0, MVE_R1,
                            MVE_OP_END,
                            MVE_OP_LDI, MVE_R0, 1, 4,
                            MVE_OP_LDI, MVE_R1, 1, 5,
                            MVE_OP_CMP, MVE_CMP_GREATER, MVE_R2, MVE_R0, MVE_R1,
                            MVE_OP_JMP, MVE_R2, 106, 0, 0, 0,
                            MVE_OP_INVOKE, 0, 0,
                            MVE_OP_NOT, MVE_R0, MVE_R0,
                            MVE_OP_EOP
    };

    MVE_VM vm;
    //mve_init(&vm, &load_next_block);
    mve_init(&vm, program);

    mve_link_function(&vm, "hello", &hello);

    mve_start(&vm);

    printf("%ld\n", sizeof(vm));

    while (mve_is_running(&vm)) {
        mve_run(&vm);
    }

    return 0;
}