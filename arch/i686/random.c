#include "cpuid.h"


unsigned int get_random_number()
{
    printf("Output == %u", 0);

    unsigned int random = 0;
    // asm("rdrand %0;"
    // : "=r" (random)
    // );
    if(random == 0)
    {
        printf("random number\n");
    asm("rdseed %0;"
        : "=r" (random)
        );
        return random;
    }
    else
    {
        return random;

    }
}
