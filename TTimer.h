#ifndef _TTIMER_H_
#define _TTIMER_H_

#include <xc.h>
#include <pic18f4321.h>
#include "Utils.h"

#define TI_NUMTIMERS 10 // Amount of timers for this TAD, can be modified.
#define NO_TIMERS_AVAILABLE 63

void TiInit(void);
void Timer0_ISR(void);
BYTE TiGetTimer(void);
unsigned int TiGetTics(BYTE timerId);
void TiResetTics(BYTE timerId);

#endif
