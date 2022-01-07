#ifndef TIMING_H
#define TIMING_H

#include <stdint.h>

/* Core Debug registers */

#define CYCCNTENA (1 << 0)
#define DWT_CYCCNT ((volatile uint32_t *)0xE0001004)
#define CPU_CYCLES *DWT_CYCCNT
#define CLK_SPEED 168000000 // EXAMPLE for CortexM4, EDIT as needed

#define STOPWATCH_START                                    \
    {                                                      \
        m_nStart = *((volatile unsigned int *)0xE0001004); \
    }
#define STOPWATCH_STOP                                    \
    {                                                     \
        m_nStop = *((volatile unsigned int *)0xE0001004); \
    }

 void stopwatch_reset(void);
 uint32_t stopwatch_getticks();

#endif