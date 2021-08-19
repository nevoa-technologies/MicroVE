#define MVE_EXTERNAL_FUNCTIONS_LIMIT 8

#define MVE_BUFFER_SIZE 128

#define MVE_STACK_SIZE 128
#define MVE_HEAP_SIZE 128

#define MVE_LOCAL_PROGRAM

//#define MVE_USE_64BIT_TYPES
//#define MVE_BIG_ENDIAN

#include "mve.c"
#include <stdio.h>

void load_next_block(MVE_VM *vm, uint8_t *buffer, uint32_t read_index, uint32_t read_length) {

    uint8_t program[] = { 0x01, 0x00, 0x03, 0x00, 0x02, 0x00, 0x00, 0x00, 0x68, 0x65, 0x6C, 0x6C, 0x6F, 0x00, 0x66, 0x75, 0x6E, 0x63, 0x32, 0x00 };

    memcpy(buffer, program + read_index, read_length);

    return;

    FILE *fileptr;
    long filelen;

    fileptr = fopen("test.bin", "rb");
    fseek(fileptr, 0, SEEK_END);
    filelen = ftell(fileptr);                   // Get the length of the file.

    fseek(fileptr, read_index, SEEK_SET);       // Go to the given index, to read.

    long length = filelen - read_index;         // Calculate the amount of bytes to read.

    if (read_length < length)                   // If the given length to read is smaller than the file length,
        length = read_length;                   // we use the given length.

    fread(buffer, length, 1, fileptr);          // Read the bytes from the given index to the smaller length, into the buffer.
    fclose(fileptr);             
}


void hello() {
    printf("Hello ");
}


int main() {

    uint8_t program[] = {   0x01, 0x00, // Verion Major
                            0x03, 0x00, // Version Minor
                            0x02, 0x00, 0x00, 0x00, // External functions count
                            'h', 'e', 'l', 'l', 'o', 0x00, // Function 1
                            'f', 'u', 'n', 'c', '2', 0x00  // Function 2
    };

    MVE_VM vm;
    //mve_init(&vm, &load_next_block);
    mve_init(&vm, program);

    mve_link_function(&vm, "hello", (void *)&hello);

    void (*func) () = vm.external_functions[0];
    func();

    mve_start(&vm);

    printf("%ld\n", sizeof(vm));

    return 0;
}