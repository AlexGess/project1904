#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "p1904_random.h"


void
p1904_init_random(void)
{
    srand(time(NULL));
}

uint32_t
p1904_random_uint_in_range(uint32_t lower, uint32_t upper)
{
    return ((rand() % (upper - lower + 1)) + lower);
}
