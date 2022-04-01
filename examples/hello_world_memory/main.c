#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <sys/time.h>

#include "script.h"

#define MVE_EXTERNAL_FUNCTIONS_LIMIT 8

#define MVE_STACK_SIZE 128
#define MVE_MEMORY_SIZE 128

#define MVE_SCOPE_LIMIT 8

#define MVE_LOCAL_PROGRAM

//#define MVE_ERROR_LOG(vm, program_index, error_id, msg) printf("%s Program index: %u.", msg, program_index);

//#define MVE_USE_64BIT_TYPES
//#define MVE_BIG_ENDIAN

#include "../../src/mve.c"

void hello(MVE_VM *vm) {

    uint32_t c1 = MVE_GET_MEMORY_UINT32(vm, 4);

    //printf("Hello: %d. \n", c1);
}


long currentTimeMillis() {
  struct timeval time;
  gettimeofday(&time, NULL);

  return time.tv_sec * 1000 + time.tv_usec / 1000;
}


int main() {

    MVE_VM vm;
    
    mve_init(&vm, (uint8_t *) script);

    mve_link_function(&vm, "hello", &hello);
    mve_link_function(&vm, "print", &hello);

    mve_start(&vm);

    while (mve_is_running(&vm)) {
        mve_run(&vm);
    }

    return 0;
}