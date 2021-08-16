#define MVE_EXTERNAL_FUNCTIONS_LIMIT 8

#define MVE_BUFFER_SIZE 128

#define MVE_STACK_SIZE 128
#define MVE_HEAP_SIZE 128

//#define MVE_USE_64BIT_TYPES

//#define MVE_BIG_ENDIAN

#include "mve.c"

void load_next(MVE_VM *vm, uint8_t *buffer, uint32_t read_index, uint32_t read_length)
{
    FILE *fileptr;
    long filelen;

    fileptr = fopen("test.bin", "rb");
    fseek(fileptr, 0, SEEK_END);
    filelen = ftell(fileptr);
    rewind(fileptr);

    fseek(fileptr, read_index, SEEK_SET);

    long length = filelen - read_index;

    if (read_length < length)
        length = read_length;

    fread(vm->program_buffer, length, 1, fileptr);
    fclose(fileptr);             
}


int hello(int x)
{
    printf("Hello ");
}


int main()
{
    MVE_VM vm;
    mve_init(&vm, &load_next);
    
    mve_link_function(&vm, "hello", (void *)&hello);

    void (*func) () = vm.external_functions[0];
    func();

    mve_start(&vm);

    printf("%d\n", sizeof(vm));

    return 0;
}