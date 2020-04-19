#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

int main()
{
    /* Dereference kernel memory region */
    int* kernel_addr;
    int test;

    kernel_addr = (int*)(0x400000);
    test = *kernel_addr;

    return 0;
}
