#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#define MVE_EXTERNAL_FUNCTIONS_LIMIT 8

#define MVE_BUFFER_SIZE 128

#define MVE_STACK_SIZE 128
#define MVE_HEAP_SIZE 128

#define MVE_SCOPE_LIMIT 8

//#define MVE_BUFFER_SIZE 128

//#define MVE_LOCAL_PROGRAM

#define MVE_ERROR_LOG(vm, program_index, error_id, msg) printf("%s Program index: %u.", msg, program_index);

//#define MVE_USE_64BIT_TYPES
//#define MVE_BIG_ENDIAN

#include "../../src/mve.c"

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


void printArray(MVE_VM *vm) {

    uint32_t array = MVE_GET_MEMORY_UINT32(vm, 4);

    uint8_t* numbers = MVE_GET_STACK(vm, array);

    MVE_SET_MEMORY_INT32(vm, 2, 5000001);

    for (int i = 0; i < 30; i++)
        printf("%d \n", (int) numbers[i]);
}

void manipulateArray(MVE_VM *vm) {
    vm->memory[MEMORY_POINTER(vm) - 1] = 87;
    vm->memory[MEMORY_POINTER(vm) - 2] = 53;
}


void print(MVE_VM *vm) {

    int8_t c1 = MVE_GET_MEMORY_UINT8(vm, 1);
    int32_t c2 = MVE_GET_MEMORY_UINT32(vm, 5);
    int32_t c3 = MVE_GET_MEMORY_UINT32(vm, 9);

    char* str = MVE_GET_RELATIVE_STACK(vm, c3);

    //printf(" <%d %d %s>\n", c1, c2, str);
}


void printByte(MVE_VM *vm) {
    char c1 = MVE_GET_MEMORY_UINT8(vm, 1);
    char c2 = MVE_GET_MEMORY_UINT8(vm, 2);
    char c3 = MVE_GET_MEMORY_UINT8(vm, 3);

    printf("Hello: %c%c%c", c3, c2, c1);
}


void printString(MVE_VM *vm) {
    int adr = MVE_GET_MEMORY_INT32(vm, 4);
    char* str = MVE_GET_STACK(vm, adr);

    printf("Hello: %s", str);
}


long currentTimeMillis() {
  struct timeval time;
  gettimeofday(&time, NULL);

  return time.tv_sec * 1000 + time.tv_usec / 1000;
}




#define UI_ELEMENT_TYPE_TEXT					0
#define UI_ELEMENT_TYPE_SPINNER					1
#define UI_ELEMENT_TYPE_BUTTON					2
#define UI_ELEMENT_TYPE_INSTANT_BUTTON			3
#define UI_ELEMENT_TYPE_CHECKBOX				4

#define UI_PAGES_STACK_LENGTH 5
#define UI_PAGE_ELEMENTS_LENGTH 8
#define UI_ELEMENT_TEXT_LENGTH 256


typedef struct {
	char text[UI_ELEMENT_TEXT_LENGTH];
	uint8_t type;
	uint8_t data;
} ui_element_t;


typedef struct {
	ui_element_t elements[UI_PAGE_ELEMENTS_LENGTH];
	uint8_t count;
	uint8_t focus_index;
} ui_page_t;


typedef struct {
	ui_page_t pages[UI_PAGES_STACK_LENGTH];
	uint8_t count;
} ui_pages_stack_t;



int main() {

ui_pages_stack_t a;

    printf("%ld///", sizeof(a));


    uint8_t program[] = {   0x01, 0x00, // Verion Major
                            0x00, 0x00, // Version Minor
                            0x02, 0x00, 0x00, 0x00, // External functions count
                            'h', 'e', 'l', 'l', 'o', 0x00, // Function 1
                            'f', 'u', 'n', 'c', '2', 0x00,  // Function 2,
                            0x02, 0, 0, 0, 4, 5, // The main scope has 2 bytes of memory, 1 byte with the value 4, and other with 5.
                            MVE_OP_INVOKE, 0, 0,
                            MVE_OP_LDI, MVE_R3, 1, 0,
                            MVE_OP_LDI, MVE_R4, 1, 1,
                            MVE_OP_LDR, MVE_R0, MVE_R3, MVE_R4,
                            MVE_OP_LDR, MVE_R1, MVE_R4, MVE_R4,
                            MVE_OP_ADD, MVE_R0, MVE_R0, MVE_R1,
                            MVE_OP_MOV, MVE_R1, MVE_R0,
                            MVE_OP_NEG, MVE_R0,
                            MVE_OP_STR, MVE_R0, MVE_R3, MVE_R4,
                            MVE_OP_CALL, 63, 0, 0, 0,
                            MVE_OP_SCOPE, 0, 0, 0, 0,
                            MVE_OP_ADD, MVE_R0, MVE_R0, MVE_R1,
                            MVE_OP_END,
                            MVE_OP_LDI, MVE_R0, 1, 4,
                            MVE_OP_LDI, MVE_R1, 1, 5,
                            MVE_OP_CMP, MVE_CMP_GREATER, MVE_R2, MVE_R0, MVE_R1,
                            MVE_OP_JNZ, MVE_R2, 95, 0, 0, 0,
                            MVE_OP_INVOKE, 0, 0,
                            MVE_OP_NOT, MVE_R0, MVE_R0,
                            MVE_OP_EOP
    };

    MVE_VM vm;
    
    clock_t start_time = clock();

    mve_init(&vm, &load_next_block);
    //mve_init(&vm, program);

    mve_link_function(&vm, "hello", &print);
    mve_link_function(&vm, "print", &print);
    mve_link_function(&vm, "printByte", &printByte);
    mve_link_function(&vm, "printString", &printString);
    mve_link_function(&vm, "printArray", &printArray);
    mve_link_function(&vm, "manipulateArray", &manipulateArray);

    mve_start(&vm);

    printf("Size of the VM (bytes): %ld\n", sizeof(vm));

    while (mve_is_running(&vm)) {
        mve_run(&vm);
    }

    return 0;
}