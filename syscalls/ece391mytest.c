#include <stdint.h>

#include "ece391support.h"
#include "ece391syscall.h"

int main ()
{
    int32_t fd, retval;
    uint8_t buf[20];

    ece391_fdputs(1, (uint8_t*) "attempting to run mytest2\n");
    retval = ece391_execute((uint8_t *) "mytest2");
    
    ece391_itoa(retval, buf, 10);
    ece391_fdputs(1, (uint8_t*) "return value: ");
    ece391_fdputs(1, buf);
    ece391_fdputs(1, (uint8_t*) "\n");

    return 0;
}
