#include "timing.h"
#include "main.h"
#include <stdint.h>


void stopwatch_reset(void)
{
    /* Enable DWT */
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CYCCNT = 0;
    /* Enable CPU cycle counter */
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

uint32_t stopwatch_getticks()
{
    return DWT->CYCCNT;
}

void stopwatch_delay(uint32_t ticks)
{
    uint32_t end_ticks = ticks;
    while (1)
    {
        if (stopwatch_getticks() >= ticks)
            break;
    }
}
