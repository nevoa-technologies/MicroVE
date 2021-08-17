#define MVE_EXTERNAL_FUNCTIONS_LIMIT 8

#define MVE_BUFFER_SIZE 128

#define MVE_STACK_SIZE 128
#define MVE_HEAP_SIZE 128

//#define MVE_USE_64BIT_TYPES
//#define MVE_BIG_ENDIAN

#include "mve.c"
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

    fread(buffer, length, 1, fileptr);          // Read the bytes from the given index to the smaller length, into the buffer.
    fclose(fileptr);             
}


int hello(int x) {
    printf("Hello ");
}


int main() {
    MVE_VM vm;
    mve_init(&vm, &load_next_block);
    
    mve_link_function(&vm, "hello", (void *)&hello);

    void (*func) () = vm.external_functions[0];
    func();

    mve_start(&vm);

    printf("%d\n", sizeof(vm));

    return 0;
}